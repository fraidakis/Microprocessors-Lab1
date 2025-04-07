		
		AREA HASH_FUNC, CODE, READONLY  ; Define a read-only code section named HASH_FUNC
		EXPORT compute_hash1           ; Makes compute_hash visible visible to the linker so other files can call it


compute_hash1	                  	  ; Label marking the function entry point


        MOV     R1, R0              ; R1 = str -> Store input string 		
strlen_loop
        LDRB    R2, [R1], #1        ; Load byte and increment -> instead of ADDS R1, R1, #1
        CMP     R2, #0              ; Check for null terminator = end of string = \0
        BNE     strlen_loop
        SUB     R1, R1, R0          ; Length = end - start
        SUB     R1, R1, #1          ; Adjust for last increment


loop_start
        LDRB    R2, [R0], #1        ; Load next character and advance pointer
        CMP     R2, #0              ; Check if null terminator
        BEQ     end_loop

check_digit
        CMP     R2, #'0'            ; Check if digit (0-9) -> If instead of #'0' we had #0 then we are comparing with character with ASCII code 0 = NULL !!! 
        BLT     loop_start
		CMP     R2, #'9'            ; Check if digit (0-9) -> And here comparing with TAB... 
        BGT     check_uppercase
        SUB     R3, R2, #'0'        ; R3 = digit (0-9)
        LDR     R2, =digit_table    ; Load address of lookup table
        LDRB    R3, [R2, R3]        ; Load corresponding value
        ADD     R1, R1, R3          ; hash += value
        B       loop_start

check_uppercase
        CMP     R2, #'A'              ; Check if uppercase (A-Z) -> #'A' : Assembler tranlates this into its ASCII value -> #66
        BLT     loop_start
        CMP     R2, #'Z'
        ADDLE     R1, R1, R2, LSL #1   ; hash += (ASCII * 2)
        BLE       loop_start

check_lowercase
        CMP     R2, #'a'            ; Check if lowercase (a-z)
        BLT     loop_start
		CMP     R2, #'z'            ; Check if lowercase (a-z)
        BGT     loop_start
        SUB     R3, R2, #'a'         ; R3 = ASCII - 97
   		MLA		R1, R3, R3, R1		 ; hash = (R3*R3) + R1 
		B       loop_start

end_loop
        MOV     R0, R1              ; Move hash to return register
        BX		LR


        ; Lookup table for digit values
ALIGN
digit_table
    DCB  5, 12, 7, 6, 4, 11, 6, 3, 10, 23

        END
