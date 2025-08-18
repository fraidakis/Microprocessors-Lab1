# Lab 3: Autonomous Environmental Control System with DHT11 Sensor
**Advanced Sensor Integration & Autonomous System Design**  
**Author:** Fraidakis Ioannis  
**Date:** June 2025  

## Overview
This laboratory exercise implements an autonomous environmental control system using the ARM NUCLEO M4 microcontroller (STM32F411RE). The system demonstrates advanced microcontroller programming concepts including sensor interfacing, dual-mode operation and multi-level alert management.

The system features a DHT11 temperature-humidity sensor interface, touch sensor-driven mode switching, comprehensive UART command interface, and sophisticated alert management with panic reset functionality. The implementation showcases interrupt-driven architecture, real-time environmental monitoring, and user-interactive control systems.

---

## Project Structure

```
Lab3/
├── README.MD             # This file - Lab 3 documentation
├── docs/                 # Documentation folder
│   ├── assignment-3.pdf  # Original assignment requirements
│   ├── report.pdf        # Detailed technical report
│   └── report.tex        # LaTeX source for report
└── project/              # Keil µVision project and source code
    ├── drivers/          # Hardware driver libraries
    ├── src/              # Source code implementations
    └── Lab3.uvprojx      # Keil µVision project file
```

---

## System Overview

### Hardware Components
The system integrates the following hardware components:

- **DHT11 Sensor**: Connected to GPIO pin `PC_0`, providing digital temperature and humidity measurements using 1-wire communication protocol
- **Status LED**: Connected to GPIO pin `PB_0`, used for visual alert indication with programmable blinking patterns  
- **Touch Sensor**: Connected to GPIO pin `PC_1`, configured with rising trigger interrupt for mode switching
- **UART Interface**: Provides bidirectional communication for user authentication, command processing, and data output at 115200 baud rate

### Software Architecture

#### Key Data Structures
- **System State Management**: Global `volatile` variables manage operational modes, authentication states, and configuration parameters. The `volatile` keyword ensures correct ISR-main loop data sharing
- **UART Receive Queue**: A `Queue` structure buffers incoming UART characters, decoupling interrupt reception from main loop processing
- **Sensor Data Storage**: Floating-point variables store the most recent temperature and humidity readings
- **Authentication Context**: Secure storage for user credentials and AEM (student ID) with proper buffer management and security considerations

#### Operational Modes
The system operates using a dual-mode architecture with authentication gate:

- **Authentication Phase**: Password verification followed by AEM capture with secure input handling and visual feedback
- **Mode A (Normal)**: Standard environmental monitoring with configurable frequency and display options
- **Mode B (Alert)**: Enhanced monitoring with LED alerts, threshold detection, and automatic recovery mechanisms

#### Interrupt Service Routines
Three main interrupt handlers orchestrate system operations:

- **timer_callback()**: Manages periodic sensor readings, LED blinking timing, and system timing base with 1-second resolution
- **button_press_callback()**: Handles touch sensor events with debouncing, mode switching logic, and frequency update calculations
- **uart_receive_callback()**: Processes incoming UART characters with queue buffering and ASCII validation

---

## Assignment Description

### System Overview
The environmental control system implements two operational variants:
1. **Button-based Control**: Using the onboard user button (PC13) for system interaction
2. **Touch Sensor Control**: Using an external touch sensor for enhanced user interaction

Both variants share core functionality with different input methods for profile switching and system control.

### Core System Features

#### 1. **Secure System Startup**
- **Password Protection**: System requires correct password ("2025") for access
- **User Authentication**: Student ID (AEM) collection and storage for personalized operation
- **Error Handling**: Incorrect password handling with re-prompt functionality
- **Session Management**: Secure login process with user identification

#### 2. **Enhanced UART Command Interface**
The system provides comprehensive command processing:

| Command | Function | Range/Constraints |
|---------|----------|-------------------|
| 'a' | Increase frequency (decrease period) | Minimum: 2 seconds |
| 'b' | Decrease frequency (increase period) | Maximum: 10 seconds |
| 'c' | Toggle display mode | Temperature/Humidity/Both |
| 'd' | Show detailed system status | All parameters |
| 'status' | Compact system information | Quick overview |
| Ctrl+C | Clear screen and refresh | Terminal reset |
| Ctrl+X | Safe system termination | Graceful shutdown |

**Enhanced Console Features:**
- **ANSI Color Coding**: Different colors for various message types
- **Animated Headers**: Professional-looking bordered headers for messages
- **Progress Indicators**: Visual feedback during authentication and processing
- **Input Restoration**: Smart terminal handling that preserves user input during system messages

