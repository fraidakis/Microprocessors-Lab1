/****************************************************************************
 * @file main.c
 * @brief Main application file for an LED control system.
 *
 * This program allows users to control an LED through UART commands and a button.
 * Users can input a sequence of digits. Even digits cause the LED to blink,
 * and odd digits cause the LED to toggle. A button press can lock/unlock the LED,
 * preventing it from changing state based on digit processing.
 * The input sequence can be set to repeat by appending a hyphen '-' to the end.
 *
 * Features:
 * - UART communication for input and output.
 * - Button interrupt for LED lock/unlock.
 * - Timer-based LED blinking and digit processing.
 * - Input buffering and processing.
 * - Special commands: Ctrl+C to clear screen, Ctrl+X to terminate.
 ****************************************************************************/

// Standard and platform-specific headers
#include "platform.h" /* Platform-specific definitions (e.g., __enable_irq, __disable_irq, __WFI) */
#include "gpio.h"     /* GPIO control functions */
#include "timer.h"    /* Timer control functions */
#include "uart.h"     /* UART communication functions */
#include "queue.h"    /* Queue data structure for UART RX buffering */

// Standard C library headers
#include <stdio.h>  /* For sprintf */
#include <stdlib.h> /* For exit() */


/****************************************************************************/
/*                          Constants and Defines                           */
/****************************************************************************/

/* Buffer and pin configuration */
#define INPUT_BUFFER_SIZE 32        /* Maximum size of UART input buffer */
#define OUTPUT_BUFFER_SIZE 128      /* Maximum size of formatting output buffer */
#define LED_PIN PA_5                /* GPIO pin for the LED */
#define BUTTON_PIN PC_13            /* GPIO pin for the button */

/* Timing parameters */
#define TIMER_INTERVAL 100000       /* Base timer interval (100000 us = 100ms) */
#define DIGIT_INTERVAL 5            /* Time between processing digits (5 * 100ms = 500ms) */
#define DEBOUNCE_DELAY 1            /* Button debounce delay (100ms) */
#define BLINK_DELAY 2               /* LED blink interval (200ms = 2 * 100ms) */

/* ASCII Control characters */
#define KEY_BACKSPACE 0x7F          /* Backspace/Delete */
#define KEY_CTRL_C_CLEAR 0x03       /* Ctrl+C - Clear Screen & Reset Input */
#define KEY_CTRL_X_TERMINATE 0x18   /* Ctrl+X - Terminate Program */

/* LED blinking states */
#define LED_BLINK_INACTIVE -1       /* LED is not blinking */

/* Output buffer for formatted messages */
char output_buffer[OUTPUT_BUFFER_SIZE];


/****************************************************************************/
/*                            Type Definitions                              */
/****************************************************************************/

/**
 * @brief Defines the possible states of the system.
 */
typedef enum
{
    STATE_IDLE,      /* The system is idle, waiting for user input. */
    STATE_PROCESSING /* The system is actively processing a sequence of digits. */
} SystemState;


/****************************************************************************/
/*                        Global Variables and State                        */
/****************************************************************************/

/**
 * @brief Global structure holding the system's state and operational variables.
 *
 * This volatile struct centralizes all dynamic data used by the application,
 * ensuring that changes made in interrupt service routines are correctly
 * observed by the main loop and other functions.
 */
volatile struct
{
    SystemState state; /* Current state of the system (IDLE or PROCESSING). */

    // UART Input Handling
    char input_buffer[INPUT_BUFFER_SIZE + 1]; /* Buffer to store incoming UART characters (+1 for null-terminator). */
    uint8_t input_length;                     /* Current number of characters in input_buffer. */
    uint8_t repeat;                           /* Flag (0 or 1) indicating if the current sequence should repeat. */

    // Digit Processing
    char processing_buffer[INPUT_BUFFER_SIZE + 1]; /* Buffer storing the filtered digits to be processed (+1 for null-terminator). */
    uint8_t processing_length;                     /* Number of digits in processing_buffer. */
    uint8_t processing_position;                   /* Current position/index in processing_buffer. */
    uint8_t digit_ready;                           /* Flag (0 or 1) indicating if the next digit is ready for processing. */
    uint8_t digit_process_timer;                   /* Counter incremented by the timer, used for DIGIT_INTERVAL timing. */

    // LED Control and Button Handling
    int8_t led_blink_timer;  /* Counter for LED blinking timing. -1 means not blinking, >=0 means blinking. */
    uint8_t led_locked;      /* Flag (0 or 1) indicating if the LED state is locked by button press. */
    uint8_t button_event;    /* Flag (0 or 1) indicating a button press event has occurred. */
    uint16_t button_count;   /* Counter for the number of button presses. */
    uint8_t debounce_timer;  /* Counter for button debounce timing. */

} sys_state = {
    .state = STATE_IDLE, /* Initial state */
    .input_length = 0,
    .repeat = 0,
    .processing_length = 0,
    .processing_position = 0,
    .digit_ready = 0,
    .digit_process_timer = 0,
    .led_blink_timer = LED_BLINK_INACTIVE, /* Not blinking initially */
    .led_locked = 0, /* LED unlocked initially */
    .button_event = 0,
    .button_count = 0,
    .debounce_timer = 0};

