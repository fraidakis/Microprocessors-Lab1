#include "platform.h"
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 32
#define LED_PIN PA_5
#define BUTTON_PIN PC_13
#define TIMER_INTERVAL 100000 // 100ms
//#define DIGIT_INTERVAL 5      // 500ms
//#define DEBOUNCE_DELAY 1      // 100ms

//#define OUTPUT_BUFFER_SIZE 128
//char output_buffer[OUTPUT_BUFFER_SIZE];

//typedef enum
//{
//    STATE_IDLE,
//    STATE_PROCESSING
//} SystemState;

//volatile struct
//{
//    SystemState state;

//    char input_buffer[INPUT_BUFFER_SIZE + 1]; // +1 for null-terminator
//    uint8_t input_length;
//    uint8_t repeat;

//    char processing_buffer[INPUT_BUFFER_SIZE + 1]; // +1 for null-terminator
//    uint8_t processing_length;
//    uint8_t processing_position;
//    uint8_t digit_ready;

//    uint8_t timer_counter;
//    int8_t blink_counter;
//    uint8_t led_locked;
//    uint8_t button_event;
//    uint16_t button_count;
//    uint8_t debounce_delay;

//} system = {0};

//Queue rx_queue;

//void MX_GPIO_Init(void);
//void button_press_isr(int status);
//void handle_button_event(void);
//void uart_rx_callback(uint8_t c);

//void timer_handler()
//{
//    // Handle blink timing
//    if (system.blink_counter != -1)
//        system.blink_counter++;

//    if (system.debounce_delay > 0)
//        system.debounce_delay--;

//    // Handle digit timing
//    if (system.state == STATE_PROCESSING && ++system.timer_counter >= DIGIT_INTERVAL)
//    {
//        system.timer_counter = 0;
//        system.digit_ready = 1;
//    }
//}

//void start_processing(void)
//{
//    // Check if last character is a hyphen (for repeat flag)
//    system.repeat = (system.input_buffer[system.input_length - 1] == '-');

//    // Copy and filter out hyphens in one pass
//    uint8_t write_pos = 0;
//    for (uint8_t i = 0; i < system.input_length; i++)
//    {
//        if (system.input_buffer[i] != '-')
//        {
//            system.processing_buffer[write_pos++] = system.input_buffer[i];
//        }
//    }

//    // Set length and null-terminate
//    system.processing_length = write_pos;
//    system.processing_buffer[system.processing_length] = '\0';

//    // Handle empty input after filtering
//    if (system.processing_length == 0)
//    {
//        system.state = STATE_IDLE;
//        system.input_length = 0;
//        system.repeat = 0;

//        uart_print("\r\nNo valid input. Enter new number: ");
//        return;
//    }

//    // Reset input buffer for new input
//    system.input_length = 0;

//    // Reset processing state and counters
//    system.timer_counter = 0;
//    system.blink_counter = -1;
//    system.state = STATE_PROCESSING;
//    system.processing_position = 0;
//    system.digit_ready = 1; // Set digit_ready to process the first digit immediately

//    // Print the processing message
//    sprintf(output_buffer, "\r\nProcessing: %.*s%s\r\n", system.processing_length, system.processing_buffer, system.repeat ? "- (repeat)" : "");
//    uart_print(output_buffer);
//}

//void handle_uart_input()
//{
//    uint8_t c;

//    while (!system.button_event && queue_dequeue(&rx_queue, &c))
//    {
//        // Handle Enter key
//        if (c == '\r' || c == '\n')
//        {
//            if (system.input_length > 0)
//                start_processing();
//            else
//                uart_print("\r\nNo input. Enter new number: ");

//            continue;
//        }

//        // Handle Backspace
//        if (c == 0x7F && system.input_length > 0)
//        {
//            system.input_length--;
//            uart_tx(c);
//            continue;
//        }

//        // Handle other characters (digits and '-')
//        if ((c >= '0' && c <= '9') || c == '-')
//        {
//            if (system.input_length < INPUT_BUFFER_SIZE)
//            {
//                system.input_buffer[system.input_length++] = c;
//                uart_tx(c);
//            }
//            else
//            {
//                // Buffer is full, process as if Enter was pressed
//                sprintf(output_buffer, "\r\nBuffer full! Processing first %d characters...\r\n", INPUT_BUFFER_SIZE);
//                uart_print(output_buffer);
//                start_processing();
//            }
//        }
//    }
//}

