/*
 * ===========================================================================
 * File:        console.c
 * Description: Console display and terminal control function implementations
 * Date:        May, 2025
 * ===========================================================================
 */

/* ========================================================================== */
/*                              INCLUDES                                      */
/* ========================================================================== */
#include "console.h"
#include "uart.h"
#include "user_auth.h"
#include "platform.h"
#include "gpio.h"
#include "timer.h"
#include "system_config.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>


/* ========================================================================== */
/*                        CONSOLE DISPLAY FUNCTIONS                           */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                        Menu and Display Management                         */
/* -------------------------------------------------------------------------- */

/**
 * @brief Display the main menu with available commands and options
 */
void display_menu(void)
{
    print_animated_header("🌿 ENVIRONMENTAL MONITORING SYSTEM 🌿", "Advanced Sensor Control Interface");
    
    print_centered_line("📋 AVAILABLE COMMANDS", COLOR_BOLD COLOR_CYAN );
    
    uart_print(COLOR_CYAN);
    uart_print(BORDER_MID);
    uart_print(COLOR_RESET);
    uart_print("\r\n");
    
    print_menu_item("a", "📈", " Increase frequency (+1s, min: 2s)", COLOR_GREEN);
    print_menu_item("b", "📉", " Decrease frequency (-1s, max: 10s)", COLOR_YELLOW);
    print_menu_item("c", "🔄", " Toggle display mode (T/H/Both)", COLOR_CYAN );
    print_menu_item("d", "📊", " Show detailed system status", COLOR_MAGENTA);
    
    uart_print(BORDER_SIDE);
    uart_print("                                                              ");
    uart_print(BORDER_SIDE);
    uart_print("\r\n");
    
    print_menu_item("Ctrl+C", "🧹", " Clear screen & refresh", COLOR_CYAN);
    print_menu_item("Ctrl+X", "❌", " Exit system safely", COLOR_RED);
    
    print_footer("💡 Tip: Use button for mode switching & AEM frequency updates");
    
    uart_print(COLOR_BOLD COLOR_GREEN "🎯 Enter command: " COLOR_RESET);
}

/**
 * @brief Display the user authentication prompt
 */
void authenticate_user_display(void)
{
    // Clear screen first
    uart_print("\033[2J\033[H");
    
    // Animated welcome sequence
    uart_print(COLOR_BOLD COLOR_BLUE);
    uart_print("████████████████████████████████████████████████████████████████\r\n");
    uart_print("█                                                              █\r\n");
    uart_print("█   ███████╗██╗   ██╗███████╗████████╗███████╗███╗   ███╗      █\r\n");
    uart_print("█   ██╔════╝╚██╗ ██╔╝██╔════╝╚══██╔══╝██╔════╝████╗ ████║      █\r\n");
    uart_print("█   ███████╗ ╚████╔╝ ███████╗   ██║   █████╗  ██╔████╔██║      █\r\n");
    uart_print("█   ╚════██║  ╚██╔╝  ╚════██║   ██║   ██╔══╝  ██║╚██╔╝██║      █\r\n");
    uart_print("█   ███████║   ██║   ███████║   ██║   ███████╗██║ ╚═╝ ██║      █\r\n");
    uart_print("█   ╚══════╝   ╚═╝   ╚══════╝   ╚═╝   ╚══════╝╚═╝     ╚═╝      █\r\n");
    uart_print("█                                                              █\r\n");
    uart_print("████████████████████████████████████████████████████████████████\r\n");
    uart_print(COLOR_RESET);
    
    print_animated_header("🔐 SECURE ACCESS PORTAL 🔐", "Environmental Monitoring System v2.0");
    
    print_centered_line("🔑  Authentication Required", COLOR_BOLD COLOR_YELLOW);
    print_centered_line("🔥 Temperature & Humidity Monitor", COLOR_CYAN);
    print_centered_line("⚡ Real-time Alert System", COLOR_GREEN);
    
    print_footer("🔒 Enter credentials to continue");
    
    uart_print(COLOR_BOLD COLOR_GREEN "🔑 Password: " COLOR_RESET);
    
    current_state = AUTH_STATE_PASSWORD;
    uart_buffer_index = 0;
    memset(uart_buffer, 0, sizeof(uart_buffer));
}


/* -------------------------------------------------------------------------- */
/*                    UART Output Formatting Functions                        */
/* -------------------------------------------------------------------------- */

/**
 * @brief Print a string to UART with color support
 */
void uart_printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    uart_print(buffer);
}

/**
 * @brief Print a message with a notification box and restore cursor position
 */