Queue rx_queue; /* Queue for buffering characters received via UART. */


/****************************************************************************/
/*                          Function Prototypes                             */
/****************************************************************************/

// Initialization
void MX_GPIO_Init(void);

// Interrupt Service Routines & Callbacks
void timer_callback(void);
void button_press_callback(int status);
void uart_rx_callback(uint8_t c);

// Event Handling
void handle_button_event(void);
void handle_uart_input(void);

// Core Processing Logic
void start_processing(void);
void process_ready_digit(void);
void update_led_blink_state(void);

// UI & Utility Functions
void welcome_message(void);
void clear_terminal(void);
void terminate_program(void);


/******************************************************************************/
/*                            Main Application                                */
/******************************************************************************/

/**
 * @brief Main function of the application.
 *
 * Initializes hardware (GPIO, Timer, UART), enables interrupts,
 * displays a welcome message, and enters an infinite loop to handle
 * events and process data.
 */
int main(void)
{
    // Initialize GPIO pins for LED and Button
    MX_GPIO_Init();

    // Initialize Timer
    timer_init(TIMER_INTERVAL);        /* Configure timer for 100ms interval */
    timer_set_callback(timer_callback); /* Register timer interrupt handler */

    // Initialize UART
    uart_init(115200);                      /* Configure UART at 115200 baud */
    uart_set_rx_callback(uart_rx_callback); /* Register UART RX interrupt callback */
    uart_enable();                          /* Enable UART peripheral */

    // Initialize RX Queue for UART
    queue_init(&rx_queue, INPUT_BUFFER_SIZE);

    // Enable global interrupts
    __enable_irq();

    // Display welcome message and initial prompt
    welcome_message();

    /********************** Main Loop *****************/
    while (1)
    {
        __WFI();  /* Enter low-power mode, waiting for an interrupt (Timer, Button, UART RX) */

        /* Process any pending UART input */
        handle_uart_input();

        /* Handle button press event if pending */
        if (sys_state.button_event)
            handle_button_event();

        /* Skip processing logic and wait for new input in idle state */ 
        if (sys_state.state == STATE_IDLE)
            continue;

        /* Process the next digit if ready */
        if (sys_state.digit_ready)
            process_ready_digit();

        /* Update LED blinking if active and not locked */
        if (sys_state.led_blink_timer != LED_BLINK_INACTIVE && !sys_state.led_locked)
            update_led_blink_state(); /* Update LED state for blinking */
    }

    // Note: Embedded applications typically do not exit main.
    // The program runs indefinitely until a reset or Ctrl+X is pressed.
}


/******************************************************************************/
/*                         Initialization Functions                           */
/******************************************************************************/

/**
 * @brief Initializes GPIO pins for the LED and Button.
 *
 * Configures LED_PIN as output.
 * Configures BUTTON_PIN as input with pull-up resistor (HIGH when not pressed, active LOW)
 * and sets up a falling edge interrupt (triggers when button is pressed and 
 * pin transitions from HIGH to LOW). */
void MX_GPIO_Init(void)
{
    // Configure LED_PIN (PA_5) as an output pin
    gpio_set_mode(LED_PIN, Output);

    // Configure BUTTON_PIN (PC_13)
    gpio_set_mode(BUTTON_PIN, PullUp);               /* Set as input with internal pull-up resistor */
    gpio_set_trigger(BUTTON_PIN, Falling);           /* Trigger interrupt on Falling edge (active LOW, from HIGH to LOW) */
    gpio_set_callback(BUTTON_PIN, button_press_callback); /* Register button interrupt handler */
}