//// Process the next digit in the sequence when a digit is ready
//void process_ready_digit()
//{
//    system.digit_ready = 0; // Reset digit ready flag

//    if (system.processing_position >= system.processing_length)
//    {
//        if (system.repeat)
//        {
//            system.processing_position = 0; // Reset position for repeat
//            system.digit_ready = 1;         // Process first digit of repeated sequence immediately
//            uart_print("\r\nRepeating sequence...\r\n");
//        }
//        else
//        {
//            system.state = STATE_IDLE;
//            uart_print("\r\nEnd of sequence. Enter new number: ");
//            return;
//        }
//    }

//    char digit = system.processing_buffer[system.processing_position++];

//    if (system.led_locked)
//    {
//        sprintf(output_buffer, "Digit %c -> Skipped (LED locked)\r\n", digit);
//    }
//    else if ((digit - '0') % 2 == 0)
//    {
//        if (system.blink_counter == -1)
//            system.blink_counter = 0; // Start/continue blinking
//        sprintf(output_buffer, "Digit %c -> Blink LED\r\n", digit);
//    }
//    else
//    {
//        gpio_toggle(LED_PIN);
//        system.blink_counter = -1; // Stop blinking
//        sprintf(output_buffer, "Digit %c -> Toggle LED\r\n", digit);
//    }

//    uart_print(output_buffer);
//}

//void update_led_blink_state()
//{
//    if (system.blink_counter % 2 == 0) // Blink at 0ms, 200ms, 400ms
//        gpio_toggle(LED_PIN);          // Toggle LED state
//}

///*********************** Main Function ***********************/

//int main(void)
//{ 
//    // GPIO Initialization
//    MX_GPIO_Init();
//    
//    // Timer Initialization
//    timer_init(TIMER_INTERVAL);
//    timer_set_callback(timer_handler);
//    
//	// UART Initialization
//    uart_init(115200);
//    uart_set_rx_callback(uart_rx_callback);
//    uart_enable();

//    // Queue Initialization
//    queue_init(&rx_queue, 2 * INPUT_BUFFER_SIZE);

//    // Enable interrupts
//    __enable_irq();

//    uart_print("\r\nLab 2 - Ready. Enter number (append '-' to repeat): ");

//    /********************** Main Loop *****************/

//    while (1)
//    {
//        __WFI(); // Wait for interrupt

//        handle_uart_input();

//        if (system.button_event)
//            handle_button_event(); // Handle button event

//        if (system.state == STATE_IDLE)
//            continue; // No processing needed, wait for new input

//        if (system.digit_ready)
//            process_ready_digit();

//        if (system.blink_counter != -1 && !system.led_locked)
//            update_led_blink_state(); // Update LED blink state
//    }
//}

///******************************* Helper Functions *******************************/

//// Button handler function
//void button_press_isr(int status)
//{
//   (void)status; // Explicitly mark as unused

//    // if (system.debounce_delay > 0)
//    //     return; // Ignore button press if debounce delay is active

//	//    uart_print("\r\nButton pin pressed\r\n");

//    system.led_locked ^= 1;
//    system.button_event = 1;
//    system.debounce_delay = DEBOUNCE_DELAY;
//	

//}

//// Button press function
//void handle_button_event(void)
//{
//    system.button_event = 0;

//    system.button_count++;
//    system.blink_counter = -1; // Reset blink counter

//    sprintf(output_buffer, "\r\nInterrupt: Button pressed. LED %s. Count: %d\r\n", system.led_locked ? "locked" : "unlocked", system.button_count);
//    uart_print(output_buffer);
//}

//// UART RX callback function
//void uart_rx_callback(uint8_t c)
//{
//    if (c >= 0x0 && c <= 0x7F)
//    {
//        queue_enqueue(&rx_queue, c);
//    }
//}

//// GPIO Initialization function
//void MX_GPIO_Init(void)
//{
//    // Initialize GPIO pins
//    gpio_set_mode(LED_PIN, Output);

//    gpio_set_mode(BUTTON_PIN,PullUp); // Both PullUp and PullDown are valid
//    gpio_set_trigger(BUTTON_PIN, Rising); // Only Rising is valid, Falling does not work
//    gpio_set_callback(BUTTON_PIN, button_press_isr);
//}