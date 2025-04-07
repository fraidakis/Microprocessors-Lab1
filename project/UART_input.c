/*

#include "platform.h"
#include <stdio.h>
#include <stdint.h>
#include "uart.h"
#include <string.h>
#include "queue.h"

#define BUFF_SIZE 128 // read buffer length

Queue rx_queue; // Queue for storing received characters

// External assembly functions
extern int compute_hash(const char *str);

extern int sum_and_mod7(int hash);
extern int reduction_result;

extern int fibonacci(int n);

extern int compute_checksum(const char *str);

// Interrupt Service Routine for UART receive
void uart_rx_isr(uint8_t rx);


int main(void)
{

    // Variables to help with UART read
    uint8_t rx_char = 0;
    char buff[BUFF_SIZE]; // The UART read string will be stored here
    uint32_t buff_index;

    // Initialize the receive queue and UART
    queue_init(&rx_queue, 128);
    uart_init(115200);
    uart_set_rx_callback(uart_rx_isr); // Set the UART receive callback function
    uart_enable();                     // Enable UART module

    __enable_irq(); // Enable interrupts

    uart_print("\r\n"); // Print newline

    while (1)
    {

        // Prompt the user to enter their full name
        uart_print("Enter your input:");
        buff_index = 0; // Reset buffer index

        do
        {
            // Wait until a character is received in the queue
            while (!queue_dequeue(&rx_queue, &rx_char))
                __WFI(); // Wait for Interrupt

            if (rx_char == 0x7F)
            { // Handle backspace character
                if (buff_index > 0)
                {
                    buff_index--;     // Move buffer index back
                    uart_tx(rx_char); // Send backspace character to erase on terminal
                }
            }
            else
            {
                // Store and echo the received character back
                buff[buff_index++] = (char)rx_char; // Store character in buffer
                uart_tx(rx_char);                   // Echo character back to terminal
            }
        } while (rx_char != '\r' && buff_index < BUFF_SIZE); // Continue until Enter key or buffer full

        // Replace the last character with null terminator to make it a valid C string
        buff[buff_index - 1] = '\0';
        uart_print("\r\n"); // Print newline

        // Check if buffer overflow occurred
        if (buff_index > BUFF_SIZE)
        {
            uart_print("Stop trying to overflow my buffer! I resent that!\r\n");
        }



        // __________ Assembly Function Calls _____ 

        int hash = compute_hash(buff);
        int reduced = sum_and_mod7(hash);
        int fib_result = fibonacci(reduced);
        int checksum = compute_checksum(buff);

				
				
        // __________ Assembly Function Calls _____ 
				
        printf("Input: %s\n", buff);
        printf("Hash: %d, Reduced: %d\nFibonacci: %d\nChecksum: %d\n", hash, reduced, fib_result, checksum);
				
				
				char str[128];
				
				sprintf(str, "Input: %s\r\n", buff);
				uart_print(str);

				sprintf(str, "Hash: %d\r\n", hash);
				uart_print(str);

				sprintf(str, "Reduced: %d\r\n", reduced);
				uart_print(str);

				sprintf(str, "Fibonacci: %d\r\n", fib_result);
				uart_print(str);

				sprintf(str, "Checksum: %d\r\n\n", checksum);
				uart_print(str);

    }
}


void uart_rx_isr(uint8_t rx)
{
    // Check if the received character is a printable ASCII character
    if (rx >= 0x0 && rx <= 0x7F)
    {
        // Store the received character
        queue_enqueue(&rx_queue, rx);
    }
}

*/