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
// result in edx (intialized to -1)
int switch3(int * p1, int * p2, mode_t action) {
    int result = -1;

    switch ( action ) {
        case MODE_A:
            result = *p1;
            *p1 = *p2;
            break;
        case MODE_B:
            result = *p2;
            *p2 = *p1;
            break;
        case MODE_C:
            *p2 = 15;
            result = *p1;
            break;
        case MODE_D:
            *p2 = *p1;
            result = 17;
            break;
        case MODE_E:
            result = 17;
            break;
        default:
            break;
    }

    return result;
}

int main() {
    int * p1;
    int * p2;
    mode_t action = MODE_A;
    switch3(p1, p2, action);
}