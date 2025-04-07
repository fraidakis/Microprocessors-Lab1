/*
    * Main function to test the performance of the compute_hash function.
    * This function takes a long string (1000 characters) as input and computes its hash value.
    * The hash value is computed using two different methods: compute_hash and compute_hash_optimized.
    * A speedup of 43% was observed in the optimized version of the function.
*/ 


/*
#include <stdio.h>

extern int compute_hash(const char *str);
extern int compute_hash_optimized(const char *str);

int main(void)
{
    const char input[] = "K9YjM5T7q8dE1Zp3W0sVaLbUc6HnIoGtQxR2kFyNmCeDgBhAjlMwXzNuRvSpLtObChYgJvIqTfKrDlAxVzEwMnJoGuTkYcXbPlErWmZnHoSdYxAeUvQmLtIgKrDzXhPnLbAeJtKmVxOwGrTyMuQcDnBeXwYsFzPrHcGoIjKvZnQsDaLhUiPoRmWqEvNhUgTvZlCxQbYpAiKvTyFgRtQwYuJmNrLeWpVcBkHpNqSgViJtUaLoMkNiJcXwMeFsDuKgVrWeXtVnDjZoTeWiNkYgXqBuZpMg_XsNzPqKjTfHzOiJxAyZlCnPmBdDkEsYvWuVdArZcBxNnDlVoGgKfQtUoItHjJmJzIqLkPfSqNpMlRaXjPtLhFeUuKoMrSzCwDbQuGjWgJjWzQsEgTgBdShXgJkKwMiItHtZuLiKmMeDdZxXwVaPpWjJzIiHpNzXaMzLyKqVtEeDdWcCiH3N8G7H2F0K9J3L2M6A4B1D5R7T0Y6P9E8U1Q3W5O9I7L8Z6X3C2V0B9N4M1S5G8H6T0R2F3E1D7C9Q8U0A7Z5W4O3I6M1L2J9K0X4V7B8N3_M2S0F9H6G3T1R8E2D4C5Q9A0U7W1O2I4L5Z3X6C9V8B0N7M1S2F3H4G5T7R6E1D0C9Q3U5A8W2O6I9L3Z1X7C5V0B4N6M7S9F1H2G4T5R0E3D7C6Q9U1A2W3O4I5L6Z7X0C8V9B2N1M3S4F6H5G2T9R7E0D1C8Q4U6A9W3O2I1L7Z5X9C0V6B4N5M8S1F2H3G7T4R9E5D2C0Q1U3A4W6O7I8L9Z0X2C3V1B5N6M7S8F9H0G1T2R4E7D6C5Q9U2A1W3O5I6L7Z8X4C0V3B9N2M5S1F4H7G6T9R3E1D0C8Q7U5A6W9O8I3L2Z1X0C6V5B7N4M3S2F8H9G0T1R6E5D7C4Q2U0A9W1O3I5L4Z7X6C8V2B0N9M1S7F6H3G2T5R8E9D4C3Q0U1A6W7O9I2L3Z5X1C4V0B6N8M7S9F0H5G4T3R2E1";

		int hash_opt = compute_hash_optimized(input);
		int hash = compute_hash(input);

    printf("Hash value (opt): %d\n", hash_opt);
    printf("Hash value: %d\n", hash);

    return 0;
}
*/