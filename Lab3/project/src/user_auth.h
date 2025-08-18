/*
 * ===========================================================================
 * File:        user_auth.h
 * Description: User authentication system function declarations
 * Date:        May, 2025
 * ===========================================================================
 */

#ifndef UART_AUTH_H
#define UART_AUTH_H

#include <stdint.h>
#include "system_config.h"


/* ========================================================================== */
/*                           EXTERNAL VARIABLES                               */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                    Communication and Buffer Variables                      */
/* -------------------------------------------------------------------------- */
extern char uart_buffer[UART_BUFFER_SIZE];
extern uint8_t uart_buffer_index;          

/* -------------------------------------------------------------------------- */
/*                    Authentication State Variables                          */
/* -------------------------------------------------------------------------- */
extern auth_state_t current_state;
extern char student_aem[5];


/* ========================================================================== */
/*                        USER AUTHENTICATION FUNCTIONS                       */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                    Authentication Input Processing                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Display initial authentication prompt to user
 * 
 * Presents the initial password entry prompt via UART and sets the system
 * to the appropriate authentication state for password input processing.
 */
void authenticate_user_display(void);

/**
 * @brief Process individual character input during authentication
 * 
 * @param c Input character received from UART
 * 
 * Handles character-by-character input processing during authentication phases,
 * including password masking, AEM numeric validation, backspace handling,
 * and appropriate character echoing based on current authentication state.
 */
void handle_auth_input(uint8_t c);

/**
 * @brief Process Enter key press during authentication
 * 
 * Handles complete input processing when Enter key is pressed, including
 * password validation, AEM storage, state transitions, and buffer management
 * for the authentication workflow.
 */
void auth_process_enter(void);

/* -------------------------------------------------------------------------- */
/*                    Authentication State Handlers                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Handle password validation and authentication
 * 
 * Validates entered password against stored credentials and manages state
 * transition to AEM input phase upon successful authentication or prompts
 * for retry upon failure.
 */
void auth_handle_password(void);

/**
 * @brief Handle AEM (Student ID) input and storage
 * 
 * Processes and stores the authenticated user's AEM (student ID) input,
 * completes the authentication process, and transitions the system to 
 * authenticated operational mode with menu display and timer activation.
 */
void auth_handle_AEM(void);

/* -------------------------------------------------------------------------- */
/*                    Authentication Status Functions                         */
/* -------------------------------------------------------------------------- */

/**
 * @brief Check current authentication status
 * 
 * @return uint8_t Authentication status (1 if authenticated, 0 if not)
 * 
 * Returns the current authentication state of the system to determine
 * whether user has completed the authentication process successfully.
 */
uint8_t auth_is_authenticated(void);

#endif // UART_AUTH_H