#### 3. **Dual Operation Modes**

**Mode A - Normal Operation:**
- Continuous environmental monitoring
- Regular sensor data collection and UART output
- LED indicator off
- Standard operation parameters

**Mode B - Alert Mode:**
- **Alert Conditions**: Temperature > 25°C OR Humidity > 60%
- **Visual Indication**: LED blinks every 1 second during alert conditions
- **Recovery Logic**: LED turns off after 5 consecutive normal readings
- **Enhanced Monitoring**: Increased attention to environmental parameters

#### 4. **Dynamic Profile Switching**
- **Input Method**: Button press or touch sensor activation
- **State Transition**: Toggle between Normal Mode (A) and Alert Mode (B)
- **Frequency Adaptation**: Every 3rd activation calculates new reading frequency
- **AEM-based Calculation**: Frequency = sum of last two AEM digits (respecting 2-10s limits)

#### 5. **Emergency Response System**
- **Panic Conditions**: Temperature > 35°C OR Humidity > 80% for 3 consecutive readings
- **System Response**: Automatic "PANIC RESET" message and system state reset
- **Safety Priority**: Override normal operation for critical environmental conditions
- **Recovery Protocol**: Automatic return to normal operation when conditions stabilize

#### 6. **Comprehensive Status Reporting**
UART command "status" provides:
- Current operational mode (Mode A/B)
- Latest temperature and humidity readings
- Total profile switch count
- System configuration parameters
- Alert status and history

---

## Hardware Components

### Required Components
- **STM32F411RE Nucleo Board**: Main microcontroller platform
- **DHT11 Sensor**: Temperature and humidity measurement
- **LED Indicator**: Visual status indication (onboard LD2/PA5)
- **User Button**: System control input (onboard B1/PC13)
- **Touch Sensor** (variant dependent): Alternative input method
- **UART Interface**: User communication and system monitoring

### Sensor Specifications
- **DHT11 Temperature Range**: 0-50°C (±2°C accuracy)
- **DHT11 Humidity Range**: 20-90% RH (±5% accuracy)
- **Reading Frequency**: Configurable 2-10 second intervals
- **Interface Protocol**: Single-wire digital communication

---

## Technical Implementation

### Implementation Details

#### DHT11 Sensor Communication
The DHT11 sensor requires microsecond-precise timing for proper communication. The implementation includes:
- **Critical Sections**: Interrupts are temporarily disabled during timing-sensitive operations using `__disable_irq()` and `__enable_irq()`
- **Precise Timing**: Dedicated timing functions ensure protocol compliance
- **Checksum Validation**: Data integrity verification for reliable sensor readings

#### UART Command Processing
Advanced command processing features include:
- **Queue-based Reception**: Interrupt-driven character buffering
- **Command Parsing**: Robust string processing with error handling
- **Real-time Response**: Immediate feedback for all user interactions

### Software Architecture
- **Modular Design**: Separate drivers for each hardware component
- **Interrupt-driven Operation**: Responsive user interface and sensor management
- **State Machine Implementation**: Clean mode transitions and system state management
- **Error Handling**: Comprehensive input validation and system recovery

### Key Algorithms
1. **Environmental Threshold Monitoring**: Continuous comparison with alert thresholds
2. **Consecutive Reading Analysis**: Multi-sample decision making for stability
3. **Dynamic Frequency Calculation**: AEM-based adaptive timing with bounds checking
4. **Emergency Detection**: Multi-parameter critical condition assessment

### Memory Management
- **Sensor Data Buffering**: Historical readings for consecutive analysis
- **Configuration Storage**: User preferences and system parameters
- **Status Tracking**: Mode transitions and alert history
- **Dynamic Allocation**: Efficient memory usage for sensor arrays

---

### Hardware Connections
```
DHT11 Sensor:
- VCC -> 3.3V or 5V
- GND -> GND
- Data -> PC_0 (Digital Pin)

Touch Sensor:
- VCC -> 3.3V
- GND -> GND
- Signal -> PC_1 (Digital Pin with interrupt capability)

Status LED:
- Connected to PB_0 (GPIO Pin)
- Programmable blinking patterns for alerts

UART Interface:
- TX/RX pins for bidirectional communication
- Baud Rate: 115200
- Used for authentication, commands, and data output
```

### Testing and Validation

#### Test Scenarios

