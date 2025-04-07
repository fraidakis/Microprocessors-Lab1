# Microprocessors Laboratory - Lab 1
**Assembly Programming & UART Communication**  
**Author:** Fraidakis Ioannis  
**Date:** April 2025  


## Overview
This laboratory exercise focuses on low-level microcontroller programming using ARM Cortex-M4 assembly language. The lab explores three key areas of embedded systems development:

1. **Character-based Hash Algorithm**: Implementation and optimization of a custom string hashing function.
2. **Recursive Computation**: Development of a recursive Fibonacci function.
3. **Hardware Communication**: Integration with UART peripheral for real-time data exchange.

The project demonstrates critical concepts such as mixed-language programming, hardware peripheral control, and algorithm optimization techniques for resource-constrained environments.

---

## Repository Structure

The repository is organized as follows:

- **`/docs/`** - Documentation folder
  - **`Report.pdf`** - Detailed technical report with implementation details and results
  - **`Assignment.pdf`** - Original assignment requirements and specifications
- **`/project/`** - Contains all source code and Keil project files
  - **`Lab1.uvprojx`** - Keil µVision project file
  - **`main/`** - Main entry point files for different input methods
  - **`question1/`** - Assembly files for Question 1 (string hashing)
  - **`question2/`** - Assembly files for Question 2 (digit reduction)
  - **`question3/`** - Assembly files for Question 3 (Fibonacci function)
  - **`bonus/`** - Assembly files for the bonus question
- **`README.md`** - This file, providing an overview and instructions

---

## Project Structure

The project is organized into a modular structure with dedicated folders for each problem solution:

### Main Entry Points
The following C files provide different input methods for testing:
- `hardcoded_input.c`: For static testing without hardware.  
- `UART_input.c`: Receives input via UART hardware interface.
- `scanf_input.c`: Uses Keil's serial console for input.
- `test_optimization.c`: Performance benchmark harness for Question 1.
- `return_ptr.c`: Used to run `fib_return_ptr.s` variant for Question 3.

> **Note**: The project is configured to use `hardcoded_input.c` by default. To switch entry points, uncomment the desired file and comment out the currently active one.

