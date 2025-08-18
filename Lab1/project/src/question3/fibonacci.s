;  ____________________________________________________________________________
; |																			   |
; |                            Answer to question h)                           |
; |____________________________________________________________________________|


        AREA Fibonacci, CODE, READONLY    ; Define a read-only code section named Fibonacci
        EXPORT fibonacci                  ; Make fibonacci function visible to the linker so other files can call it


fibonacci

        ; R0    -> Input number (int) - passed as argument to the function
        ; R1    -> Temporary variable (int) - used to store intermediate results



; __________________  Base cases: F(0) = 0, F(1) = 1  __________________

		; Base case: F(0) = 0
        CMP R0, #0                   
        BXEQ LR           ; If R0 == 0, return 0 (R0 is already 0) 

        ; Base case: F(1) = 1
        CMP R0, #1
        BXEQ LR           ; If R0 == 1, return 1 (R0 is already 1)
        
        ; Take advantage of BX conditional execution to avoid extra branches & labels



; __________________  Recursive case: F(n) = F(n-1) + F(n-2)  __________________

        ; Save current `n` and return address (LR)
		PUSH {LR}   
		PUSH {R0}	
		; Same as: PUSH {R0, LR} -> Separate them to make it clear that LR is in the bottom of the stack and R0 on top

        ; Compute F(n-1)
        SUB R0, R0, #1       ; R0 = n - 1 -> Prepare argument for the recursive call 		  
        BL fibonacci         ; Recursive call: F(n-1)
        ; R0 now contains F(n-1)

        POP {R1}             ; R1 = n -> Restore current `n` 
        PUSH {R0}            ; Save F(n-1)


        ; Compute F(n-2)
        SUB R0, R1, #2       ; R0 = n - 2 -> Prepare argument for the recursive call
        BL fibonacci         ; Recursive call: F(n-2)
        ; R0 now contains F(n-2)


        POP {R1}             ; R1 = F(n-1) -> Restore F(n-1)
        ADD R0, R0, R1       ; Compute F(n) = F(n-1) + F(n-2)
        ; R0 now contains F(n)            



; __________________  Return Fibonacci Result  __________________

        POP {PC}                    ; Restore the Link Register (LR) from the stack to return back to the caller
        