void uart_print_with_restore(const char* message)
{
    // Save cursor position and clear line
    uart_print("\033[s\r\033[K");
    
    // Create beautiful notification box
    uart_print(COLOR_BOLD COLOR_CYAN "┌─ 📢 NOTIFICATION " COLOR_RESET COLOR_CYAN);
    for(int i = 0; i < 44; i++) uart_print("─");
    uart_print("┐" COLOR_RESET "\r\n");
    
    uart_print(COLOR_CYAN "│ " COLOR_RESET COLOR_YELLOW);

    // Print the message but skip any newlines
    const char* ptr = message;
    while (*ptr) {
        if (*ptr != '\r' && *ptr != '\n') {
            char c[2] = {*ptr, 0};
            uart_print(c);
        }
        ptr++;
    }

    uart_print(COLOR_RESET);    
    
    // Calculate visual display width (not byte count) - similar to print_centered_line
    int display_width = 0;
    ptr = message;
    
    while (*ptr) {
        if (*ptr == '\r' || *ptr == '\n') {
            ptr++;
            continue;
        }
        
        // Handle emoji/special characters (typically UTF-8)
        if ((*ptr & 0xC0) == 0xC0) {
            // Skip all bytes of this UTF-8 character
            unsigned char first_byte = *ptr;
            int bytes_to_skip = 0;
            
            if ((first_byte & 0xE0) == 0xC0) bytes_to_skip = 2;      // 2-byte UTF-8
            else if ((first_byte & 0xF0) == 0xE0) bytes_to_skip = 3; // 3-byte UTF-8
            else if ((first_byte & 0xF8) == 0xF0) bytes_to_skip = 4; // 4-byte UTF-8
            
            display_width += 2; // Most emoji/symbols display as width 2 in many terminals
            while (bytes_to_skip > 0 && *ptr) {
                ptr++;
                bytes_to_skip--;
            }
        } else {
            // Regular ASCII character
            display_width++;
            ptr++;
        }
    }
    
    // Calculate padding for right border based on visual width
    int padding_needed = 60 - display_width;
    for(int i = 0; i < padding_needed; i++) {
        uart_print(" ");
    }
    uart_print(COLOR_CYAN " │" COLOR_RESET "\r\n");
    
    uart_print(COLOR_CYAN "└");
    for(int i = 0; i < 62; i++) uart_print("─");
    uart_print("┘" COLOR_RESET "\r\n");
    
    // Restore prompt and user input
    if (uart_buffer_index > 0) {
        uart_print(COLOR_BOLD COLOR_GREEN "\n🎯 Enter command: " COLOR_RESET);
        for (int i = 0; i < uart_buffer_index; i++) {
            uart_tx(uart_buffer[i]);
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                       Terminal Control Functions                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Clear the terminal screen and reset cursor position
 */
void clear_terminal(void)
{
    // Reset input state
    uart_buffer_index = 0;
    uart_buffer[0] = '\0'; /* Null-terminate for safety */

    // Clear the screen and reset cursor position
    uart_print("\033[2J\033[H");    
}

/**
 * @brief Terminate the program and perform cleanup
 */
void terminate_program(void)
{
    uart_print("\r\n\n");
    
    // Clear screen first
    uart_print("\033[2J\033[H");
    
    // Animated farewell sequence with ASCII art
    uart_print(COLOR_BOLD COLOR_RED);
    uart_print("████████████████████████████████████████████████████████████████\r\n");
    uart_print("█                                                              █\r\n");
    uart_print("█                    ███████╗██╗  ██╗██╗████████╗              █\r\n");
    uart_print("█                    ██╔════╝╚██╗██╔╝██║╚══██╔══╝              █\r\n");
    uart_print("█                    █████╗   ╚███╔╝ ██║   ██║                 █\r\n");
    uart_print("█                    ██╔══╝   ██╔██╗ ██║   ██║                 █\r\n");
    uart_print("█                    ███████╗██╔╝ ██╗██║   ██║                 █\r\n");
    uart_print("█                    ╚══════╝╚═╝  ╚═╝╚═╝   ╚═╝                 █\r\n");
    uart_print("█                                                              █\r\n");
    uart_print("████████████████████████████████████████████████████████████████\r\n");
    uart_print(COLOR_RESET);
        
    print_animated_header("👋 EXIT", "System Termination Sequence");
    
    print_centered_line("✅ Program terminated by user request", COLOR_GREEN);

    __disable_irq();
    gpio_set(LED_PIN, 0);
    timer_disable();

    print_centered_line("🔧 Hardware cleanup completed", COLOR_CYAN);
    
    print_footer("Thank you for using Environmental Monitor v2.0! 🌟");
        
    exit(0);
}


/* ========================================================================== */
/*                              ENHACED DISPLAY FUNCTIONS                     */
/* ========================================================================== */

/**
 * @brief Print a centered line of text with color support
 */
void print_centered_line(const char* text, const char* color)
{
    int display_width = 0;
    const char* ptr = text;
    
    // Calculate visual display width (not byte count)
    while (*ptr) {
        // Handle emoji/special characters (typically UTF-8)
        if ((*ptr & 0xC0) == 0xC0) {
            // Skip all bytes of this UTF-8 character
            unsigned char first_byte = *ptr;
            int bytes_to_skip = 0;
            
            if ((first_byte & 0xE0) == 0xC0) bytes_to_skip = 2;      // 2-byte UTF-8
            else if ((first_byte & 0xF0) == 0xE0) bytes_to_skip = 3; // 3-byte UTF-8
            else if ((first_byte & 0xF8) == 0xF0) bytes_to_skip = 4; // 4-byte UTF-8
            
            display_width += 2; // Most emoji/symbols display as width 2 in many terminals
            while (bytes_to_skip > 0 && *ptr) {
                ptr++;
                bytes_to_skip--;
            }
        } else {
            // Regular ASCII character
            display_width++;
            ptr++;
        }
    }
    
    // Adjust total width to account for the borders
    int total_width = 62; // Width between borders
    int padding = (total_width - display_width) / 2;
    
    uart_print(BORDER_SIDE);
    uart_print(color);
    
    // Left padding
    for(int i = 0; i < padding; i++) {
        uart_print(" ");
    }
    
    uart_print(text);
    
    // Right padding - ensure we fill exactly to the right border
    int right_padding = total_width - display_width - padding;
    for(int i = 0; i < right_padding; i++) {
        uart_print(" ");
    }
    
    uart_print(COLOR_RESET);
    uart_print(BORDER_SIDE);
    uart_print("\r\n");
}

/**
 * @brief Print a formatted line with icon and description
 */
void print_menu_item(const char* key, const char* icon, const char* description, const char* color)
{
    // Print the border and start of the line
    uart_print(BORDER_SIDE);
    uart_print(PADDING);
    
    // Print the colored key part
    uart_print("[");
    uart_print(color);
    uart_print(key);
    uart_print(COLOR_RESET);
    uart_print("] - ");
    
    // Print the icon and description
    uart_print(icon);
    uart_print(description);
    
    // Calculate the visual length (not counting escape sequences)
    int visual_length = strlen(PADDING) + 4; // "[" + "] - " = 4 chars
    visual_length += strlen(key);           // Length of the key
    
    // Calculate emoji width - most emoji are double-width characters
    int icon_visual_length = 0;
    const char* ptr = icon;
    while (*ptr) {
        if ((*ptr & 0xC0) == 0xC0) { // UTF-8 multi-byte sequence
            icon_visual_length += 2;  // Most emoji display as width 2
            unsigned char first_byte = *ptr;
            int bytes_to_skip = 0;
            
            if ((first_byte & 0xE0) == 0xC0) bytes_to_skip = 2;      // 2-byte UTF-8
            else if ((first_byte & 0xF0) == 0xE0) bytes_to_skip = 3; // 3-byte UTF-8
            else if ((first_byte & 0xF8) == 0xF0) bytes_to_skip = 4; // 4-byte UTF-8
            
            while (bytes_to_skip > 0 && *ptr) {
                ptr++;
                bytes_to_skip--;
            }
        } else {
            icon_visual_length++;
            ptr++;
        }
    }
    
    visual_length += icon_visual_length;
    visual_length += strlen(description);
    
    // Calculate remaining spaces to align with right border
    int total_width = 61; // Width between borders
    int remaining = total_width - visual_length;
    
    // Add padding spaces to reach the right border
    for (int i = 0; i < remaining; i++) {
        uart_print(" ");
    }
    
    // Print the right border and newline
    uart_print(BORDER_SIDE);
    uart_print("\r\n");
}

/**
 * @brief Print animated header with gradient effect
 */
void print_animated_header(const char* title, const char* subtitle)
{
    uart_print("\r\n");
    uart_print(COLOR_CYAN);
    uart_print(BORDER_TOP);
    uart_print(COLOR_RESET);
    uart_print("\r\n");
    
    print_centered_line(title, COLOR_BOLD COLOR_GREEN);
    
    if(subtitle) {
        print_centered_line(subtitle, COLOR_DIM COLOR_CYAN);
    }
    
    uart_print(COLOR_CYAN);
    uart_print(BORDER_MID);
    uart_print(COLOR_RESET);
    uart_print("\r\n");
}

/**
 * @brief Print footer with timestamp or additional info
 */
void print_footer(const char* info)
{
    if(info) {
        print_centered_line(info, COLOR_DIM);
        uart_print(COLOR_CYAN);
        uart_print(BORDER_MID);
        uart_print(COLOR_RESET);
        uart_print("\r\n");
    }
    
    uart_print(COLOR_CYAN);
    uart_print(BORDER_BOTTOM);
    uart_print(COLOR_RESET);
    uart_print("\r\n");
}

