
#include <stdio.h>

// External assembly functions
extern int compute_hash(const char *str);

extern int sum_and_mod7(int hash);
extern int reduction_result;

extern int fibonacci(int n);

extern int compute_checksum(const char *str);

int main(void)
{
    const char input[] = "A9b3";

    int hash = compute_hash(input);
    int reduced = sum_and_mod7(hash);
    int fib_result = fibonacci(reduced);
    int checksum = compute_checksum(input);

    printf("Input: %s\n", input);

    printf("Hash: %d\n", hash);
	  // Access the reduced result of Q2 in two ways: through the return value and the global variable declared in assembly
    printf("Reduced value (return): %d  &&  (global): %d\n", reduced, reduction_result);
    printf("Fibonacci: %d\nChecksum: %d\n", fib_result, checksum);

    return 0;
}
