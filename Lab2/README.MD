# Lab 2: Timer Interrupts & LED Control
**Real-Time Programming with Interrupts**  
**Author:** Fraidakis Ioannis  
**Date:** May 2025  

## Overview
This laboratory exercise focuses on developing an interrupt-driven LED control system that demonstrates fundamental microcontroller programming concepts. The project implements a user interface combining UART communication with physical button input to control an LED in various patterns.

The system processes numerical input where even digits trigger blinking behavior and odd digits toggle the LED state. A key feature is the ability to lock/unlock the LED via button press, preventing state changes during digit processing. The system demonstrates interrupt prioritization, state management, and hardware peripheral coordination using a robust queue-based architecture.

---

## Project Structure

```
Lab2/
├── README.MD             # This file - Lab 2 documentation
├── docs/                 # Documentation folder
│   ├── assignment-2.pdf  # Original assignment requirements
│   ├── report.pdf        # Detailed technical report
│   └── report.tex        # LaTeX source for report
└── project/              # Keil µVision project and source code
    ├── drivers/          # Hardware driver libraries
    ├── src/              # Source code implementations
    └── Lab2.uvprojx      # Keil µVision project file
```

---

## Hardware Components

The system interacts with the following hardware components:

- **LED**: Connected to GPIO pin `PA_5`, used for visual feedback
- **Button**: Connected to GPIO pin `PC_13`, used for user interaction (locking/unlocking the LED). It is configured with an internal pull-up resistor and triggers an interrupt on a falling edge (button press)

---

## Software Architecture

### Key Data Structures

- **System State (`sys_state`)**: A global `volatile struct` named `sys_state` centralizes all dynamic data and operational flags. The `volatile` keyword ensures that the compiler correctly handles accesses to this structure, as it can be modified by Interrupt Service Routines (ISRs).

- **UART Receive Queue (`rx_queue`)**: A queue of type `Queue` (defined in `queue.h`) is used to buffer characters received via UART. This decouples the UART ISR from the main processing loop.

### State Machine
The system operates using a state machine with two main states:

- **STATE_IDLE**: The system is waiting for user input
- **STATE_PROCESSING**: The system is processing a sequence of digits

### Interrupt Service Routines
Three main interrupt handlers manage system events:

- **timer_callback()**: Manages timing for LED blinking and digit processing
- **button_press_callback()**: Handles button press, toggling LED lock state  
- **uart_rx_callback()**: Adds incoming UART characters to a receive queue

---

## Assignment Description

### Core Functionality
The system implements a digit analysis program with the following behavior:

1. **Number Input via UART**: User inputs an integer number (e.g., 52719) via UART communication

2. **Sequential Digit Processing**: Each digit is processed every 0.5 seconds using timer interrupts:
   - **Even digits**: LED blinks (200ms ON – 200ms OFF)
   - **Odd digits**: LED toggles state and remains in the new state
   - All actions are logged to UART

3. **Button Interrupt Control**:
   - Button press during analysis locks the LED in its current state
   - Displays button press count: "Interrupt: Button pressed. LED locked. Count = X"
   - Analysis continues with UART logging but no LED actions
   - Second button press unlocks LED functionality

4. **Dynamic Input Handling**:
   - New number input interrupts current processing
   - System immediately starts fresh analysis with new number
   - Supports special characters like "-" for repeat functionality
   - Enhanced input validation and error handling

5. **Advanced Input Features**:
   - Numbers ending with "-" (dash) trigger repeat behavior
   - Backspace support for input correction
   - Non-digit character filtering
   - Buffer overflow protection (32 character limit)
   - Special control sequences (Ctrl+C to clear, Ctrl+X to terminate)

---

## Testing and Validation

### Test Scenarios
The system has been thoroughly tested with various scenarios:

| Action | Input/Event | Expected Outcome |
|--------|-------------|------------------|
| Basic Blink | UART: "24" | LED blinks for digit 2, then for digit 4 |
| Basic Toggle | UART: "13" | LED toggles for digit 1, then for digit 3 |
| Mixed Sequence | UART: "123" | LED toggles, then blinks, then toggles |
| Repeat Sequence | UART: "28-" | LED blinks for 2, then for 8, then repeats |
| LED Lock | Press Button | UART shows "LED Status: LOCKED" |
| Empty Input | UART: Enter | "No input. Enter new number:" prompt |
| Hyphen Only | UART: "- - -" | "No valid digits to process." prompt |
| Non-Digit Chars | UART: "1a2b-c3d" | Processes as "123" |
| Backspace | UART: "123" + Backspace | Input becomes "12", cursor moves back |
| Buffer Full | UART: (33 digits) | Processes first 32 digits |
| Ctrl+C | Press Ctrl+C | Terminal clears, prompt reappears |
| Ctrl+X | Press Ctrl+X | Program termination message, LED off |

### Validation Steps
- Send various digit sequences and observe LED behavior and UART output
- Verify correct timing for blinking and digit processing
- Press the button at different times (idle, during processing) and verify lock/unlock functionality
- Test boundary conditions like empty input, full buffer, and special characters

---

## Key Features

