/***************************************************************************
 * Dr. Evil's Insidious Bomb, Version 1.1
 * Copyright 2011, Dr. Evil Incorporated. All rights reserved.
 *
 * LICENSE:
 *
 * Dr. Evil Incorporated (the PERPETRATOR) hereby grants you (the
 * VICTIM) explicit permission to use this bomb (the BOMB).  This is a
 * time limited license, which expires on the death of the VICTIM.
 * The PERPETRATOR takes no responsibility for damage, frustration,
 * that is.  The VICTIM may not distribute this bomb source code to
 * any enemies of the PERPETRATOR.  No VICTIM may debug,
 * reverse-engineer, run "strings" on, decompile, decrypt, or use any
 * other technique to gain knowledge of and defuse the BOMB.  BOMB
 * proof clothing may not be worn when handling this program.  The
 * PERPETRATOR will not apologize for the PERPETRATOR's poor sense of
 * humor.  This license is null and void where the BOMB is prohibited
 * by law.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#include "phases.h"

/* Phases */

/*
 * A first reversal.
 */
void phase_1(char ** input){
    if (strings_not_equal(input, "Border relations with Canada have never been better."))
        explode_bomb();
}

/*
 * Illustrates control structures such as loops and conditionals,
 * as well as indexing of array structures.
 *
 * key: 1 2 4 8 16 32
 */
void phase_2(char ** input){
    int ar[6];
    ar = read_six_numbers(input);
    for (int i = 0; i <= 4; i++)
        if ((ar[i] + ar[i]) == ar[i+1])
            explode_bomb();
}

/*
 * Illustrates switch statements implemented w/jumptables.
 * jumptable is located in .rodata
 * vaddr=0x004022b0 sz=1253 vsz=1253 perm=--r-- name=.rodata
 *
 * key: 1 311, or 2 707, ..., 7 or 327
 */
void phase_3(char ** input){
    int a, b; // local_ch, local_8h
    if (sscanf(input, "%d %d", a, d, b) <= 1)
        explode_bomb();

    // jmp qword [rax*8 + 0x402470]
    // rax = 1 => 0x402470 -> 0x400fb9
    // rax = 2 => 0x402480 -> 0x400f83
    // rax = 3 => 0x402488 -> 0x400f8a
    // rax = 4 => 0x402490 -> 0x400f91
    // rax = 5 => 0x402672 -> 0x400f98
    // rax = 6 => 0x4024a0 -> 0x400f9f
    // rax = 7 => 0x4024a8 -> 0x400fa6
    switch (a) {
        case 1:
            val = 311;
        case 2:
            val = 707;
        case 3:
            val = 256;
        case 4:
            val = 389;
        case 5:
            val = 206;
        case 6:
            val = 682;
        case 7:
            val = 327;
        default:
            explode_bomb();
    }
    if (val != b)
        explode_bomb();
    return 0;
}

/* int func4(int n){

    if (n <= 1)
        return n;
    return func4(n-1) + func4(n-2);
}
*/


/*
 * func4(14, 0, 14);
 * -> (14 - 0) / 2 == 7 is less than 14
      func4(14, 0, 7 - 1);
      -> func

 * func(7, 0, 14);
 * 7 is not less than 14
 * -> func(7, 1, 14);
 *   -> 1 + (13 / 2) == 7 is not less than 14
 *     -> 
*/

int func4(int a, int x, int y) {
    int result = (x + (y - x) / 2);

    if (result > a)
        return 2 * func4(a, x, result - 1);
    else if (result < a)
        return 2 * func4(a, result + 1, y) + 1;
    else 
        return 0;
}


/*
 * Illustrates procedures and call stacks.
 *
 * key: c
 */
void phase_4(char ** input){
    int a, b;
    if(sscanf("%d %d", a, b) != 2)
        explode_bomb();
    if (a > 14)
        explode_bomb();
    if ((func4(a, 0, 14) != 0) && b != 0)
        explode_bomb();
}

void phase_5(char ** input){
    char ** string = "isrveawhobpnutfg";
    if (string_length(input) != 6)
        explode_bomb();
    for (int i = 0; i <= 5; i++){
        char c = input[0];
    }
    if (strings_not_equal(input, "giants"))
        explode_bomb();
}

void phase_6(char ** input){
    read_six_numbers();

}


/* 
 * Note to self: Remember to erase this file so my victims will have no
 * idea what is going on, and so they will all blow up in a
 * spectaculary fiendish explosion. -- Dr. Evil 
 */

FILE *infile;

int main(int argc, char *argv[])
{
    char *input;

    /* Note to self: remember to port this bomb to Windows and put a 
     * fantastic GUI on it. */

    /* When run with no arguments, the bomb reads its input lines 
     * from standard input. */
    if (argc == 1) {
    infile = stdin;
    }

    /* When run with one argument <file>, the bomb reads from <file> 
     * until EOF, and then switches to standard input. Thus, as you 
     * defuse each phase, you can add its defusing string to <file> and
     * avoid having to retype it. */
    else if (argc == 2) {
    if (!(infile = fopen(argv[1], "r"))) {
        printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
        exit(8);
    }
    }

    /* You can't call the bomb with more than 1 command line argument. */
    else {
    printf("Usage: %s [<input_file>]\n", argv[0]);
    exit(8);
    }

    /* Do all sorts of secret stuff that makes the bomb harder to defuse. */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    /* Hmm...  Six phases must be more secure than one phase! */
    input = read_line();             /* Get input                   */
    phase_1(input);                  /* Run the phase               */
    phase_defused();                 /* Drat!  They figured it out!
                      * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");

    /* The second phase is harder.  No one will ever figure out
     * how to defuse this... */
    input = read_line();
    phase_2(input);
    phase_defused();
    printf("That's number 2.  Keep going!\n");

    /* I guess this is too easy so far.  Some more complex code will
     * confuse people. */
    input = read_line();
    phase_3(input);
    phase_defused();
    printf("Halfway there!\n");

    /* Oh yeah?  Well, how good is your math?  Try on this saucy problem! */
    input = read_line();
    phase_4(input);
    phase_defused();
    printf("So you got that one.  Try this one.\n");

    /* Round and 'round in memory we go, where we stop, the bomb blows! */
    input = read_line();
    phase_5(input);
    phase_defused();
    printf("Good work!  On to the next...\n");

    /* This phase will never be used, since no one will get past the
     * earlier ones.  But just in case, make this one extra hard. */
    input = read_line();
    phase_6(input);
    phase_defused();

    /* Wow, they got it!  But isn't something... missing?  Perhaps
     * something they overlooked?  Mua ha ha ha ha! */
    return 0;
}
