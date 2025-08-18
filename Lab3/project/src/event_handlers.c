/*
 * ===========================================================================
 * File:        event_handlers.c
 * Description: Event handling system function implementations
 * Date:        May, 2025
 * ===========================================================================
 */

/* ========================================================================== */
/*                              INCLUDES                                      */
/* ========================================================================== */
#include "event_handlers.h"
#include "uart.h"
#include "gpio.h"
#include "queue.h"
#include "dht11.h"
#include "user_auth.h"
#include "system_config.h"
#include "delay.h"
#include "system_control.h"
#include "console.h"
#include "console.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static void handle_display_mode_change(void);
static void print_detailed_status(void);
static void print_status(void);


/* ========================================================================== */
/*                        INTERRUPT CALLBACK FUNCTIONS                        */
/* ========================================================================== */

/**
 * @brief UART receive interrupt callback function
 */
void uart_receive_callback(uint8_t c)
{
    // Process only valid ASCII characters (0 to 127)
    if (c <= 0x7F)
    {
        // Add the received character to the RX queue
        queue_enqueue(&rx_queue, c);
    }
}

/**
 * @brief Button press interrupt callback function
 */
void button_press_callback(int status)
{
    (void)status;

    // Only process button presses if the system is authenticated
    if (!auth_is_authenticated())
    {
        return;
    }

    uint32_t current_time = data_timer_count;
    if (current_time - button_last_press_time >= BUTTON_DEBOUNCE_TIME)
    {
        button_pressed_flag = 1;
        button_last_press_time = current_time;
    }
}

/**
 * @brief Timer interrupt callback function
 */
void timer_callback(void)
{
    // Increment main timer counter
    data_timer_count++;

    // Set data reading flag based on frequency
    if (data_timer_count % data_update_period == 0)
    {
        read_data_flag = 1;
    }

    // Handle LED blinking timer for alert mode
    if (current_mode == MODE_B_ALERT && alert_active)
    {
        led_timer_count++;
        if (led_timer_count >= LED_BLINK_INTERVAL)
        { 
            led_timer_count = 0;
            update_led_flag = 1;
        }
    }
}


/* ========================================================================== */
/*                          EVENT HANDLER FUNCTIONS                           */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                        UART Data Processing                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Process queued UART data from main loop
 */
void handle_uart_data(void)
{
    uint8_t c;

    while (queue_dequeue(&rx_queue, &c))
    {
        if (!auth_is_authenticated())
        {
            handle_auth_input(c);
        }
        else
        {
            handle_command_input(c);
        }
    }
}

/**
 * @brief Handle command input processing for authenticated users
 */
void handle_command_input(uint8_t c)
{
    if (c == '\r' || c == '\n')
    {
        if (uart_buffer_index > 0)
        {
            uart_buffer[uart_buffer_index] = '\0';
            uart_print("\r\n");
            uart_buffer_index = 0;
            handle_uart_command(uart_buffer);
        }
    }

    else if (c == 127 || c == 8) // Backspace
    {
        if (uart_buffer_index > 0)
        {
            uart_buffer_index--;
            uart_print("\b \b");
        }
    }

    else if (uart_buffer_index < UART_BUFFER_SIZE - 1 && (c >= 'a' && c <= 'z'))
    {
        uart_buffer[uart_buffer_index++] = c;
        uart_tx(c); // Echo character
    }

    // Handle Ctrl+C (ASCII ETX - End of Text)
    else if (c == KEY_CTRL_C_CLEAR)
    {
        clear_terminal(); /* Clear the terminal screen */
    }

    // Handle Ctrl+X (ASCII CAN - Cancel)
    else if (c == KEY_CTRL_X_TERMINATE)
    {
        terminate_program(); /* Terminate the application */
    }
}


/**
 * @brief Process complete UART commands
 */
void handle_uart_command(char *command)
{
    if (strcmp(command, "a") == 0)
    {
        if (data_update_period > MIN_PERIOD)
        {
            data_update_period--;
            char msg[128];
            sprintf(msg, "📈 Frequency increased - New interval: %d seconds", (int)data_update_period);
            uart_print_with_restore(msg);
        }
        else
        {
            uart_print_with_restore("❗Minimum frequency limit reached (2 seconds)");
        }
    }
    else if (strcmp(command, "b") == 0)
    {
        if (data_update_period < MAX_PERIOD)
        {
            data_update_period++;
            char msg[128];
            sprintf(msg, "📉 Frequency decreased - New interval: %d seconds", (int)data_update_period);
            uart_print_with_restore(msg);
        }
        else
        {
            uart_print_with_restore("❗Maximum frequency limit reached (10 seconds)");
        }
    }
    else if (strcmp(command, "c") == 0)
    {
        handle_display_mode_change();
    }
    else if (strcmp(command, "d") == 0)
    {
        print_detailed_status();
    }
    else if (strcmp(command, "status") == 0)
    {
        print_status();
    }
    else
    {
        uart_print_with_restore("❌ Unknown command! Available: a, b, c, d, status");
    }
}