/******************************************************************************/
/*                   Interrupt Service Routines & Callbacks                   */
/******************************************************************************/

/**
 * @brief Timer interrupt handler, called periodically by the timer.
 *
 * This function manages timing for:
 * 1. LED blinking: Increments `led_blink_timer` if blinking is active.
 * 2. Button debouncing: Decrements `debounce_timer` if active.
 * 3. Digit processing: Increments `digit_process_timer` and sets `digit_ready`
 *    when `DIGIT_INTERVAL` is reached during `STATE_PROCESSING`.
 */
void timer_callback()
{
    // Handle LED blink timing
    if (sys_state.led_blink_timer != LED_BLINK_INACTIVE)
    {
        sys_state.led_blink_timer++; /* Increment blink counter if blinking is active */
    }

    // Handle button debounce timing
    if (sys_state.debounce_timer > 0)
    {
        sys_state.debounce_timer--; /* Decrement debounce delay counter */
    }

    // Handle digit processing timing
    // If the system is in STATE_PROCESSING, increment the timer_counter.
    // When timer_counter reaches DIGIT_INTERVAL, a digit is ready for processing.
    if (sys_state.state == STATE_PROCESSING && ++sys_state.digit_process_timer >= DIGIT_INTERVAL)
    {
        sys_state.digit_process_timer = 0; /* Reset counter for the next digit */
        sys_state.digit_ready = 1;   /* Signal that a digit is ready */
    }
}

/**
 * @brief Button press interrupt service routine (ISR).
 *
 * Called when the button's configured edge (Rising) is detected.
 * It handles button debouncing, toggles the LED lock state,
 * sets a button event flag, and resets the debounce delay.
 * @param status The status from the GPIO interrupt (unused in this implementation).
 */
void button_press_callback(int status)
{
    (void)status; /* Explicitly mark 'status' parameter as unused to prevent compiler warnings. */

    // Ignore button press if debounce delay is currently active
    if (sys_state.debounce_timer > 0)
        return;
        
    // Set debounce delay to prevent multiple triggers from a single press
    sys_state.debounce_timer = DEBOUNCE_DELAY;

    // Toggle the LED lock state
    sys_state.led_locked ^= 1; /* XOR with 1 to toggle between 0 and 1 */

    // Signal that a button event has occurred for processing in the main loop
    sys_state.button_event = 1;
}

/**
 * @brief UART receive (RX) callback function.
 *
 * Called by the UART driver whenever a character is received.
 * It validates if the character is a standard ASCII character (0x00-0x7F)
 * and enqueues it into the `rx_queue` for later processing.
 * @param c The character received via UART.
 */
void uart_rx_callback(uint8_t c)
{
    // Process only valid ASCII characters (0 to 127)
    if (c <= 0x7F) /* Check if c is within the standard ASCII range */
    {
        // Add the received character to the RX queue
        queue_enqueue(&rx_queue, c);
    }
}


/******************************************************************************/
/*                           Event Handling Functions                         */
/******************************************************************************/

/**
 * @brief Handles button press events.
 *
 * This function is called from the main loop when `sys_state.button_event` is set.
 * It clears the event flag, increments the button press counter, resets the LED
 * blink counter (as button interaction overrides current blinking), and prints
 * information about the button press and new LED lock status to the UART.
 * If the system is idle and no input is pending, it re-prompts the user.
 */
void handle_button_event(void)
{
    sys_state.button_event = 0; /* Clear the button event flag */

    sys_state.button_count++;   /* Increment the total button press count */
    sys_state.led_blink_timer = LED_BLINK_INACTIVE; /* Stop any active LED blinking */

    // Print button press information to UART
    uart_print("\r\n+----------------------------+\r\n");
    sprintf(output_buffer, "| INTERRUPT: Button press #%-1d |\r\n", sys_state.button_count);
    uart_print(output_buffer);
    sprintf(output_buffer, "| LED Status: %-14s |\r\n",sys_state.led_locked ? "LOCKED" : "UNLOCKED");
    uart_print(output_buffer);
    uart_print("+----------------------------+\r\n\r\n");

    // If the system is idle and no input has been typed, re-display the prompt.
    // This handles cases where a button press might interrupt the user's expectation of an input prompt.
    if (sys_state.state == STATE_IDLE && sys_state.input_length == 0)
    {
        uart_print("Enter number: ");
    }
}

