        AREA FibonacciPTR, CODE, READONLY
        EXPORT fibonacci_pointer

fibonacci_pointer
        ; R0 -> Input number (int)
        
        ; Base cases
        CMP R0, #0
        BEQ base_case_0
        CMP R0, #1
        BEQ base_case_1
        
        ; Recursive case
        PUSH {R4, LR}         ; Save R4 and LR (R4 will store our result)
        MOV R4, R0            ; Save n
        
        SUB R0, R0, #1        ; n-1
        BL fibonacci_pointer        ; Calculate F(n-1)
        LDR R0, [R0]          ; Get the value from the address
        PUSH {R0}             ; Save F(n-1)
        
        SUB R0, R4, #2        ; n-2
        BL fibonacci_pointer        ; Calculate F(n-2)
        LDR R0, [R0]          ; Get the value from the address
        POP {R1}              ; Get F(n-1)
        ADD R0, R0, R1        ; F(n) = F(n-1) + F(n-2)
        
        LDR R1, =fibonacci_resultPTR
        STR R0, [R1]          ; Store result
        MOV R0, R1            ; Return address
        POP {R4, PC}          ; Restore and return
        
base_case_0
        MOV R0, #0
        LDR R1, =fibonacci_resultPTR
        STR R0, [R1]          ; Store 0
        MOV R0, R1            ; Return address
        BX LR
        
base_case_1
        MOV R0, #1
        LDR R1, =fibonacci_resultPTR
        STR R0, [R1]          ; Store 1
        MOV R0, R1            ; Return address
        BX LR

        AREA |.data|, DATA, READWRITE
        ALIGN 4
fibonacci_resultPTR DCD 0
        
        END