/* -------------------------------------------------------------------------- */
/*                        Hardware Event Processing                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Handle button press events from main loop
 */
void handle_button_press(void)
{
    button_press_count++;

    // Toggle mode
    current_mode = (current_mode == MODE_A_NORMAL) ? MODE_B_ALERT : MODE_A_NORMAL;

    char mode_msg[64];
    const char* mode_icon = (current_mode == MODE_A_NORMAL) ? "🟢" : "🟡";
    const char* mode_name = (current_mode == MODE_A_NORMAL) ? "Normal" : "Alert";
    
    sprintf(mode_msg, "%s Switched to Mode %c (%s)\r\n", 
            mode_icon, (current_mode == MODE_A_NORMAL) ? 'A' : 'B', mode_name);
    uart_print_with_restore(mode_msg);

    // Reset LED state when switching modes
    if (current_mode == MODE_A_NORMAL && alert_active)
    {
        disable_alert_mode();
        uart_print_with_restore("🟢 Alert mode disabled - switched to Normal Mode\r\n");
    }

    // Every 3rd press, update frequency from AEM
    if (button_press_count % 3 == 0)
    {
        uart_print_with_restore("🔄 Updating frequency based on AEM...\r\n");
        update_data_frequency_from_aem();
    }
}

/**
 * @brief Handle periodic data reading operations
 */
void handle_data_reading(void)
{
    float temperature, humidity;
    if (read_DHT11_Sensor(&temperature, &humidity))
    {
        last_temperature = temperature;
        last_humidity = humidity;

        char msg[128];
        switch (display_mode)
        {
        case DISPLAY_TEMP:
            sprintf(msg, "🔥 Temperature: %.1f C\r\n", temperature);
            break;
        case DISPLAY_HUMIDITY:
            sprintf(msg, "💧 Humidity: %.1f%%\r\n", humidity);
            break;
        case DISPLAY_BOTH:
            sprintf(msg, "📊 Temp: %.1f C | Humidity: %.1f%%\r\n", temperature, humidity);
            break;
        }
 
        uart_print_with_restore(msg);

        check_alert_conditions(temperature, humidity);
        check_panic_conditions(temperature, humidity);
    }
    else
    {
        uart_print_with_restore("❌ DHT11 sensor read error\r\n");
    }
}


/* ========================================================================== */
/*                        PRIVATE HELPER FUNCTIONS                            */
/* ========================================================================== */

/**
 * @brief Handle display mode cycling and feedback
 */
static void handle_display_mode_change(void)
{
    display_mode = (display_mode + 1) % 3;
    const char *mode_names[] = {
        "🔥  Temperature Only", 
        "💧 Humidity Only", 
        "📊 Both Sensors"
    };
    char msg[128];
    sprintf(msg, "🔄 Display mode changed to: %s", mode_names[display_mode]);
    uart_print_with_restore(msg);
}

/**
 * @brief Display detailed system status with enhanced formatting
 */
static void print_detailed_status(void)
{
    uart_print("\r\n");
    print_animated_header("📊 SYSTEM STATUS REPORT 📊", "Current Environmental Data");
    
    char temp_line[64], humid_line[64], mode_line[64], freq_line[64], button_line[64];
    sprintf(temp_line, "🔥 Temperature: %.1f C", last_temperature);
    sprintf(humid_line, "💧 Humidity: %.1f%%", last_humidity);
    
    const char* mode_icon = (current_mode == MODE_A_NORMAL) ? "🟢" : "🟡";
    const char* mode_name = (current_mode == MODE_A_NORMAL) ? "Normal" : "Alert";
    sprintf(mode_line, "%s Current Mode: %c (%s)", mode_icon, 
            (current_mode == MODE_A_NORMAL) ? 'A' : 'B', mode_name);
    sprintf(freq_line, "⏰ Read Frequency: %d seconds", (int)data_update_period);
    sprintf(button_line, "🔘 Button Presses: %d", (int)button_press_count);
    
    print_centered_line(temp_line, COLOR_RED);
    print_centered_line(humid_line, COLOR_CYAN);
    print_centered_line("", COLOR_RESET); // Empty line for spacing
    print_centered_line(mode_line, COLOR_GREEN);
    print_centered_line(freq_line, COLOR_CYAN);
    print_centered_line(button_line, COLOR_YELLOW);
    
    print_footer("Real-time environmental monitoring data");
}

/**
 * @brief Display compact system status on single line
 */
static void print_status(void)
{
    const char* mode_icon = (current_mode == MODE_A_NORMAL) ? "🟢" : "🟡";
    char status_msg[256];
    sprintf(status_msg, "📍 Mode: %s%c | 🔥 %.1f C | 💧 %.1f%% | 🔘 %d presses",
            mode_icon, (current_mode == MODE_A_NORMAL) ? 'A' : 'B',
            last_temperature, last_humidity, (int)button_press_count);
    uart_print_with_restore(status_msg);
}

