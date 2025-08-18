/*
 * ===========================================================================
 * File:        console.h
 * Description: Console display and terminal control function declarations
 * Date:        May, 2025
 * ===========================================================================
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>


/* ========================================================================== */
/*                           EXTERNAL VARIABLES                               */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                    Communication and Buffer Variables                      */
/* -------------------------------------------------------------------------- */
extern char uart_buffer[];
extern uint8_t uart_buffer_index;


/* ========================================================================== */
/*                        CONSOLE DISPLAY FUNCTIONS                           */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                    Menu and Display Management                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Display main system menu to user
 * 
 * Presents the main operational menu with available system commands and
 * options for environmental monitoring system control.
 */
void display_menu(void);

/**
 * @brief Display initial authentication prompt to user
 * 
 * Presents the initial system startup message and password entry prompt
 * via UART, initializing the authentication state for user login.
 */
void authenticate_user_display(void);

/* -------------------------------------------------------------------------- */
/*                    UART Output Formatting Functions                        */
/* -------------------------------------------------------------------------- */

/**
 * @brief Formatted printing via UART with printf-style arguments
 * 
 * @param format Format string (printf-style)
 * @param ... Variable arguments for format string
 * 
 * Provides printf-style formatted output functionality through UART
 * communication interface with variable argument support.
 */
void uart_printf(const char *format, ...);

/**
 * @brief Print message with input buffer restoration
 * 
 * @param message Message string to display
 * 
 * Clears current line, prints the specified message, and restores any
 * partial user input that was in progress, maintaining input continuity.
 */
void uart_print_with_restore(const char* message);

/* -------------------------------------------------------------------------- */
/*                    Terminal Control Functions                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Clear terminal screen and redisplay menu
 * 
 * Uses ANSI escape codes to clear the entire terminal screen, move cursor
 * to home position, and redisplay the main system menu for clean interface.
 */
void clear_terminal(void);

/**
 * @brief Terminate program gracefully
 * 
 * Performs complete system shutdown including cleanup operations, hardware
 * peripheral disabling, interrupt disabling, and program termination with
 * appropriate user notification.
 */
void terminate_program(void);


/* ========================================================================== */
/*                              ENHACED DISPLAY FUNCTIONS                     */
/* ========================================================================== */

/**
 * @brief Print a centered line of text with color support
 * 
 * @param text Text to center and display
 * @param color ANSI color code for text color
 * 
 * Centers the specified text within a defined width, applying the given
 * color formatting for visual emphasis in terminal output.
 */
void print_centered_line(const char* text, const char* color);

/**
 * @brief Print a formatted line with icon and description
 * 
 * @param key Key character for command
 * @param icon Emoji or icon to display
 * @param description Description text for the command
 * @param color ANSI color code for text color
 * 
 * Displays a menu item with a key, icon, and description, applying color
 * formatting and ensuring proper alignment within the terminal.
 */
void print_menu_item(const char* key, const char* icon, const char* description, const char* color);

/**
 * @brief Print animated header with gradient effect
 * 
 * @param title Main title text to display
 * @param subtitle Optional subtitle text for additional context
 * 
 * Displays a visually appealing header with the specified title and
 * optional subtitle, using color formatting and centered alignment.
 */
void print_animated_header(const char* title, const char* subtitle);

/**
 * @brief Print footer with additional information or timestamp
 * 
 * @param info Optional information to display in footer
 * 
 * Displays a footer line with the specified information, applying color
 * formatting and centered alignment for visual consistency.
 */
void print_footer(const char* info);



#endif // CONSOLE_H