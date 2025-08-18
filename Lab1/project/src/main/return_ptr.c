/*

#include <stdio.h>

extern int* fibonacci_pointer(int n);

int main() {
    int n = 10;
    
    // Call assembly function
    int* result_ptr = fibonacci_pointer(n);
    
    // Print both the address and the value 
    printf("Address: %p, Value: %d\n", result_ptr, *result_ptr);
    printf("Fibonacci(%d) = %d\n", n, *result_ptr);
    
    return 0;
}

*/