;  ____________________________________________________________________________
; |																			   |
; |                      Answers to questions a) - e)						   |
; |____________________________________________________________________________|


		AREA HASH_FUNC, CODE, READONLY  ; Define a read-only code section named HASH_FUNC
		EXPORT compute_hash             ; Make compute_hash visible visible to the linker so other files can call it


compute_hash                        ; Label marking the function entry point

        PUSH    {R4-R8, LR}         ; Save registers R4-R8 and LR (Link Register) to the stack, preserving their values for function return

        ; R0    -> Input string pointer (str*) - passed as argument to the function
        ; R4    -> String pointer (str*) - used to traverse the string
        ; R5    -> Hash value (int) - initialized to 0, used to store the computed hash value
        ; R6    -> Current character (char) - used to store the current character being processed from the string
        ; R7    -> Temp variable (int) - used for intermediate calculations, such as converting characters to integers or performing arithmetic operations
        ; R8    -> Lookup table pointer (int*) - used to point to the lookup table for digit values
		



; ______________________________  Question a) - String Length  ______________________________

	MOV     R4, R0              ; R4 = str* -> Initialize R4 to point to the start of the string
		
strlen_loop					; Loop to calculate string length
        LDRB    R6, [R4]  			; R6 = current char
		ADD 	R4, R4, #1			; Increment pointer
        CMP     R6, #0                          ; Compare R6 with NULL (0) -> Check if we reached the end of the string
        BNE     strlen_loop			; if(R6 != '\0') -> If R6 isn't NULL continue to next char

        SUB     R7, R4, R0          ; R7 = End - Start -> Calculate the length of the string by subtracting the start address from the end address
        SUB     R7, R7, #1          ; Adjust the length to account for the null terminator
; _____________________________________________________________________________________________		


 
; ______________________________  Loop through the string  ______________________________

        MOV     R4, R0              ; R4 = str* -> Reset R4 to the start of the string
		MOV	R5, R7  	    ; Initialize hash value with length of input string
loop_start
        LDRB    R6, [R4]  			; R6 = current char
		ADD 	R4, R4, #1			; Increment pointer
        CMP     R6, #0                          ; Check if we reached the end of the string (NULL terminator)
        BEQ     end_loop			; If NULL, exit loop and return hash value
                               
                                    ; Else process the character: 



; _______________________________  Question d) - Digits  _______________________________

check_digit
        CMP     R6, #'0'            ; Compare R6 with ASCII value of '0' = #48 -> If instead of #'0' we write #0 then we are comparing to character with ASCII code 0 -> NULL ! 
        BLT     loop_start	    ; if(char < '0') -> Skip character 
		CMP     R6, #'9'             
        BGT     check_uppercase		; if(char > '9') -> Move to next check (Uppercase + lowercase)
		
        SUB     R7, R6, #'0'        ; R7 = atoi(R6) -> Convert ASCII to integer by subtracting ASCII value of '0' (48)
        LDR     R8, =digit_table    ; R8 = address of digit_table -> Load the address of the lookup table into R8
        LDRB    R7, [R8, R7]        ; R7 = digit_table[R7] -> Load the value from the lookup table using R7 as an index -> (R8 + R7*1byte) , because of DCB
        ADD     R5, R5, R7          ; hash += value -> Add the value from the lookup table to the hash value in R5
        B       loop_start          ; Continue to the next character




; _______________________________  Question b) - Uppercase  _______________________________

check_uppercase
        CMP     R6, #'A'            ; Check if uppercase (A-Z)
        BLT     loop_start
        CMP     R6, #'Z'
		BGT	check_lowercase			; Get rid of branch instruction by using conditional execution -> instead of using BLT + ADD + B
        
        ADD		R6, R6, R6			; R6 = 2*R6
		ADD   	R5, R5, R6   		; R5 += 2*R6
        B     	loop_start




; _______________________________  Question c) - Lowercase  _______________________________

check_lowercase
        CMP     R6, #'a'            ; Check if lowercase (a-z)
        BLT     loop_start
        CMP     R6, #'z'
        BGT		loop_start

        SUB     R7, R6, #'a'        ; Subtract ASCII value of 'a' (97)   
		MLA		R5, R7, R7, R5		; hash = (R7*R7) + R5 -> Multiply and accumulate instead of MUL + ADD 
		B       loop_start



; _______________________________  Return Hash Value  _______________________________


end_loop                            
        MOV     R0, R5              ; R0 = hash value -> Move the computed hash value to return register R0
        POP     {R4-R8, PC}         ; Restore registers and return from the function using the Link Register (LR) stored in the stack
									


; ______________________________  Data Section  ______________________________

        ALIGN 4                                  ; Ensure data are aligned for faster access
digit_table DCB 5, 12, 7, 6, 4, 11, 6, 3, 10, 23 ; DCB (Define Constant Byte) is used to define a !byte! array in memory

		END
