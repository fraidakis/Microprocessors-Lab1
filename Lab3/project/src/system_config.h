/*
 * ===========================================================================
 * File:        system_config.h
 * Description: System-wide configuration parameters, constants, and type definitions
 * Date:        May, 2025
 *
 * This header file defines all configuration parameters, hardware pin assignments,
 * operational thresholds, and data type definitions used throughout the embedded
 * environmental monitoring system. It serves as the central configuration point
 * for system customization and hardware abstraction.
 * ===========================================================================
 */

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include <stdint.h>


/* ========================================================================== */
/*                         SYSTEM STATE DEFINITIONS                           */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                             Boolean Constants                              */
/* -------------------------------------------------------------------------- */
#define ON 1  // Boolean constant for active/enabled state
#define OFF 0 // Boolean constant for inactive/disabled state

/* -------------------------------------------------------------------------- */
/*                          Hardware Pin Assignments                         */
/* -------------------------------------------------------------------------- */
#define DHT11_PIN PC_0   // DHT11 sensor data pin (temperature/humidity) - BOARD A5
#define BUTTON_PIN PC_13 // User button pin
#define LED_PIN PB_0     // Status LED pin (visual feedback) - BOARD A3 -> change to PA_5 for the built-in LED
#define TOUCH_SENSOR_PIN PC_1 // Touch sensor pin (for touch input) - BOARD A4

/* -------------------------------------------------------------------------- */
/*                            Timer Configuration                             */
/* -------------------------------------------------------------------------- */
#define TIMER_INTERVAL 1000000 // Primary timer interval in microseconds (1 second)
#define LED_BLINK_INTERVAL 1   // LED blink period: 1 second (for alert mode)
#define BUTTON_DEBOUNCE_TIME 1 // Button debounce timeout: 1 second

/* -------------------------------------------------------------------------- */
/*                    Sensor Data Acquisition Configuration                   */
/* -------------------------------------------------------------------------- */
#define MIN_PERIOD 2     // Minimum sensor reading interval: 2 seconds
#define MAX_PERIOD 10    // Maximum sensor reading interval: 10 seconds
#define DEFAULT_PERIOD 6 // Default sensor reading interval: 6 seconds

/* -------------------------------------------------------------------------- */
/*                        Authentication Configuration                         */
/* -------------------------------------------------------------------------- */
#define PASSWORD "2025" // System access password for UART authentication

/* -------------------------------------------------------------------------- */
/*                        Environmental Alert Thresholds                      */
/* -------------------------------------------------------------------------- */
#define TEMP_ALERT_THRESHOLD 25     // Temperature alert threshold: 30°C
#define HUMIDITY_ALERT_THRESHOLD 60 // Humidity alert threshold: 60%
#define TEMP_PANIC_THRESHOLD 35     // Temperature panic threshold: 35°C
#define HUMIDITY_PANIC_THRESHOLD 80 // Humidity panic threshold: 90%

/* -------------------------------------------------------------------------- */
/*                        Alert State Control Parameters                      */
/* -------------------------------------------------------------------------- */
#define PANIC_CONSECUTIVE_COUNT 3  // Consecutive readings required to trigger panic mode
#define NORMAL_CONSECUTIVE_COUNT 5 // Consecutive normal readings required to exit alert mode

/* -------------------------------------------------------------------------- */
/*                        UART Communication Settings                         */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
#define QUEUE_SIZE 64         // UART receive buffer size (bytes)
#define UART_BAUD_RATE 115200 // UART communication rate: 115200 bps
#define UART_BUFFER_SIZE 64   // UART command input buffer size (bytes)
#define AUTH_LENGTH 10        // Length of password and AEM (student ID) input

/* -------------------------------------------------------------------------- */
/*                       Control Character Definitions                        */
/* -------------------------------------------------------------------------- */
#define KEY_CTRL_C_CLEAR 0x03     // Ctrl+C: Clear screen and reset input buffer
#define KEY_CTRL_X_TERMINATE 0x18 // Ctrl+X: Terminate program execution


/* -------------------------------------------------------------------------- */
/*                          User Interface Formatting                         */
/* -------------------------------------------------------------------------- */
#define BORDER_TOP    "╔══════════════════════════════════════════════════════════════╗" // Top border for UI frames
#define BORDER_BOTTOM "╚══════════════════════════════════════════════════════════════╝" // Bottom border for UI frames
#define BORDER_MID    "╠══════════════════════════════════════════════════════════════╣" // Middle separator for UI frames
#define BORDER_SIDE   "║"   // Side border character for UI frames
#define PADDING       "  "  // Standard padding for UI content alignment

/* -------------------------------------------------------------------------- */
/*                         ANSI Color Code Definitions                        */
/* -------------------------------------------------------------------------- */
#define COLOR_RESET   "\033[0m"  // Reset all formatting to default
#define COLOR_GREEN   "\033[32m" // Green text color
#define COLOR_BLUE    "\033[34m" // Blue text color
#define COLOR_YELLOW  "\033[33m" // Yellow text color
#define COLOR_RED     "\033[31m" // Red text color
#define COLOR_CYAN    "\033[36m" // Cyan text color
#define COLOR_MAGENTA "\033[35m" // Magenta text color
#define COLOR_BOLD    "\033[1m"  // Bold text formatting
#define COLOR_DIM     "\033[2m"  // Dimmed text formatting


/* ========================================================================== */
/*                              TYPE DEFINITIONS                              */
/* ========================================================================== */

/**
 * @brief System operational mode enumeration
 */
typedef enum
{
    MODE_A_NORMAL = 0, // Normal operation mode - standard monitoring
    MODE_B_ALERT = 1   // Alert operation - enhanced monitoring with LED feedback
} system_mode_t;

/**
 * @brief Sensor data display modes
 */
typedef enum
{
    DISPLAY_TEMP = 0,     // Display temperature data only
    DISPLAY_HUMIDITY = 1, // Display humidity data only
    DISPLAY_BOTH = 2      // Display both temperature and humidity data
} display_mode_t;

/**
 * @brief User authentication state enumeration
 */
typedef enum
{
    AUTH_STATE_PASSWORD = 0,     // Awaiting password entry
    AUTH_STATE_AEM = 1,          // Awaiting AEM (student ID) entry
    AUTH_STATE_AUTHENTICATED = 2 // Authentication completed successfully
} auth_state_t;

#endif // SYSTEM_CONFIG_H