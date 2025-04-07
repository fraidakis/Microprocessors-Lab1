        AREA HASH_FUNC_OPT, CODE, READONLY
        EXPORT compute_hash_optimized


compute_hash_optimized
        PUSH    {R4-R8, LR}        

        ; R0 = input string pointer
        ; R4 = string pointer for traversal
        ; R5 = hash value (processing only)
        ; R6 = current character
        ; R7 = length count	er / temp variable
        ; R8 = lookup table pointer

        MOV     R4, R0              ; Initialize R4 to start of string
        MOV     R5, #0              ; Initialize hash to 0 (we'll add length later)
        MOV     R7, #0              ; Initialize length counter
        
        
        LDR     R8, =digit_table    ; !Initialize once outside the loop


; _______________________________  Question a) - String Length  ______________________________

;! Single-pass loop - count length while processing characters
loop_start
        LDRB    R6, [R4], #1        ;! R6 = current char -> Load byte and increment -> instead of LDR + ADD  
        CMP     R6, #0             
        BEQ     finalize_hash      

        ADD     R7, R7, #1         



; _______________________________  Question b) - Digits  _______________________________

check_digit
        CMP     R6, #'0'            
        BLT     loop_start         
        CMP     R6, #'9'
        BGT     check_uppercase     

        SUB     R6, R6, #'0'       
        LDRB    R6, [R8, R6]        
        ADD     R5, R5, R6
        B       loop_start



; _______________________________  Question c) - Uppercase  _______________________________

check_uppercase
        CMP     R6, #'A'
        BLT     loop_start         
        CMP     R6, #'Z'
        ;! Get rid of branch instruction by using conditional execution -> instead of using BLT check_lowercase
        ADDLE   R5, R5, R6, LSL #1 ;! R5 += 2 * R6 -> Get rid of a separate shift instruction by using barrel shifter
        BLE     loop_start



; _______________________________  Question d) - Lowercase  _______________________________

        SUB     R6, R6, #'a'	
        CMP     R6, #25         ; if(R6 > 25) -> Skip character 
        ; Unsigned comparison to handle negative values correctly  
        ; Example: if R6 = '^' (ASCII 94), then R6 - 'a' = 94 - 97 = -3 -> -3 < 25 so it would be processed
        ; So signed comparison would wrongly process it as a valid character...
        BHI     loop_start      ;! Only 1 (comparison & branch) instruction instead of 2  
        MLA     R5, R6, R6, R5  ;! hash = (R6*R6) + R5 -> Multiply and accumulate instead of MUL + ADD 
        B       loop_start



; _______________________________  Finalize hash  _______________________________

finalize_hash
        ADD     R5, R5, R7      ; Add length to hash value  
        
        MOV     R0, R5           
        POP     {R4-R8, PC}       

        ALIGN 4
digit_table DCB 5, 12, 7, 6, 4, 11, 6, 3, 10, 23

        END