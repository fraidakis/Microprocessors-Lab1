# Microprocessors and Peripherals Laboratory Projects
**ARM Cortex-M4 Embedded Systems Development**  
**Author:** Fraidakis Ioannis  
**Institution:** Aristotle University of Thessaloniki  
**Date:** 2025  

## Overview
This repository contains three comprehensive laboratory exercises focusing on embedded systems programming using ARM Cortex-M4 microcontrollers (STM32F411RE). Each lab progressively builds upon fundamental concepts, from low-level assembly programming to advanced peripheral integration and real-time system design.

## Repository Structure

```
├── README.MD              # This file - main repository overview
├── Lab1/                  # Assembly Programming & UART Communication
│   ├── README.MD         # Lab 1 specific documentation
│   ├── docs/             # Lab 1 documentation and reports
│   └── project/          # Lab 1 source code and Keil project
├── Lab2/                  # Timer Interrupts & LED Control
│   ├── README.MD         # Lab 2 specific documentation
│   ├── docs/             # Lab 2 documentation and reports
│   └── project/          # Lab 2 source code and Keil project
└── Lab3/                  # Environmental Control System
    ├── README.MD         # Lab 3 specific documentation
    ├── docs/             # Lab 3 documentation and reports
    └── project/          # Lab 3 source code and Keil project
```
## Laboratory Exercises

### [Lab 1: Assembly Programming & UART Communication](./Lab1/)
**Focus:** Low-level ARM assembly programming and hardware communication
- **Key Topics:** String hashing algorithms, recursive functions, mixed-language programming
- **Technologies:** ARM Cortex-M4 Assembly, C, UART peripheral
- **Skills:** Algorithm optimization, register management, stack operations

### [Lab 2: Timer Interrupts & LED Control](./Lab2/)
**Focus:** Real-time programming with interrupts and peripheral control
- **Key Topics:** Timer interrupts, button handling, LED control, UART communication
- **Technologies:** C programming, Timer peripherals, GPIO, Interrupt handling
- **Skills:** Interrupt-driven programming, state machines, real-time processing

### [Lab 3: Environmental Control System](./Lab3/)
**Focus:** Advanced sensor integration and autonomous system design
- **Key Topics:** Environmental monitoring, sensor interfacing, adaptive control systems
- **Technologies:** DHT11 sensor, Touch/Button interfaces, LED indicators, UART menu system
- **Skills:** Sensor integration, system state management, user interface design

## Development Environment

### Prerequisites
- **Keil µVision IDE** (MDK-Arm)
  - Download and install from: [https://www.keil.com/download/product](https://www.keil.com/download/product)
  - Launch Keil and open Pack Installer (Project > Manage > Pack Installer)
  - Search for "STM32F411" and install "Keil::STM32F4xx_DFP - version 2.17.0"
  - **Note**: Version 2.17.0 ensures compatibility with project files. Newer versions require additional STM32CubeMX configuration
- **STM32F4xx Device Family Pack** (version 2.17.0)
- **Hardware:** STM32F411RE Nucleo board
- **Additional Components:** 
  - DHT11 temperature/humidity sensor (Lab 3)
  - Touch sensor or additional buttons (Lab 3)
  - Serial terminal application (PuTTY, Tera Term, etc.)
  - Micro USB cable for power and programming

### Quick Start
1. **Clone the Repository:**
   ```bash
   git clone https://github.com/fraidakis/Microprocessors-Peripherals.git
   cd Microprocessors-Peripherals
   ```

2. **Setup Keil µVision:**
   - Install Keil µVision IDE
   - Install STM32F4xx_DFP version 2.17.0 through Pack Installer
   - Open the desired lab's `.uvprojx` file in the respective `project/` folder

3. **Build and Run:**
   - Build the project (F7)
   - Start debug session (Ctrl+F5)
   - Run in simulator or deploy to hardware

### Detailed Setup Instructions

#### Building Any Lab Project
1. **Open Project in Keil µVision:**
   - Launch Keil µVision
   - Open the project file (`Lab1.uvprojx`, `Lab2.uvprojx`, or `Lab3.uvprojx`) in the respective `project/` folder

2. **Build the Project:**
   - Click "Build" button or press `F7`
   - Check "Build Output" window for errors/warnings
   - Wait for "0 Error(s), X Warning(s)" confirmation

3. **Debug and Run (Simulation):**
   - Click "Start/Stop Debug Session" (bug icon) or press `Ctrl+F5`
   - In debug mode:
     - Run: Click "Run" (►) or press F5
     - Step through: "Step Into" (F11) or "Step Over" (F10)
     - Set breakpoints: Click in left margin next to code lines
     - **Important**: Reset CPU (RST button) before each new run

4. **Deploy to Hardware:**
   - Connect STM32F4 Nucleo board via USB
   - In project options (Alt+F7), go to "Debug" tab
   - Switch from "Use Simulator" to "Use ST-Link Debugger"
   - Build and start debugging (Ctrl+F5)

5. **UART Communication Setup:**
   - Open serial terminal (e.g., PuTTY)
   - Select correct COM port (check Device Manager)
   - Set Baud Rate: 115200
   - Send characters to interact with application

## Learning Objectives

Through these laboratory exercises, students will:

1. **Master ARM Assembly Programming:** Understand instruction sets, addressing modes, and optimization techniques
2. **Implement Interrupt-Driven Systems:** Design responsive real-time applications using timer and external interrupts
3. **Integrate Hardware Peripherals:** Interface with sensors, communication protocols, and control devices
4. **Apply Software Engineering Practices:** Implement modular design, error handling, and system documentation
5. **Optimize Embedded Applications:** Balance performance, memory usage, and power consumption

## Key Concepts Covered

- **Low-Level Programming:** Assembly language, register allocation, stack management
- **Hardware Abstraction:** Driver development, peripheral configuration, hardware timers
- **Real-Time Systems:** Interrupt handling, task scheduling, timing constraints
- **Communication Protocols:** UART, serial communication, data formatting
- **System Design:** State machines, error handling, user interfaces
- **Performance Optimization:** Algorithm efficiency, memory management, code optimization

## Getting Help

Each lab folder contains detailed documentation including:
- Assignment specifications
- Implementation guidelines
- Testing procedures
- Performance analysis
- Troubleshooting guides

For technical support or questions, please refer to the individual lab README files or contact the course instructor.

---

**Course:** Microprocessors and Peripherals Laboratory
**Institution:** Aristotle University of Thessaloniki  
**Semester:** Spring 2025