| Test Category | Test Case | Expected Outcome |
|---------------|-----------|------------------|
| Authentication | Correct password "2025" | Proceed to AEM input stage |
| Authentication | Incorrect password | Error message, re-prompt |
| Authentication | AEM input "10736" | Successful login, menu display |
| Sensor Reading | Normal conditions | Temperature/humidity display |
| Mode Switching | Touch sensor press | Toggle between Mode A/B |
| Frequency Control | Command 'a' | Increase frequency (min 2s) |
| Frequency Control | Command 'b' | Decrease frequency (max 10s) |
| AEM Frequency | 3rd touch press | Update from AEM last 2 digits |
| Display Toggle | Command 'c' | Cycle through T/H/Both modes |
| Alert System | Temp > 25°C in Mode B | LED blinking activation |
| Alert Recovery | 5 normal readings | LED stops, alert cleared |
| Panic Reset | Temp > 35°C (3 times) | System reset triggered |
| Status Display | Command 'd' or 'status' | Detailed system information |
| Terminal Control | Ctrl+C | Screen clear, menu refresh |
| Safe Shutdown | Ctrl+X | Graceful program termination |

#### Validation Methodology
- **Sensor Accuracy**: Verify DHT11 readings against reference measurements and validate checksum integrity
- **Mode Transitions**: Test touch sensor responsiveness and proper state transitions between modes
- **Alert Thresholds**: Validate alert activation/deactivation with controlled environmental conditions
- **Command Processing**: Verify all UART commands function correctly with proper error handling

#### Implementation Challenges and Solutions

**Challenge 1: DHT11 Timing Critical Sections**
The DHT11 sensor requires microsecond-precise timing for proper communication. Any interrupt during the critical reading phase could disrupt the timing and cause data corruption or communication failure.

**Solution**: Implemented critical sections during DHT11 communication by temporarily disabling interrupts using `__disable_irq()` and `__enable_irq()` around timing-sensitive operations. Additionally, used dedicated timing functions with precise delay mechanisms to ensure protocol compliance. The sensor reading is scheduled during low-activity periods to minimize interrupt conflicts.

### Setup and Execution

**Testing Procedure**:
   - Connect to UART terminal (115200 baud)
   - Power on system and observe startup sequence
   - Enter correct password ("2025") and Student ID
   - Test menu commands (a, b, c, d, status)
   - Test terminal controls (Ctrl+C, Ctrl+X)
   - Verify mode switching with touch sensor
   - Test alert conditions with environmental changes
   - Validate emergency response with extreme conditions

---

## Expected System Behavior

### Startup Sequence
```
System Starting...
Enter Password: ****
Password Correct!
Enter Student ID (AEM): 10736
Login Successful!

=== Environmental System ===
[Menu displays]
Current Mode: Normal (A)
Reading Frequency: 6 seconds
```

### Normal Operation Example
```
Temperature: 23.5°C, Humidity: 45%
Temperature: 24.1°C, Humidity: 47%
[Touch sensor pressed - Mode switch]
Switched to Alert Mode (B)
Temperature: 26.2°C, Humidity: 52%
ALERT: High temperature detected! LED blinking...
```

### Emergency Response Example
```
Temperature: 36.8°C, Humidity: 85%
WARNING: Critical conditions detected (1/3)
Temperature: 37.2°C, Humidity: 87%
WARNING: Critical conditions detected (2/3)
Temperature: 38.1°C, Humidity: 89%
PANIC RESET: System entering emergency mode!
```

---

## Learning Objectives

Upon completion of this lab, students will have:

1. **Advanced Sensor Integration**: Experience with digital sensor protocols and data processing
2. **Complex System Design**: Skills in multi-component system architecture and coordination
3. **Adaptive Control Systems**: Implementation of intelligent decision-making algorithms
4. **User Interface Development**: Creation of interactive menu systems and user feedback
5. **Safety System Design**: Development of emergency response and fail-safe mechanisms
6. **Real-world Application**: Understanding of practical environmental monitoring systems

---

## Future Enhancements

Potential improvements for advanced exploration:
- **Wireless Communication**: WiFi or Bluetooth connectivity for remote monitoring
- **Data Logging**: SD card storage for historical environmental data

---

## Conclusion

This laboratory exercise provides comprehensive experience with advanced embedded systems development, demonstrating the integration of multiple sensors, complex control algorithms, and user-friendly interfaces to create a practical environmental monitoring solution. The skills developed here are directly applicable to industrial IoT systems, smart building automation, and environmental compliance monitoring.

---

**Course:** Microprocessors and Peripherals Laboratory  
**Institution:** Aristotle University of Thessaloniki  
**Semester:** Spring 2025
