/*
 * ===========================================================================
 * File:        system_control.h
 * Description: System control and management function declarations
 * Date:        May, 2025
 * ===========================================================================
 */

#ifndef SYSTEM_CONTROL_H
#define SYSTEM_CONTROL_H

#include <stdint.h>
#include "system_config.h"
#include "queue.h"


/* ========================================================================== */
/*                           EXTERNAL VARIABLES                               */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                        Communication and Timing Variables                  */
/* -------------------------------------------------------------------------- */
extern Queue rx_queue;
extern char student_aem[];
extern uint32_t data_update_period;

/* -------------------------------------------------------------------------- */
/*                        System State and Alert Variables                    */
/* -------------------------------------------------------------------------- */
extern system_mode_t current_mode;
extern uint8_t alert_active;
extern volatile uint32_t led_timer_count;
extern volatile uint8_t update_led_flag;


/* ========================================================================== */
/*                        SYSTEM CONTROL FUNCTIONS                           */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                    System Initialization and Management                    */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize all system components and peripherals
 * 
 * Performs comprehensive system initialization including hardware configuration,
 * peripheral setup, and initial state establishment for the environmental
 * monitoring system.
 */
void system_init(void);

/**
 * @brief Update sensor data acquisition frequency based on AEM input
 * 
 * Calculates new data update frequency using the sum of the last two digits
 * from the authenticated student AEM. The frequency is automatically clamped
 * between MIN_PERIOD and MAX_PERIOD bounds. For single-digit AEMs, the
 * frequency update is skipped with appropriate console notification.
 * 
 * @note Outputs the calculated frequency to UART for user confirmation
 */
void update_data_frequency_from_aem(void);

/**
 * @brief Perform software system reset
 * 
 * Executes a controlled software reset using NVIC_SystemReset() after
 * sending a notification message via UART. Includes a 100ms delay to
 * ensure message transmission completion before reset execution.
 */
void software_reset(void);

/* -------------------------------------------------------------------------- */
/*                         Alert Management System                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Disable alert mode and return to normal operation
 * 
 * Transitions the system from alert mode back to normal operational mode,
 * disabling visual alert indicators and resetting alert state counters.
 */
void disable_alert_mode(void);

/**
 * @brief Evaluate environmental conditions for alert threshold violations
 * 
 * @param temp Current temperature reading in degrees Celsius
 * @param humidity Current humidity reading as percentage
 * 
 * Analyzes current environmental sensor readings against configured alert
 * thresholds and manages transitions between normal and alert operational modes.
 */
void check_alert_conditions(float temp, float humidity);

/**
 * @brief Evaluate environmental conditions for panic threshold violations
 * 
 * @param temp Current temperature reading in degrees Celsius
 * @param humidity Current humidity reading as percentage
 * 
 * Monitors environmental readings for critical panic threshold violations
 * requiring immediate system response and potential system reset.
 */
void check_panic_conditions(float temp, float humidity);

#endif // SYSTEM_CONTROL_H