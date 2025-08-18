/*
 * ===========================================================================
 * File:        user_auth.c
 * Description: User authentication system function implementations
 * Date:        May, 2025
 * ===========================================================================
 */

/* ========================================================================== */
/*                              INCLUDES                                      */
/* ========================================================================== */
#include "user_auth.h"
#include "system_config.h"
#include "uart.h"
#include "timer.h"
#include "console.h"
#include <string.h>
#include <stdio.h>


/* ========================================================================== */
/*                    AUTHENTICATION INPUT PROCESSING                         */
/* ========================================================================== */

/**
 * @brief Process individual character input during authentication
 */
void handle_auth_input(uint8_t c)
{
    // Handle Enter key press - process complete input
    if (c == '\r' || c == '\n') {
        auth_process_enter();
        return;
    }
    
    // Handle Backspace key press - remove last character
    if (c == 127 || c == 8) { // ASCII DEL or BS
        if (uart_buffer_index > 0) {
            uart_buffer_index--;           // Move buffer pointer back
            uart_print("\b \b");           // Clear character on terminal
        }
        return;
    }
    
    // Process regular character input based on current authentication state
    if (uart_buffer_index < AUTH_LENGTH) {
        switch (current_state) {
            case AUTH_STATE_PASSWORD:
                // Password input - store character and echo asterisk for security
                uart_buffer[uart_buffer_index++] = c;
                uart_tx('*'); // Security masking with asterisk
                break;
                
            case AUTH_STATE_AEM:
                // AEM input - only accept numeric characters (0-9)
                if (c >= '0' && c <= '9') {
                    uart_buffer[uart_buffer_index++] = c;
                    uart_tx(c); // Echo actual numeric character
                }
                // Silently ignore non-numeric characters for AEM input
                break;
                
            case AUTH_STATE_AUTHENTICATED:
                // Should not reach here - handled in handle_uart_data()
                break;
        }
    }
}

/**
 * @brief Process Enter key press during authentication
 */
void auth_process_enter(void)
{
    // Skip processing if buffer is empty
    if (uart_buffer_index == 0) return;
    
    // Null-terminate the input string for processing
    uart_buffer[uart_buffer_index] = '\0';

    // Store current state for buffer management decisions
    uint8_t was_password_state = (current_state == AUTH_STATE_PASSWORD);
    
    // Route processing based on current authentication state
    switch (current_state) {
        case AUTH_STATE_PASSWORD:
            auth_handle_password();
            break;
            
        case AUTH_STATE_AEM:
            auth_handle_AEM();
            break;
            
        case AUTH_STATE_AUTHENTICATED:
            // Should not reach here - already authenticated
            break;
    }
    
    // Reset buffer index for next input cycle
    uart_buffer_index = 0;

    // Clear sensitive password data from buffer for security
    if (was_password_state) {
        memset(uart_buffer, 0, sizeof(uart_buffer));
    }
}


/* ========================================================================== */
/*                    AUTHENTICATION STATE HANDLERS                           */
/* ========================================================================== */

/**
 * @brief Handle password validation and authentication
 */
void auth_handle_password(void)
{
    uart_print("\r\n");
    
    if (strcmp(uart_buffer, PASSWORD) == 0) {
        // Use enhanced formatting from console.c
        print_animated_header("✅ ACCESS GRANTED ✅", "Password Authentication Successful!");
        print_centered_line("🎉 Welcome to the system! 🎉", COLOR_BOLD COLOR_GREEN);
        print_footer("Proceeding to Student ID verification...");
        
        uart_print(COLOR_BOLD COLOR_CYAN "👤 Please enter your Student ID (AEM): " COLOR_RESET);
        current_state = AUTH_STATE_AEM;
    } else {
        // Use enhanced formatting for error message
        print_animated_header("❌ ACCESS DENIED ❌", "Authentication Failed");
        print_centered_line("🚫 Invalid Password - Try Again 🚫", COLOR_BOLD COLOR_RED);
        print_footer("💡 Hint: Check your password carefully");
        
        uart_print(COLOR_BOLD COLOR_YELLOW "🔑 Password: " COLOR_RESET);
    }
}

/**
 * @brief Handle AEM (Student ID) input and storage
 */
void auth_handle_AEM(void)
{
    uart_print("\r\n");
    
    strcpy(student_aem, uart_buffer);
    
    print_animated_header("🎓 STUDENT VERIFIED 🎓", "System Initialization Complete");
    
    // Use uart_printf for formatted output
    char aem_info[64];
    sprintf(aem_info, "👤 AEM: %s", student_aem);
    print_centered_line(aem_info, COLOR_BOLD COLOR_CYAN);
    print_centered_line("🚀 System Initialized", COLOR_GREEN);
    print_centered_line("✅ All Systems Online", COLOR_CYAN );
    
    print_footer("🌟 Ready for environmental monitoring");
    
    current_state = AUTH_STATE_AUTHENTICATED;
    display_menu();
    timer_enable();
}

/* ========================================================================== */
/*                    AUTHENTICATION STATUS FUNCTIONS                         */
/* ========================================================================== */

/**
 * @brief Check current authentication status
 */
uint8_t auth_is_authenticated(void)
{
    return (current_state == AUTH_STATE_AUTHENTICATED);
}