/**
 * @brief Processes characters from the UART receive queue.
 *
 * This function is called from the main loop. It dequeues characters
 * from `rx_queue` and handles them based on their value:
 * - Enter ('\r' or '\n'): Finalizes input and starts processing.
 * - Backspace (0x7F): Deletes the last character from the input buffer.
 * - Digits ('0'-'9') and Hyphen ('-'): Appends to the input buffer.
 * - Ctrl+C (0x03): Clears the terminal and resets input state.
 * - Ctrl+X (0x18): Terminates the program.
 *
 * UART input processing is paused if a button event is pending to prioritize button handling.
 */
void handle_uart_input()
{
    uint8_t c; /* Variable to store the dequeued character */

    // Process UART queue only if no button event is pending,
    // giving button presses higher priority in terms of immediate feedback/handling.
    while (!sys_state.button_event && queue_dequeue(&rx_queue, &c))
    {
        // Handle Enter key (Carriage Return or Line Feed)
        if (c == '\r' || c == '\n')
        {
            if (sys_state.input_length > 0)
                start_processing(); /* Begin processing the entered number sequence */
            else
                uart_print("\r\nNo input. Enter new number: "); /* Prompt for new input if no characters were entered */
        }

        // Handle Backspace key (ASCII DEL character)
        else if (c == KEY_BACKSPACE && sys_state.input_length > 0)
        {
            sys_state.input_length--; /* Remove last character from buffer */
            uart_tx(c);               /* Echo backspace to terminal (moves cursor back) */
        }

        // Handle valid input characters (digits '0'-'9' and hyphen '-')
        else if ((c >= '0' && c <= '9') || c == '-')
        {
            if (sys_state.input_length < INPUT_BUFFER_SIZE)
            {
                // Add character to buffer and echo to terminal
                sys_state.input_buffer[sys_state.input_length++] = c;
                uart_tx(c);
            }
            else
            {
                // Input buffer is full
                sprintf(output_buffer, "\r\nBuffer full! Processing first %d characters...\r\n", INPUT_BUFFER_SIZE);
                uart_print(output_buffer);
                start_processing(); /* Process the current buffer content */
            }
        }

        // Handle Ctrl+C (ASCII ETX - End of Text)
        else if (c == KEY_CTRL_C_CLEAR)
        {
            clear_terminal(); /* Clear the terminal screen */
            // Reset input state
            sys_state.input_length = 0;
            sys_state.input_buffer[0] = '\0'; /* Null-terminate for safety */
            sys_state.state = STATE_IDLE;     /* Ensure system returns to idle state */
        }

        // Handle Ctrl+X (ASCII CAN - Cancel)
        else if (c == KEY_CTRL_X_TERMINATE)
            terminate_program(); /* Terminate the application */

        // Other characters are ignored
    }
}


/******************************************************************************/
/*                         Core Processing Logic                              */
/******************************************************************************/

/**
 * @brief Initiates the processing of the entered number sequence.
 *
 * This function is called when the user presses Enter after typing input.
 * It performs the following steps:
 * 1. Checks if the last character of the input is a hyphen ('-') to set the `repeat` flag.
 * 2. Copies the input from `sys_state.input_buffer` to `sys_state.processing_buffer`,
 *    filtering out all hyphens during the copy.
 * 3. Null-terminates the `processing_buffer`.
 * 4. If the `processing_buffer` is empty after filtering (e.g., input was "---"),
 *    it resets to `STATE_IDLE` and prompts for new input.
 * 5. Clears the original `sys_state.input_buffer`.
 * 6. Resets processing-related state variables (`digit_process_timer`, 
 *    `led_blink_timer`, `processing_position`).
 * 7. Sets the system state to `STATE_PROCESSING`.
 * 8. Sets `digit_ready = 1` to process the first digit immediately.
 * 9. Prints a message to UART indicating the sequence being processed.
 */
