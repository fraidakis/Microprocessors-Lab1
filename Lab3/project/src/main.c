/*
 * ===========================================================================
 * File:        main.c
 * Project:     Lab3 Embedded System Project
 * Description: Main application entry point and event-driven control loop
 * Date:        May, 2025
 *
 * This program implements a dual-mode environmental monitoring system using
 * a DHT11 sensor. Features include real-time temperature and humidity reading,
 * configurable sampling frequency, alert mode with LED indicators, and UART
 * command interface for remote control.
 *
 * System Modes:
 * - Normal Mode: Standard periodic monitoring
 * - Alert Mode: Enhanced monitoring with visual alerts
 *
 * Key Features:
 * - DHT11 sensor data acquisition
 * - Button control with press counting
 * - Alert system with LED feedback
 * - UART command interface
 * ===========================================================================
 */



/* ========================================================================== */
/*                              INCLUDES                                      */
/* ========================================================================== */
#include "system_config.h"  // System configuration parameters and constants
#include "system_control.h" // System initialization and control functions
#include "console.h"        // Console I/O and user authentication functions
#include "event_handlers.h" // Event handler function declarations
#include "gpio.h"           // General-Purpose Input/Output pin control functions
#include "queue.h"          // Queue data structure implementation for buffering


/* ========================================================================== */
/*                           GLOBAL VARIABLES                                 */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                        System State Variables                              */
/* -------------------------------------------------------------------------- */
system_mode_t current_mode = MODE_A_NORMAL;       // Current operational mode (MODE_A_NORMAL, MODE_B_ALERT)
uint32_t data_update_period = DEFAULT_PERIOD;     // Period for DHT11 data rading in seconds
display_mode_t display_mode = DISPLAY_BOTH;       // Sensor data display mode (temperature, humidity, or both)
float last_temperature = 0.0f;                    // Most recently read temperature value in degrees Celsius
float last_humidity = 0.0f;                       // Most recently read humidity value as percentage
uint8_t alert_active = 0;                         // Boolean flag: 1 if alert condition is active, 0 otherwise
auth_state_t current_state = AUTH_STATE_PASSWORD; // Current authentication state (AUTH_STATE_PASSWORD, AUTH_STATE_AEM, AUTH_STATE_AUTHENTICATED)
char student_aem[AUTH_LENGTH + 1] = {0};           // Storage buffer for authenticated student AEM (student ID) 
volatile uint32_t button_press_count = 0;         // Total count of button presses (volatile for ISR access)

/* -------------------------------------------------------------------------- */
/*                        Interrupt Service Routine Flags                     */
/* -------------------------------------------------------------------------- */
// These flags are set by ISRs and cleared by the main loop after processing.
// All flags are volatile to ensure proper access between ISR and main contexts.
volatile uint8_t button_pressed_flag = 0; // Set by button ISR when button press is detected
volatile uint8_t read_data_flag = 0;      // Set by timer ISR when periodic data reading is due
volatile uint8_t update_led_flag = 0;     // Set by timer ISR when LED state should be toggled

/* -------------------------------------------------------------------------- */
/*                        Timer-related Variables                             */
/* -------------------------------------------------------------------------- */
volatile uint32_t data_timer_count = 0;       // Counter incremented by timer ISR for data acquisition timing
volatile uint32_t led_timer_count = 0;        // Counter incremented by timer ISR for LED blink timing in alert mode
volatile uint32_t button_last_press_time = 0; // Timestamp of the last button press event

/* -------------------------------------------------------------------------- */
/*                        UART Communication Variables                        */
/* -------------------------------------------------------------------------- */
Queue rx_queue;                            // UART receive queue for buffering incoming characters
char uart_buffer[UART_BUFFER_SIZE] = {0}; // Buffer for storing global command input from UART
uint8_t uart_buffer_index = 0;            // Current write position index in global_cmd_buffer


/* ========================================================================== */
/*                              MAIN FUNCTION                                 */
/* ========================================================================== */

/**
 * @brief Main application entry point and event loop.
 *
 * This function serves as the primary control flow for the embedded system:
 * 1. Initializes all hardware peripherals and software components
 * 2. Performs user authentication and displays system information
 * 3. Enters an infinite event-driven loop that:
 *    - Uses __WFI() to enter low-power mode between events
 *    - Processes interrupt-signaled events in priority order:
 *      • Button press events (user input)
 *      • Periodic sensor data acquisition
 *      • LED state changes (visual feedback/alerts)
 *      • UART communication processing
 *
 * The system operates in an interrupt-driven fashion where ISRs set flags
 * that are processed by this main loop, ensuring responsive real-time behavior
 */
int main(void)
{
    // Initialize all system hardware peripherals and software components
    system_init();

    // Perform user authentication sequence and display initial system status
    authenticate_user_display();

    /****************************   Main Event Loop   ******************************/
    while (1)
    {
        // Enter low-power sleep mode until an interrupt occurs
        __WFI(); // Wait For Interrupt - reduces power consumption

        // Process button press events
        if (button_pressed_flag)
        {
            button_pressed_flag = 0; 
            handle_button_press();  
        }

        // Process periodic sensor data acquisition events
        if (read_data_flag)
        {
            read_data_flag = 0;   
            handle_data_reading();
        }

        // Process LED state updates for alert mode
        if (update_led_flag)
        {
            update_led_flag = 0; 
            gpio_toggle(LED_PIN); 
        }

        // Process incoming UART communication data
        handle_uart_data(); // Check queue and process any received commands
    }

    // Note: Embedded applications typically do not exit main.
    // The program runs indefinitely until a reset or termination signal is received.
}
