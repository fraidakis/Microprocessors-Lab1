;  ____________________________________________________________________________
; |																			   |
; |                      Answers to questions f) - g)						   |
; |____________________________________________________________________________|


        AREA SumMod7, CODE, READONLY ; Define a read-only code section named SumMod7
        EXPORT sum_and_mod7          ; Make sum_and_mod7 visible to the linker so other files can call it   
        EXPORT reduction_result              ; Export the reduced hash result variable so it can be accessed by other modules
			
	

sum_and_mod7

        PUSH {R4-R8, LR}    ; Save registers R4-R5 and LR (Link Register) to the stack
    
        ; R0    -> Input hash value (int) - passed as argument to the function
        ; R4    -> current hash value (int) - used to store the current value of the hash during digit extraction
        ; R5    -> divisor (int) - used to store the divisor for division operations
        ; R6    -> quotient (int) - used to store the result of division operations
        ; R7    -> remainder (int) - used to store the remainder of division operations
        ; R8    -> sum of digits (int) - used to store the sum of digits of the hash value
    



; ______________________________  Question f) - Sum of digits  _______________________________

        MOV R4, R0          ; R4 = hash value (input) 
        MOV R8, #0          ; R8 = sum of digits (initially 0)
        MOV R5, #10         ; R5 = divisor (10)

sum_digits
        UDIV R6, R4, R5     ; R6 = quotient = R4 / 10 (UDIV since we are working with positive numbers)

    	MLS R7, R5, R6, R4  ; R7 = remainder = R4 - (R5 * R6) -> Get rid of a separate SUB instruction by using MLS
	    ;MUL R7, R5, R6
	    ;SUB R7, R4, R7
    
	    ADD R8, R8, R7      ; R8 += digit -> Add current extracted digit to the sum
        
		MOVS R4, R6          ; Update R4 with the quotient for the next iteration 
							; and set flags to check if zero. Same as doing : CMP
;       CMP R4, #0          ; Check if we have processed all digits     
        
		BNE sum_digits
        MOV R4, R8          ; Update hash with sum




; ______________________________  Question g) - Modulo 7  _______________________________

        MOV R5, #7          ; R5 = divisor (7)
        UDIV R6, R4, R5     ; R6 = quotient = R4 / 7 (UDIV since we are working with positive numbers)
        MLS R7, R5, R6, R4  ; R7 = remainder = R4 - (R5 * R6) 
    


; ______________________________  Store & Return Hash Value  ______________________________
        
		LDR     R4, =reduction_result    ; Load address of reduction_result variable
        STR     R7, [R4]        	     ; Store hash value at that address

        MOV R0, R7          ; R0 = reduced value (sum + modulo) -> Move the reduced value to return register R0
        POP {R4-R8, PC}     ; Restore registers and return from the function using the Link Register (LR) stored in the stack



; ______________________________  Data Section  ______________________________

        AREA REDUCTION_DATA, DATA, READWRITE  ; Define a read-write data section
        ALIGN 4                          ; Ensure 4-byte alignment for the data
reduction_result DCD 0                   ; Allocate a 4-byte word initialized to zero

		END