void start_processing(void)
{
    // Null-terminate the input buffer before using it with string functions or checks
    sys_state.input_buffer[sys_state.input_length] = '\0';

    // Check if the last character of the raw input is a hyphen to enable repeat mode
    sys_state.repeat = (sys_state.input_buffer[sys_state.input_length - 1] == '-');

    // Filter out hyphens from input_buffer and store the result in processing_buffer
    uint8_t write_pos = 0; /* Current writing position in processing_buffer */
    for (uint8_t i = 0; i < sys_state.input_length; i++)
    {
        if (sys_state.input_buffer[i] != '-') /* Only copy non-hyphen characters (digits) */
            sys_state.processing_buffer[write_pos++] = sys_state.input_buffer[i];
    }
    sys_state.processing_length = write_pos;                         /* Set the length of the filtered sequence */
    sys_state.processing_buffer[sys_state.processing_length] = '\0'; /* Null-terminate the processing_buffer */

    // Handle cases where input becomes empty after filtering (e.g., input was "-" or "---")
    if (sys_state.processing_length == 0)
    {
        sys_state.state = STATE_IDLE;     /* Return to idle state */
        sys_state.input_length = 0;       /* Clear original input buffer */
        sys_state.input_buffer[0] = '\0'; /* Null-terminate for safety */
        sys_state.repeat = 0;             /* Ensure repeat is off */

        uart_print("\r\nNo valid digits to process. Enter new number: ");
        return;
    }

    // Clear the original input buffer now that its content has been copied
    sys_state.input_length = 0;
    sys_state.input_buffer[0] = '\0'; /* Null-terminate for safety */

    // Reset state variables for the new processing sequence
    sys_state.digit_process_timer = 0; /* Reset timer for digit processing */
    sys_state.led_blink_timer = LED_BLINK_INACTIVE; /* Ensure LED is not blinking initially for this sequence */
    sys_state.state = STATE_PROCESSING; /* Set system state to processing */
    sys_state.processing_position = 0;  /* Start from the first digit of the processing_buffer */
    sys_state.digit_ready = 1;          /* Signal that the first digit is ready to be processed immediately */

    // Print the sequence that will be processed
    sprintf(output_buffer, " -> %.*s%s\r\n", sys_state.processing_length, sys_state.processing_buffer, sys_state.repeat ? "- (repeat)" : "");
    uart_print(output_buffer);
    uart_print("+------------------------+\r\n"); 
}

/**
 * @brief Processes a single digit from the `processing_buffer`.
 *
 * This function is called when `sys_state.digit_ready` is true.
 * It performs the following:
 * 1. Resets `sys_state.digit_ready` to 0.
 * 2. Checks if the end of the `processing_buffer` has been reached:
 *    - If `sys_state.repeat` is true, resets `sys_state.processing_position` to 0
 *      to start over.
 *    - If `sys_state.repeat` is false, sets `sys_state.state` to `STATE_IDLE`,
 *      prints an end-of-sequence message, and prompts for new input.
 * 3. If not at the end, retrieves the current digit from `processing_buffer`.
 * 4. Based on the digit and `sys_state.led_locked` status:
 *    - If LED is locked: Prints a "Skipped" message.
 *    - If digit is even: Starts LED blinking (sets `sys_state.blink_counter` to 0
 *      if not already blinking) and prints "Blink LED".
 *    - If digit is odd: Toggles the LED, stops any blinking (sets
 *      `sys_state.blink_counter` to -1), and prints "Toggle LED".
 * 5. Increments `sys_state.processing_position`.
 */