### Implementation Folders
- **question1/**: String hashing implementations:
  - `hash.s`: Basic implementation.
  - `hash_optimized.s`: Optimized version with performance improvements.
  - `min_registers.s`: Uses only scratch registers (no stack usage).
- **question2/**: Digit reduction algorithm:
  - `sum_mod.s`: Straightforward implementation.
- **question3/**: Fibonacci implementations:
  - `fibonacci.s`: Standard recursive implementation.
  - `fib_return_ptr.s`: Returns the address of the result, stored in a local variable.
- **bonus/**: Contains the solution for the bonus question.

---

## Key Features

### 1. String Hasher
- Processes characters based on their type:
  - **Digits**: Mapped to predefined values using a lookup table.
  - **Uppercase Letters**: Adds 2×ASCII value using a barrel shifter.
  - **Lowercase Letters**: Adds the square of the offset from `'a'` using the `MLA` instruction.
  - **Special Characters**: Skipped during processing.
- Optimized for performance with techniques like single-pass processing, conditional execution, and combined operations.

### 2. Digit Reducer
- Extracts each digit of a hash value and computes their sum.
- Performs modulo operations to reduce the result.

### 3. Fibonacci Function
- Implements a recursive Fibonacci function with stack management.
- Handles base cases efficiently and uses `PUSH`/`POP` for parameter passing and return value handling.

---

## Optimization Techniques
The following optimizations were applied to improve performance:
- **Single-Pass Processing**: Combined string length calculation with character processing.
- **Conditional Execution**: Eliminated unnecessary branches using conditional instructions.
- **Barrel Shifter Utilization**: Efficient multiplication by powers of 2.
- **Multiply-Accumulate (MLA)**: Combined multiplication and addition in a single instruction.
- **Post-Indexed Addressing**: Combined load and increment operations.

> **Performance**: The optimized hash implementation achieved a **43.5% speedup** over the baseline implementation.

---

## How to Run the Project  

### Prerequisites  
1. **Keil µVision IDE**:  
   - Download and install Keil µVision from the official website: [https://www.keil.com/download/product](https://www.keil.com/download/product) (MDK-Arm).
   - Launch Keil and open Pack Installer by clicking on the "Pack Installer" icon or through Project > Manage > Pack Installer.
   - Search for "STM32F411" and install "Keil::STM32F4xx_DFP - version 2.17.0".
   - **Note on version 2.17.0**: This specific version ensures compatibility with our project files. Newer versions require additional STM32CubeMX configuration.

2. **Hardware (Optional)**:  
   - An ARM Cortex-M4-based microcontroller (e.g., STM32F411RE Nucleo board).
   - Micro USB cable for power and programming.
   - UART interface for hardware communication.

---

### Steps to Clone and Run  
1. **Clone the Repository**:  
   Open a terminal and run:  
   ```bash
   git clone https://github.com/fraidakis/Microprocessors-Lab1.git
   cd Microprocessors-Lab1
   ```

2. **Open the Project in Keil µVision**:  
   - Launch Keil µVision.  
   - Open the project file (`Lab1.uvprojx`) located in the repository.  

3. **Build the Project**:  
   - Click on the "Build" button or press `F7` to compile the project.
   - Check the "Build Output" window at the bottom for any errors or warnings.
   - Wait until you see "0 Error(s), X Warning(s)" indicating a successful build.

4. **Debug and Run the Simulation**:  
   - Click on the "Start/Stop Debug Session" button (bug icon) or press `Ctrl+F5`.
   - Once in debug mode, you can:
     - Run the program by clicking the "Run" button (►) or pressing F5.
     - Step through code with "Step Into" (F11) or "Step Over" (F10).
     - Set breakpoints by clicking in the left margin next to code lines.
     - **Note for running**: Before each new run make sure to reset the CPU (RST button).

5. **Deploy to Hardware**:  
   - Connect your STM32F4 Nucleo board via USB.
   - In project options (Alt+F7), go to the "Debug" tab.
   - Switch from "Use Simulator" to "Use ST-Link Debugger".
   - Build the project and start debugging (Ctrl+F5).
   - The program will automatically run on the hardware.

6. **Test UART Communication**:  
   - Open a serial terminal application (e.g., PuTTY):
     - Select the correct COM port (check Device Manager for the assigned number).
     - Set Baud Rate to 115200.
   - Send characters through the terminal to interact with your application.

---



## Testing & Validation
The implementations were tested using both static test vectors and dynamic inputs. Performance was evaluated using the Keil μVision simulator with the "Performance Analyzer" enabled.

### Validation Results
| Input           | Raw Hash | Reduced | Fibonacci | Checksum | Characteristics |
|------------------|----------|---------|-----------|----------|------------------|
| `""` (empty)    | 0        | 0       | 0         | 0        | Edge case        |
| `"Keil uVision"`| 1752     | 1       | 1         | 90       | Alphabetic       |
| `"STM32-F411RE"`| 983      | 6       | 8         | 3        | Mixed case       |
| `"Arm 2025"`    | 601      | 0       | 0         | 123      | Alphanumeric     |

---

## Future Work
Potential improvements for future exploration:
- **Division Optimization**: Replace `UDIV` instructions with multiplication-based division for constant divisors.
- **Loop Unrolling**: Reduce branch prediction overhead for long input strings.
- **Register Allocation Refinement**: Improve locality and reduce memory operations.
- **Hybrid Digital Root**: Combine digit summation with modulo 9 properties for single-step digit addition.

---

## Conclusion
This lab provided hands-on experience with ARM assembly programming, optimization techniques, and hardware interaction. The skills developed here form a strong foundation for advanced embedded systems development.