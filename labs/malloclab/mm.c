/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * This implementation uses an implicit list is used with boundary tag coalescing.
 * In order to implement boundary tags 
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "gnu-e-ducks",
    /* First member's full name */
    "Isaak Yansane-Sisk",
    /* First member's email address */
    "yandil03@evergreen.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT & mask out lower three bits */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE     4         /* Word and header/footer size (bytes) */
#define DSIZE     8         /* Double word size (bytes) */
#define CHUNKSIZE (1 << 2)  /* Extend heap by this amount (bytes) */
#define OVERHEAD  16        /* Header and footer require 8 bytes each */

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write word at address p */
#define GET(p)      (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given a block ptr bp, compute address of it's header and footer */
#define HDRP(bp)     ((char *)(bp) - WSIZE)
#define FTRP(bp)     ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given a block ptr bp, compute the address of the next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Heap list pointer */
static char * heap_listp;


/*
 * coalesce - Takes two contiguous free blocks and merges them into
 *            one free block by overwrting the size bits of the previous
 *            blocks header with the combined sizes of the two free blocks
 *            and doing the same for the footer of the current block.
 */
void * coalesce(void * prev_block, void * curr_block)
{
    size_t size;

    size = GET_SIZE(prev_block) + GET_SIZE(curr_block);
    size = (size % 2) ? (size + 1) & WSIZE : size * WSIZE;

    PUT(HDRP(prev_block), PACK(size, 0));
    PUT(FTRP(curr_block), PACK(size, 0));

    PUT(FTRP(prev_block), 0);
    PUT(HDRP(curr_block), 0);

    return prev_block;
}

/*
 * extend_heap - Extends the heap with a free block and returns it's
 *               block pointer. Maintains alignment by allocating only
 *               an even number of words to extend the heap by.
 *
 *               Overwrites epilogue of the previously allocated heap
 *               space with the header of the first new block.
 *               After this new space is added to the heap, an epilogue
 *               header is readded in the beginning of the newly
 *               acquired heap space.
 */
void * extend_heap(size_t words) 
{
    char * bp;    // base pointer
    size_t size;  // size 

    /* allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) & WSIZE : words * WSIZE;

    /* 16 bytes needed for next and prev pointers in free block */
    if (size < OVERHEAD) size = OVERHEAD;
    if ((long)(bp = mem_sbrk(size)) == -1) return NULL;

    /* initialize free block header/footer and epilogue header */
    PUT(HDRP(bp), PACK(size, 0));          // free block header
    PUT(FTRP(bp), PACK(size, 0));          // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));  // new epilogue header

    if (GET_ALLOC(PREV_BLKP(bp)) == 0)
        bp = coalesce(PREV_BLKP(bp), bp);

    return bp;
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(DSIZE * 4)) == NULL)
        return -1;

    PUT(heap_listp, 0);                           // padding (for alignment)
    PUT(heap_listp + WSIZE, PACK(1, 1));          // prologue header
    PUT(heap_listp + DSIZE, PACK(1, 1));          // prologue footer
    PUT(heap_listp + DSIZE + WSIZE, PACK(1, 0));  // epilogue header

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
        return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;

    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;

    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);

    return newptr;
}

/*
 * mm_checkheap - 
 */