void process_ready_digit()
{
    sys_state.digit_ready = 0; /* Reset the flag, indicating the digit is being processed */

    // Check if all digits in the processing_buffer have been processed
    if (sys_state.processing_position >= sys_state.processing_length)
    {
        if (sys_state.repeat) /* If repeat mode is enabled */
        {
            sys_state.processing_position = 0; /* Reset position to start from the beginning */
            uart_print("\r\nRepeating sequence...\r\n");
        }
        else /* End of sequence and no repeat */
        {
            sys_state.state = STATE_IDLE;                 /* Return to idle state */
            uart_print("+------------------------+\r\n"); /* Footer for the digit processing table */
            uart_print("\r\n*** End of sequence ***\r\n\r\n");
            uart_print("Enter new number: "); /* Prompt for new input */
            return;                           /* Exit function, no more digits to process */
        }
    }

    // Get the current digit to process and increment the position for the next cycle
    char digit_char = sys_state.processing_buffer[sys_state.processing_position++];
    int digit_val = digit_char - '0'; /* Convert char '0'-'9' to int 0-9 */

    // Action depends on whether the LED is locked
    if (sys_state.led_locked)
    {
        sprintf(output_buffer, "| Digit [%c] | Skipped (LED locked) |\r\n", digit_char);
    }
    else /* LED is not locked, process the digit */
    {
        if (digit_val % 2 == 0) /* Even digit */
        {
            // Start blinking if not already blinking (previous digit was odd)
            // Continue blinking if already in blink mode (previous digit was even), to prevent restarting the cycle
            if (sys_state.led_blink_timer == LED_BLINK_INACTIVE)
                sys_state.led_blink_timer = 0; /* Start blink cycle */
            
            sprintf(output_buffer, "| Digit [%c] | Blink  LED |\r\n", digit_char);
        }
        else /* Odd digit */
        {
            gpio_toggle(LED_PIN); /* Toggle LED state immediately */
            sys_state.led_blink_timer = LED_BLINK_INACTIVE; /* Stop blinking if it was active */
            sprintf(output_buffer, "| Digit [%c] | Toggle LED |\r\n", digit_char);
        }
    }

    uart_print(output_buffer); /* Print the action taken for the current digit */
}

/**
 * @brief Updates the LED state for blinking.
 *
 * This function is called from the main loop when `sys_state.blink_counter` is not -1
 * (indicating blinking is active) and the LED is not locked.
 * It toggles the LED at intervals defined by `BLINK_DELAY` based on the `blink_counter`
 * value, which is incremented by `timer_callback`.
 */
void update_led_blink_state()
{
    // The blink_counter is incremented by the timer_callback every TIMER_INTERVAL (100ms).
    // BLINK_DELAY is the number of timer ticks for one phase of the blink.
    // So, LED toggles every BLINK_DELAY * TIMER_INTERVAL.
    
    // Example: BLINK_DELAY = 2 (200ms). LED toggles when blink_counter is 0ms, 200ms, 400ms, etc.
    if (sys_state.led_blink_timer % BLINK_DELAY == 0)
    {
        gpio_toggle(LED_PIN); /* Toggle LED state */
    }
}


/******************************************************************************/
/*                       User Interface & Utility Functions                   */
/******************************************************************************/

/**
 * @brief Displays the welcome message and initial instructions to the user via UART.
 */
void welcome_message()
{
    uart_print("\r\n+--------------------------------------+\r\n");
    uart_print("|           LED Control System         |\r\n");
    uart_print("+--------------------------------------+\r\n");

    uart_print("\r\nWelcome to the LED Blinker Program!\r\n");
    uart_print("Press the button to lock/unlock the LED.\r\n");
    uart_print("Enter a number (0-9 digits) followed by '-' to repeat.\r\n");
    uart_print("Press Enter to start processing.\r\n");
    uart_print("Ctrl+C to clear screen, Ctrl+X to terminate.\r\n\r\n");

    uart_print("Enter number: "); /* Initial prompt for input */
}

/**
 * @brief Clears the terminal screen and re-displays the welcome message and prompt.
 *
 * Uses ANSI escape codes to clear the screen (\033[2J) and move the cursor
 * to the home position (\033[H).
 */
void clear_terminal(void)
{
    // ANSI escape sequence:
    // \033[2J : Clear entire screen
    // \033[H  : Move cursor to home position (top-left)
    uart_print("\033[2J\033[H");

    // After clearing, display the welcome message and prompt again
    welcome_message();
}

/**
 * @brief Terminates the program gracefully.
 *
 * Prints a termination message, disables interrupts, turns off the LED,
 * disables hardware peripherals, and exits the program.
 */
void terminate_program(void)
{
    uart_print("\r\n\n+--------------------------------------+\r\n");
    uart_print("|       Program terminated by user     |\r\n");
    uart_print("+--------------------------------------+\r\n");

    // Disable global interrupts to prevent further ISR execution
    __disable_irq();

    // Perform cleanup:

    // Turn off LED
    gpio_set(LED_PIN, 0); 

    // Stop the timer
    timer_disable(); 

    // Exit the program.
    exit(0);
}
