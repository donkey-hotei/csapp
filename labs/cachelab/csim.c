#define _GNU_SOURCE
#include<ctype.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<strings.h>
#include<getopt.h>
#include<math.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "cachelab.h"

/*
 * cachelab
 * a set associative cache simulator which uses a LRU replacement policy
 */

typedef unsigned long long int mem_addr_t;

/* A cache line.
 */
typedef struct {
    int is_valid;   // indicates if line contains meaningful data
    mem_addr_t tag;
    char * block;
    int num_accesses;
} cache_line_t;

/* Set of lines within the cache.
 */
typedef struct {
    cache_line_t * lines;
    int num_lines;
} cache_set_t;

/* A cache.
 */
typedef struct {
    cache_set_t * sets;

    int hits;
    int misses;
    int evictions;

    int block_size; // cacheline block size (in bytes), B = 2**b
    int num_sets;   // number of sets, S = 2**s
    int num_lines;  // number of lines per set
    int size;       // C = S * B * E
} cache_t;

/* Parameters of the cache that characterize it's organization.
 */
typedef struct {
    int s; // number of set index bits
    int b; // number of block bits
    int E; // associativity number (number of lines per set)
} cache_params_t;


/* Globals */
cache_t cache;
cache_params_t params;


void printUsage(char ** argv)  {
    printf("usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
}

/*
 * Initialize the cache given some cache parameters.
 */
void initCache(cache_params_t params) {
    cache_set_t cache_set;
    cache_line_t cache_line;

    int B, S, E, i, j;

    bzero(&cache, sizeof(cache_t));

    // set derived quantities
    B = (1 << params.b);  // 2**b
    S = (1 << params.s);  // 2**s
    E = params.E;

    cache.block_size = B;
    cache.num_sets   = S;
    cache.num_lines  = E;
    cache.size       = B * S * E;

    cache.sets = (cache_set_t *) malloc(S * sizeof(cache_set_t));

    if (cache.sets == NULL) {
        fprintf(stderr, "Unable to allocate %d cache sets!", S);
        exit(1);
    }

    for (i = 0; i < cache.num_sets; i++) {
        cache_set.num_lines = E;
        cache_set.lines = (cache_line_t *) malloc(E * sizeof(cache_line_t));
        cache.sets[i] = cache_set;

        if (cache_set.lines == NULL) {
            fprintf(stderr, "Unable to allocate cache lines for set %d", i);
            exit(1);
        }

        for (j = 0; j < E; j++) {
            cache_line.is_valid     = 0;
            cache_line.tag          = 0;
            cache_line.num_accesses = 0;
            cache_set.lines[j] = cache_line;
        }
    }

    return;
}

/*
 * Frees cache from memory by first freeing the lines of
 * each set, and then the cache set itself.
 */
void freeCache( ) {
    int i;

    for (i = 0; i < cache.num_sets; i++)
        free(cache.sets[i].lines);

    free(cache.sets);

    return;
}

int getLRUCacheLineIndex(cache_set_t cache_set) {
    int i, min_accesses, cache_line_index;

    min_accesses = cache_set.lines[0].num_accesses;

    for (i = 0; i < cache_set.num_lines; i++) {
        if (min_accesses > cache_set.lines[i].num_accesses) {
            min_accesses = cache_set.lines[i].num_accesses;
            cache_line_index = i;
        }
    }

    cache.evictions++;

    return cache_line_index;
}

int getEmptyCacheLineIndex(cache_set_t cache_set) {
    int i;

    for (i = 0; i < cache_set.num_lines; i++) {
        if (cache_set.lines[i].is_valid)
            continue;
        else
            return i;
    }
    return i;
}

/*
 *
 */
void accessData(mem_addr_t address) {
    cache_set_t cache_set;
    int line_index, cache_set_is_full, prev_hits, tag_size, i;
    unsigned long long tag, set_index;

    // assume cache set is full
    cache_set_is_full = 1;
    prev_hits = cache.hits;

    tag_size  = (64 - (params.b + params.s));
    set_index = (address << tag_size) >> (tag_size + (params.b));
    tag = address >> (params.b + params.s);

    cache_set = cache.sets[set_index];

    for (i = 0; i < cache.num_lines; i++) {
        if (cache_set.lines[i].is_valid) {
            if (cache_set.lines[i].tag == tag) {
                cache_set.lines[i].num_accesses++;
                cache.hits++;
            }
        } else if (cache_set_is_full)
            cache_set_is_full = 0;  // cache isn't full
    }

    if (cache.hits == prev_hits)
        cache.misses++;
    else
        return;

    if (cache_set_is_full) {
        // evict a line using LRU replacement policy
        line_index = getLRUCacheLineIndex(cache_set);
        cache_set.lines[line_index].tag = tag;
        cache_set.lines[line_index].num_accesses++;
    } else {
        // use first empty line
        line_index = getEmptyCacheLineIndex(cache_set);
        cache_set.lines[line_index].is_valid = 1;
        cache_set.lines[line_index].tag = tag;
        cache_set.lines[line_index].num_accesses++;
    }
    return;
}


/*
 * Replay trace file on a simulated cache.
 */
void replayTrace(char * trace_file_path) {
    FILE * trace_file_p;
    char * trace_line = NULL;
    size_t len;
    ssize_t read;

    char access_cmd;
    mem_addr_t address;
    int size;

    if ((trace_file_p = fopen(trace_file_path, "r")) == NULL) {
        fprintf(stderr, "Error opening file %s", trace_file_path);
        exit(1);
    }

    while ((read = getline(&trace_line, &len, trace_file_p)) != -1) {
        sscanf(trace_line, " %c %llx,%d", &access_cmd, &address, &size);

        switch(access_cmd) {
            case 'M':
                accessData(address);
                accessData(address);
                break;
            case 'L':
                accessData(address);
                break;
            case 'S':
                accessData(address);
                break;
        }
    }
    return;
}

int main(int argc, char ** argv)
{
    int option;
    // int verbosity_level;
    char * trace_file_path;

    bzero(&params, sizeof(params));

    while ((option = getopt (argc, argv, "s:E:b:t:hv")) != -1) {
        switch (option) {
            case 's':
                params.s = atoi(optarg);
                break;
            case 'E':
                params.E = atoi(optarg);
                break;
            case 'b':
                params.b = atoi(optarg);
                break;
            case 't':
                trace_file_path = optarg;
                break;
            case 'v':
                // verbosity_level = 1;
                break;
            case 'h':
                printUsage(argv);
                exit(0);
            default:
                abort();
        }
    }

    if (params.s == 0 || params.E == 0 || params.b == 0 || trace_file_path == NULL) {
        puts("All arguments are necessary.");
        printUsage(argv);
        exit(1);
    }

    initCache(params);
    replayTrace(trace_file_path);
    printSummary(cache.hits, cache.misses, cache.evictions);

    freeCache();

    return 0;
}
