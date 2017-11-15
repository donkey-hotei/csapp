/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.

NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
    /*
     * By De Morgan's laws, we have:
     * P AND Q = NOT ((NOT P) OR (NOT Q))
     */
    return ~(~x|~y);
}
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    /*
     * Finds the nth byte from the left by
     * shifting x to the right 8*n places
     * (since there are 8 bits in a byte).
     * Retrurn the LSB by masking out all but
     * the LSB.
     */
    return (x >> (n << 3)) & 0xff;
}
/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int logicalShift(int x, int n) {
    /*
     * First assign s to be the mask used to
     * mark the n 1's shifted in (e.g: for n = 2,
     * we have 11011111) before XOR'ing it with
     * the result of the arithmetic shift to toggle
     * those leading bits to 0.
     * Finally, AND this result with the original
     * shift to find all those bits in common.
     */
    int s = ((1 << 31) >> n) << 1;
    return ((x >> n) ^ s) & (x >> n);
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    /*
     * First count on-bits in every 2 bits,
     * then in every 4 bits, .., up to 16-bits
     * finally merging them all to the far right.
     */
    int a = ((0x55 << 24) | (0x55 << 16) | (0x55 << 8) | 0x55);
    int b = ((0x33 << 24) | (0x33 << 16) | (0x33 << 8) | 0x33);
    int c = ((0x0f << 24) | (0x0f << 16) | (0x0f << 8) | 0x0f);
    int d = ((0x00 << 24) | (0xff << 16) | (0x00 << 8) | 0xff);
    int e = ((0x00 << 24) | (0x00 << 16) | (0xff << 8) | 0xff);
    x = (x & a) + ((x >> 1)  & a);
    x = (x & b) + ((x >> 2)  & b);
    x = (x & c) + ((x >> 4)  & c);
    x = (x & d) + ((x >> 8)  & d);
    x = (x & e) + ((x >> 16) & e);
    return x;
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
    /*
     * Propgate any 1's to the left by shifting
     * in successive halves of a word and OR'ing
     * against it's unshifted self). This is to
     * be sure that any and all bits eventually
     * become the least significant bit. Finally
     * we flip that bit AND'ing with 1 s.t. the
     * result is zero iff there were no on bits.
     */
    x = (x >> 16) | x;
    x = (x >> 8)  | x;
    x = (x >> 4)  | x;
    x = (x >> 2)  | x;
    x = (x >> 1)  | x;
    return ~x & 1;
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  /* The smallest 32-bit number has only the sign bit
   * turned on.
   */
  return 1 << 31;
}
/*
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
    /* Idea is that a number x truncated to
     * n-bits should be the same as it's
     * untruncated self if it can fit into n-bits.
     */
    int s = 32 + (~n + 1);  // shift 32 - n
    int t = (x << s) >> s;  // sign-extension will change x
    return !(x ^ t); // x equal to truncated x?
}
/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    /* When x is positive, x / 2^n == x >> n but
     * when x is negative, x / 2^n == (x + 1 << n - 1) >> n.
     *
     * The reason for this is that shifting right by n will
     * round towards infinity but adding a "bias" by adding
     * one when the quotient is negative and odd that will
     * round the remainder towards zero.
     */
    int s = (x >> 31) & (1 << n) + ~0;
    return  (x + s) >> n;
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    /* By flipping all the bits and adding 1 we can
     * obtain either the negative version of a positive
     * number (i.e: 11111111 is -1, and 1 is 00000001)
     * and vice versa. The case of zero is taken care of
     * because the leftmost significant bit is carried out.
     */
    return ~x + 1;
}
/*
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
    /* Check if the sign bit is on or if
     * x is zero, if so then return 1 else
     * return 0.
     */
    return !(x >> 31 | !x);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    /* First determine if both x and y have the same sign,
     * if x is the negative of the two in the case that they
     * have differing signs, and determine if x is greater
     * than y. Use these three facts to determine if x <= y.
     */
    int s = !(x >> 31) ^ !(y >> 31);  // 0 if same sign, 1 otherwise
    int t = (s & (x >> 31));   // 1 if x is the negative of two, 0 otherwise
    int u = (y + (~x + 1));    // negative if x > y, postive if x < y, 0 if x = y
    int v = (!s & !(u >> 31)); // if y - x > 0 and have same sign then 1 else 0
    return t | v;  // 1 if x is the negative of the two or y - x > 0
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    return 2;
}
/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    return 2;
}
/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    return 2;
}
/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    return 2;
}
