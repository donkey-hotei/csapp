/*
 * Recall that enumerated types in C are simply a way to introduce a set of names
 * having associated integer values. By default, the integers assigned to the names
 * go from zero upward.
 *
 * Fill in the missing parts of the C code using the provided assembly.
 */

typedef enum { MODE_A, // 0
               MODE_B, // 1
               MODE_C, // 2
               MODE_D, // 3
               MODE_E  // 4
} mode_t;

// p1 @ ebp + 8, p2 @ ebp + 12, action @ ebp + 16
int switch3(int * p1, int * p2, mode_t action) {
    int result = 0;

    switch ( action ) {
        case MODE_A:
            break;
        case MODE_B:
            break;
        case MODE_C:
            break;
        case MODE_D:
            break;
        case MODE_E:
            break;
        default:
            break;
    }

    return result;
}