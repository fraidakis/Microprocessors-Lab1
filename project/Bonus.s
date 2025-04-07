;  ____________________________________________________________________________
; |																			   |
; |                         Answer to BONUS question                           |
; |____________________________________________________________________________|

		
		AREA Checksum, CODE, READONLY
		EXPORT compute_checksum
		EXPORT checksum_result          ; Export the checksum result variable so it can be accessed by other modules



; Function: compute_checksum
; ---------------------------
; This function computes a simple checksum for a given null-terminated string.
; The checksum is calculated using an XOR operation over all characters in the string.
 

compute_checksum

		; R0 -> Initially holds the pointer to the input string and later holds the computed checksum.
		; R1 -> Temporarily stores the current byte being processed from the input string.
		; R4 -> Stores the pointer to the input string to traverse it.
		; R5 -> Stores the computed checksum value.

		PUSH {R4-R6, LR}     ; Save registers R4-R6 and LR (Link Register) to the stack
		
		MOV R4, R0           ; R4 = str* -> Initialize R4 to point to the start of the string
		MOV R5, #0           ; R5 = checksum -> Initialized to 0
    
checksum_loop
		LDRB R6, [R4], #1    ; Load the next byte from the string into R6 and increment the pointer (R4) by 1
		CMP R6, #0           ; Check if we reached the end of the string (NULL terminator)
        BEQ checksum_done    ; If we reached the end, exit the loop and return the checksum 
		EOR R5, R5, R6       ; R5 = checksum XOR current byte -> Compute the checksum by XORing the current byte with the accumulated checksum value
		B checksum_loop      ; Repeat for the next character
    
checksum_done

        LDR R1, =checksum_result    ; Load address of checksum_result variable
        STR R5, [R1]                ; Store the computed checksum in checksum_result variable

		MOV R0, R5           ; Store the computed checksum in R0 (return value)
		POP {R4-R6, PC}      ; Restore registers R4, R5 and return to caller
        ; Alternatively, we could use BX LR to return, since we didn't call any function that would modify the LR register.

			
; ______________________________  Data Section  ______________________________

        AREA CHECKSUM_DATA, DATA, READWRITE  ; Define a read-write data section
        ALIGN 4                          ; Ensure 4-byte alignment for the data
checksum_result DCD 0                	 ; Allocate a 4-byte word initialized to zero
    
        END
