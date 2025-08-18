/*
 * ===========================================================================
 * File:        system_control.c
 * Description: System control and management function implementations
 * Date:        May, 2025
 * ===========================================================================
 */

/* ========================================================================== */
/*                              INCLUDES                                      */
/* ========================================================================== */
#include "system_control.h"
#include "uart.h"
#include "timer.h"
#include "gpio.h"
#include "delay.h"
#include "queue.h"
#include "system_config.h"
#include "event_handlers.h"
#include "console.h"
#include <stdio.h>
#include <string.h>


/* ========================================================================== */
/*             SYSTEM INITIALIZATION AND MANAGEMENT FUNCTIONS                 */
/* ========================================================================== */

/**
 * @brief Initialize all system components and peripherals
 */
void system_init(void)
{
    // Initialize queue for UART input buffering
    queue_init(&rx_queue, QUEUE_SIZE);

    // Initialize UART communication subsystem
    uart_init(UART_BAUD_RATE);                   // Configure baud rate
    uart_set_rx_callback(uart_receive_callback); // Set ISR callback for incoming data
    uart_enable();                               // Enable UART peripheral

    // Configure LED_PIN (PB_0) as an output pin for alert indicators
    gpio_set_mode(LED_PIN, Output);

    // Configure BUTTON_PIN (PC_13) for user input with interrupt capability
    gpio_set_mode(BUTTON_PIN, PullUp);                    // Enable internal pull-up resistor
    gpio_set_trigger(BUTTON_PIN, Falling);                // Configure falling edge trigger
    gpio_set_callback(BUTTON_PIN, button_press_callback); // Set ISR callback

    // Configure TOUCH_SENSOR_PIN for touch input with interrupt capability
    gpio_set_mode(TOUCH_SENSOR_PIN, Input);                     // Floating input (no pull resistor needed)
    gpio_set_trigger(TOUCH_SENSOR_PIN, Rising);                 // Configure rising edge trigger (touch detection)
    gpio_set_callback(TOUCH_SENSOR_PIN, button_press_callback); // Set ISR callback

    // Initialize DHT11 sensor pin configuration
    gpio_set_mode(DHT11_PIN, PullUp); // Configure with pull-up for data line
    gpio_set(DHT11_PIN, ON);          // Set initial high state

    // Initialize Timer subsystem for periodic operations
    timer_init(TIMER_INTERVAL);         // Configure timer interval
    timer_set_callback(timer_callback); // Set timer ISR callback
    timer_disable();                    // Start with timer disabled until authentication is complete
}

/**
 * @brief Perform software system reset
 */
void software_reset(void)
{
    // Use console functions instead of basic uart_print
    print_animated_header("🔄 SYSTEM RESET", "Performing System Restart");
    print_centered_line("System will restart in 3 seconds...", COLOR_YELLOW);
    print_footer("Please wait for system initialization");
    
    // Allow time for message transmission to complete
    delay_ms(100);

    // Execute system reset using ARM Cortex-M NVIC
    NVIC_SystemReset();
}

/**
 * @brief Update sensor data acquisition frequency based on AEM input
 */
void update_data_frequency_from_aem(void)
{
    // Calculate AEM string length to determine digit availability
    int len = strlen(student_aem);

    // Process multi-digit AEMs for frequency calculation
    if (len >= 2)
    {
        // Extract last two digits from AEM string
        int last_digit = student_aem[len - 1] - '0';        // Convert ASCII to integer
        int second_last_digit = student_aem[len - 2] - '0'; // Convert ASCII to integer
        uint32_t new_freq = last_digit + second_last_digit; // Calculate sum

        // Apply frequency bounds checking and clamping
        if (new_freq < MIN_PERIOD)
            new_freq = MIN_PERIOD; // Enforce minimum period limit
        if (new_freq > MAX_PERIOD)
            new_freq = MAX_PERIOD; // Enforce maximum period limit

        // Update global data acquisition period
        data_update_period = new_freq;

        // Confirm new frequency via UART output
        char freq_msg[80];
        snprintf(freq_msg, sizeof(freq_msg), 
                "🔄 AEM-based frequency: %d seconds (digits: %d+%d)", 
                (int)data_update_period, second_last_digit, last_digit);
        uart_print_with_restore(freq_msg);
    }
    else
    {
        // Handle single-digit AEM case - skip frequency update
        uart_print_with_restore("❗ Single-digit AEM detected, frequency update skipped");
    }
}


/* ========================================================================== */
/*                         ALERT MANAGEMENT FUNCTIONS                         */
/* ========================================================================== */

/**
 * @brief Disable alert mode and return to normal operation
 */
void disable_alert_mode(void)
{
    // Turn off alert LED indicator
    gpio_set(LED_PIN, OFF);

    // Reset LED timing variables to initial state
    led_timer_count = 0;
    update_led_flag = 0;

    // Clear alert active status flag
    alert_active = 0;
}

/**
 * @brief Evaluate environmental conditions for alert threshold violations
 */
void check_alert_conditions(float temp, float humidity)
{
    // Static variable maintains count between function calls
    static uint8_t normal_consecutive_count = 0;

    // Only process alert conditions when system is in alert mode
    if (current_mode == MODE_B_ALERT)
    {
        // Check for threshold violations (OR logic - either parameter can trigger)
        if (temp > TEMP_ALERT_THRESHOLD || humidity > HUMIDITY_ALERT_THRESHOLD)
        {
            // Immediate alert activation upon threshold violation
            if(!alert_active)
                uart_print_with_restore("🚨 ALERT ACTIVATED - Environmental thresholds exceeded!\r\n");
            alert_active = 1;             // Set alert active flag
            normal_consecutive_count = 0; // Reset normal condition counter
        }
        else
        {
            // Conditions are normal, handle alert state if active
            if (alert_active)
            {
                // Increment consecutive normal reading counter
                if (++normal_consecutive_count >= NORMAL_CONSECUTIVE_COUNT)
                {
                    // Disable alert after sufficient consecutive normal readings
                    disable_alert_mode();
                    normal_consecutive_count = 0; // Reset counter for next alert cycle
                    uart_print_with_restore("✅ Alert deactivated - conditions normalized\r\n");
                }
            }
        }
    }
}

/**
 * @brief Evaluate environmental conditions for panic threshold violations
 */
void check_panic_conditions(float temp, float humidity)
{
    // Static variable maintains count between function calls 
    static uint8_t panic_consecutive_count = 0;

    // Check for critical threshold violations (OR logic - either parameter can trigger)
    if (temp > TEMP_PANIC_THRESHOLD || humidity > HUMIDITY_PANIC_THRESHOLD)
    {
        // Increment consecutive panic condition counter
        if (++panic_consecutive_count >= PANIC_CONSECUTIVE_COUNT)
        {
            // Execute panic reset after consecutive violations confirmed
            uart_print_with_restore("🚨 PANIC RESET - CRITICAL CONDITIONS DETECTED! 🚨\r\n");
            software_reset(); // Execute immediate system reset
        }
        else
        {
            uart_printf("⚠️  WARNING: Panic condition %d/%d detected!\r\n", 
                       panic_consecutive_count, PANIC_CONSECUTIVE_COUNT);
        }
    }
    else
    {
        // Reset panic counter when conditions return to normal
        panic_consecutive_count = 0;
    }
}
