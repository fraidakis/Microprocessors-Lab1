# Lab 1: Assembly Programming & UART Communication
**ARM Cortex-M4 Low-Level Programming**  
**Author:** Fraidakis Ioannis  
**Date:** April 2025  

## Overview
This laboratory exercise focuses on low-level microcontroller programming using ARM Cortex-M4 assembly language. The lab explores three key areas of embedded systems development:

1. **Character-based Hash Algorithm**: Implementation and optimization of a custom string hashing function.
2. **Recursive Computation**: Development of a recursive Fibonacci function.
3. **Hardware Communication**: Integration with UART peripheral for real-time data exchange.

The project demonstrates critical concepts such as mixed-language programming, hardware peripheral control, and algorithm optimization techniques for resource-constrained environments.

---

## Project Structure

The project is organized into a modular structure with dedicated folders for each problem solution:

```
Lab1/
├── README.MD             # This file - Lab 1 documentation
├── docs/                 # Documentation folder
│   ├── assignment-1.pdf  # Original assignment requirements
│   ├── report.pdf        # Detailed technical report
│   └── report.tex        # LaTeX source for report
└── project/              # Keil µVision project and source code
    ├── drivers/          # Hardware driver libraries
    ├── src/              # Source code implementations
    └── Lab1.uvprojx      # Keil µVision project file
```

### Main Entry Points
The following C files in `src/` provide different input methods for testing:
- `hardcoded_input.c`: For static testing without hardware.  
- `UART_input.c`: Receives input via UART hardware interface.
- `scanf_input.c`: Uses Keil's serial console for input.
- `test_optimization.c`: Performance benchmark harness for Question 1.
- `return_ptr.c`: Used to run `fib_return_ptr.s` variant for Question 3.

> **Note**: The project is configured to use `hardcoded_input.c` by default. To switch entry points, uncomment the desired file and comment out the currently active one.

### Implementation Folders
- **question1/**: String hashing implementations:
  - `hash.s`: Basic implementation
  - `hash_optimized.s`: Optimized version with performance improvements
  - `min_registers.s`: Uses only scratch registers (no stack usage)
- **question2/**: Digit reduction algorithm:
  - `sum_mod.s`: Straightforward implementation
- **question3/**: Fibonacci implementations:
  - `fibonacci.s`: Standard recursive implementation
  - `fib_return_ptr.s`: Returns the address of the result, stored in a local variable
- **bonus/**: Contains the solution for the bonus question

---

## Key Features

### 1. String Hasher
- Processes characters based on their type:
  - **Digits**: Mapped to predefined values using a lookup table
  - **Uppercase Letters**: Adds 2×ASCII value using a barrel shifter
  - **Lowercase Letters**: Adds the square of the offset from `'a'` using the `MLA` instruction
  - **Special Characters**: Skipped during processing
- Optimized for performance with techniques like single-pass processing, conditional execution, and combined operations

### 2. Digit Reducer
- Extracts each digit of a hash value and computes their sum
- Performs modulo operations to reduce the result

### 3. Fibonacci Function
- Implements a recursive Fibonacci function with stack management
- Handles base cases efficiently and uses `PUSH`/`POP` for parameter passing and return value handling

---

## Optimization Techniques
The following optimizations were applied to improve performance:
- **Single-Pass Processing**: Combined string length calculation with character processing
- **Conditional Execution**: Eliminated unnecessary branches using conditional instructions
- **Barrel Shifter Utilization**: Efficient multiplication by powers of 2
- **Multiply-Accumulate (MLA)**: Combined multiplication and addition in a single instruction
- **Post-Indexed Addressing**: Combined load and increment operations

> **Performance**: The optimized hash implementation achieved a **43.5% speedup** over the baseline implementation.

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

## Learning Objectives

Upon completion of this lab, students will have:

1. **Mastered ARM Assembly Fundamentals**: Understanding of instruction sets, addressing modes, and register usage
2. **Implemented Algorithm Optimization**: Applied various optimization techniques to improve performance
3. **Developed Mixed-Language Programming Skills**: Integrated assembly routines with C applications
4. **Gained Hardware Interface Experience**: Used UART peripheral for real-time communication
5. **Applied Software Engineering Practices**: Implemented modular design and comprehensive testing

---

## Future Work
Potential improvements for future exploration:
- **Division Optimization**: Replace `UDIV` instructions with multiplication-based division for constant divisors
- **Loop Unrolling**: Reduce branch prediction overhead for long input strings
- **Register Allocation Refinement**: Improve locality and reduce memory operations
- **Hybrid Digital Root**: Combine digit summation with modulo 9 properties for single-step digit addition

---

## Conclusion
This lab provided hands-on experience with ARM assembly programming, optimization techniques, and hardware interaction. The skills developed here form a strong foundation for advanced embedded systems development and serve as preparation for the more complex real-time systems explored in subsequent laboratory exercises.

---

**Course:** Microprocessors and Peripherals Laboratory
**Institution:** Aristotle University of Thessaloniki  
**Semester:** Spring 2025
