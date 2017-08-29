/*
 * A chance at reverse engineering a switch statement from machine code.
 */


/*
 * (gdb) x/6w 0x80485d0 <- location of jump table
 * 0x80485d0: 0x8048438 0x8048448 0x8048438 0x804843d
 * 0x80485e0: 0x8048442 0x8048445
 */
int switch_prob(int x, int n) {
    int result = x;

    switch ( n ) {
        case 0:
            result <<= 2;
            break;
        case 2:
            result += 10;
            break;
        case 3:
            result <<= 2;
            break;
        case 4:
            result >>= 2;
            break;
        case 5:
            result *= result;
            break;
        default:
            break;

    }

    return result;
}