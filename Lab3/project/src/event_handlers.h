/*
 * ===========================================================================
 * File:        event_handlers.h
 * Description: Event handling system function declarations
 * Date:        May, 2025
 * ===========================================================================
 */

#ifndef EVENT_HANDLERS_H
#define EVENT_HANDLERS_H

#include <stdint.h>
#include "system_config.h"
#include "queue.h"


/* ========================================================================== */
/*                           EXTERNAL VARIABLES                               */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                    Communication and Buffer Variables                      */
/* -------------------------------------------------------------------------- */
extern Queue rx_queue;
extern char uart_buffer[UART_BUFFER_SIZE];
extern uint8_t uart_buffer_index;

/* -------------------------------------------------------------------------- */
/*                        System State and Mode Variables                     */
/* -------------------------------------------------------------------------- */
extern system_mode_t current_mode;
extern display_mode_t display_mode;
extern uint32_t data_update_period;

/* -------------------------------------------------------------------------- */
/*                         Alert and Status Variables                         */
/* -------------------------------------------------------------------------- */
extern uint8_t alert_active;
extern float last_temperature;
extern float last_humidity;

/* -------------------------------------------------------------------------- */
/*                        Timer and Control Variables                         */
/* -------------------------------------------------------------------------- */
extern volatile uint32_t button_press_count;
extern volatile uint32_t data_timer_count;
extern volatile uint32_t button_last_press_time;

/* -------------------------------------------------------------------------- */
/*                         Flag Control Variables                             */
/* -------------------------------------------------------------------------- */
extern volatile uint8_t update_led_flag;
extern volatile uint8_t button_pressed_flag;
extern volatile uint8_t read_data_flag;


/* ========================================================================== */
/*                        INTERRUPT CALLBACK FUNCTIONS                        */
/* ========================================================================== */

/**
 * @brief UART receive interrupt callback function
 * 
 * @param c Received character from UART
 * 
 * Processes incoming UART characters and adds valid ASCII characters
 * to the receive queue for later processing in the main loop.
 */
void uart_receive_callback(uint8_t c);

/**
 * @brief Button press interrupt callback function
 * 
 * @param status Button status (pressed/released)
 * 
 * Handles button press events with debouncing and sets appropriate
 * flags for processing in the main loop when system is authenticated.
 */
void button_press_callback(int status);

/**
 * @brief Timer interrupt callback function
 * 
 * Manages periodic timer events including data reading scheduling,
 * LED blinking control for alert mode, and main system timing.
 */
void timer_callback(void);


/* ========================================================================== */
/*                          EVENT HANDLER FUNCTIONS                           */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                        UART Data Processing                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Process queued UART data from main loop
 * 
 * Dequeues characters from the UART receive queue and routes them to
 * appropriate handlers based on current authentication status.
 */
void handle_uart_data(void);

/**
 * @brief Handle command input processing for authenticated users
 * 
 * @param c Input character received from UART
 * 
 * Processes character input for command building, handles special keys
 * (backspace, enter, Ctrl+C, Ctrl+X), and manages command buffer.
 */
void handle_command_input(uint8_t c);

/**
 * @brief Process complete UART commands
 * 
 * @param command Null-terminated command string
 * 
 * Parses and executes system commands including frequency adjustment,
 * display mode changes, status queries, and system information display.
 */
void handle_uart_command(char *command);

/* -------------------------------------------------------------------------- */
/*                        Hardware Event Processing                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Handle button press events from main loop
 * 
 * Processes button press events including mode switching, alert management,
 * and periodic frequency updates from AEM-based calculations.
 */
void handle_button_press(void);

/**
 * @brief Handle periodic data reading operations
 * 
 * Performs DHT11 sensor readings, updates system variables, displays
 * data according to current mode, and checks alert/panic conditions.
 */
void handle_data_reading(void);

#endif // EVENT_HANDLERS_H