### 1. Queue-Based UART Processing
- **Interrupt-driven Reception**: UART characters are captured via interrupts and stored in a queue
- **Decoupled Processing**: Main loop processes queued characters independently of reception
- **Non-blocking Operation**: System remains responsive during character input

### 2. Robust Input Handling
- **Input Validation**: Filters non-digit characters automatically
- **Special Character Support**: Handles backspace, control sequences, and repeat indicators
- **Buffer Management**: Prevents overflow with 32-character input limit
- **Error Recovery**: Graceful handling of invalid inputs

### 3. Timer-Based Sequential Processing
- **Precise Timing**: 0.5-second intervals controlled by hardware timers
- **Interrupt-driven Architecture**: Timer interrupts ensure accurate timing regardless of other operations

### 4. LED Control System
- **State-based Control**: LED behavior depends on digit parity (even/odd)
- **Blink Pattern Generation**: 200ms ON/OFF cycles for even digits
- **Toggle Functionality**: State persistence for odd digits
- **Lock Mechanism**: Button-controlled LED disable/enable

### 5. Button Interrupt Handling
- **Improved Detection**: Uses pull-up resistors with falling edge detection for reliable button press detection (see [Challenge 3](#challenge-3-button-press-detection))
- **Lock State Management**: Toggles between locked and unlocked LED modes
- **ISR Optimization**: Minimal processing in interrupt handlers with flag-based main loop processing

### 6. UART Communication System
- **Real-time Logging**: All system actions reported via UART
- **Input Processing**: Non-blocking number input with validation
- **Status Reporting**: Comprehensive system state information
- **Error Handling**: Invalid input detection and user feedback

---

## Implementation Challenges and Solutions

### Challenge 1: ISR Optimization
**Problem**: Long or complex code execution within ISRs can lead to system instability, missed interrupts, and timing issues.

**Solution**: Implemented a flag-based approach where ISRs only update relevant status flags in the global `sys_state` structure. The main function checks these flags and performs complex processing, keeping ISRs short and responsive.

### Challenge 2: Input Processing During Digit Analysis
**Problem**: New UART inputs during digit sequence processing could interfere with ongoing operations.

**Solution**: Adopted a two-buffer strategy with reception buffer and separate processing buffer that isolates active processing from new inputs.

### Challenge 3: Button Press Detection
**Problem**: Button interrupts failed to trigger properly due to a logic conflict in the interrupt handler.

**Root Cause**: Our button uses active-low configuration with pull-up resistors (pin HIGH when idle, LOW when pressed). The interrupt handler had a condition `if(p->IDR&(1<<IRQ_pin_index))` that only executed callbacks when the pin was HIGH. However, with falling edge detection, the pin is LOW immediately after the interrupt triggers, so the callback never executed.

**Solution**: Modified the GPIO driver to execute callbacks regardless of pin state after interrupt trigger. This enabled proper falling edge detection with pull-up resistors - the preferred configuration for button interfaces (better noise immunity and immediate response on button press rather than release).

**Benefit**: Using pull-up resistors with falling edge detection is industry standard for embedded button interfaces, providing better noise immunity and immediate response.

---

## Technical Implementation

### Interrupt Architecture
The system uses a hierarchical interrupt structure:
- **Timer Interrupts**: Provide precise 0.5-second timing for digit processing
- **External Interrupts**: Handle button presses with proper edge detection
- **UART Interrupts**: Manage asynchronous communication using queue buffering

### State Machine Design
```
States:
- IDLE: Waiting for user input
- PROCESSING: Analyzing digits with timer control
```

### Hardware Peripherals
- **GPIO**: LED control (PA_5) and button input (PC_13) with interrupt capability
- **UART**: Serial communication for user interface with interrupt-driven reception
- **Timers**: Precise timing control for digit processing intervals

---

## Learning Objectives

Upon completion of this lab, students will have:

1. **Mastered Interrupt Programming**: Understanding of timer and external interrupt handling with proper ISR design
2. **Implemented Queue-Based Systems**: Experience with interrupt-driven data buffering and processing
3. **Developed State Machine Design**: Skills in system state management and transitions
4. **Applied Hardware Integration**: Coordinated multiple peripherals (GPIO, UART, Timers) with proper interrupt prioritization
5. **Enhanced Debugging Skills**: Experience with real-time debugging and embedded system analysis
6. **Learned Input Validation**: Robust handling of user input with error recovery and special character processing

---

## Advanced Topics Covered

- **Interrupt Service Routine Optimization**: Keeping ISRs minimal with flag-based main loop processing
- **Queue-Based Data Management**: Decoupling interrupt-driven data reception from processing
- **System Responsiveness**: Maintaining user interface responsiveness during background processing
- **Robust Error Handling**: Input validation, buffer management, and system recovery
---

## Conclusion

This laboratory exercise provides comprehensive experience with interrupt-driven embedded programming, demonstrating how to build responsive real-time systems that coordinate multiple hardware peripherals while maintaining precise timing control and robust user interaction. The implementation showcases advanced concepts including queue-based data management, optimized interrupt service routines, and comprehensive input validation.

---

**Course:** Microprocessors and Peripherals Laboratory  
**Institution:** Aristotle University of Thessaloniki  
**Semester:** Spring 2025
