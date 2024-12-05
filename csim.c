#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef unsigned long long ull;
typedef struct CACHELINE {
    bool valid;
    ull tag;
    ull LRU;
} cacheLine;
cacheLine line[1024][8];

bool verbose = 0;
int hit = 0, miss = 0, eviction = 0;

void load(ull S_addr, ull E_addr, int E) {
    bool isHit = false;
    for(int i = 0; i < E; i++) {
        if (line[S_addr][i].valid && line[S_addr][i].tag == E_addr) {
            isHit = true;
            hit++;
            if (verbose) {
                printf(" hit");
            }
            for (int j = 0; j < E; ++j) {
                if (line[S_addr][j].LRU < line[S_addr][i].LRU) {
                    line[S_addr][j].LRU++;
                }
            }
            line[S_addr][i].LRU = 0;
            break;
        }
    }
    if(!isHit) {
        miss++;
        if(verbose) {
            printf(" miss");
        }
        bool isFull = true;
        ull maxTime = 0;
        int maxTimeIndex = 0;
        for(int i = 0; i < E; ++i) {
            if(!line[S_addr][i].valid) {
                isFull = false;
                line[S_addr][i].valid = true;
                line[S_addr][i].tag = E_addr;
                line[S_addr][i].LRU = 0;
                for(int j = 0; j < E; ++j) {
                    if(i == j) {
                        continue;
                    }
                    line[S_addr][j].LRU++;
                }
                break;
            }
            else {
                if(line[S_addr][i].LRU > maxTime) {
                    maxTime = line[S_addr][i].LRU;
                    maxTimeIndex = i;
                }
            }
        }
        if(isFull) {
            eviction++;
            if(verbose) {
                printf(" eviction");
            }
            line[S_addr][maxTimeIndex].tag = E_addr;
            line[S_addr][maxTimeIndex].LRU = 0;
            for(int i = 0; i < E; ++i) {
                if(i == maxTimeIndex) {
                    continue;
                }
                line[S_addr][i].LRU++;
            }
        }
    }
}

void store(ull S_addr, ull E_addr, int E) {
    load(S_addr, E_addr, E);
}

ull getaddr(ull addr, int S, int b) {
    ull shift = b;
    ull mask = (1 << S) - 1;
    return (addr >> shift) & mask;
}

int main(int argc, char *argv[]) {
    int ret;
    int S = 0, E = 0, b = 0;
    FILE *fp;
    while((ret = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        char *endptr;
        switch(ret) {
            case 'h': {
                break;
            }
            case 'v': {
                verbose = true;
                break;
            }
            case 's': {
                S = (int)strtol(optarg, &endptr, 10);
                if(*endptr != '\0') {
                    fprintf(stderr, "Error: invalid argument -s %s\n", optarg);
                    exit(1);
                }
                break;
            }
            case 'E': {
                E = (int)strtol(optarg, &endptr, 10);
                if(*endptr != '\0') {
                    fprintf(stderr, "Error: invalid argument -E %s\n", optarg);
                    exit(1);
                }
                break;
            }
            case 'b': {
                b = (int)strtol(optarg, &endptr, 10);
                if(*endptr != '\0') {
                    fprintf(stderr, "Error: invalid argument -b %s\n", optarg);
                    exit(1);
                }
                break;
            }
            case 't': {
                fp = fopen(optarg, "r");
                if(fp == NULL) {
                    fprintf(stderr, "Error: cannot open file %s\n", optarg);
                    exit(1);
                }
                break;
            }
            default: {
                fprintf(stderr, "Error: invalid argument -%c\n", ret);
            }
        }
    }
    if(S == 0 || E == 0 || b == 0 || fp == NULL) {
        fprintf(stderr, "Error: missing required argument\n");
        exit(1);
    }

    char lineIn[1000];
    char op;
    ull addr;
    int size;
    while(fgets(lineIn, 1000, fp) != NULL) {
        sscanf(lineIn, " %c %llx,%d", &op, &addr, &size);
        if(verbose) {
            printf("%c %llx,%d", op, addr, size);
        }
        ull E_addr = addr >> (S + b);
        ull S_addr = getaddr(addr, S, b);
        switch(op) {
            case 'I': {
            //    puts("I");
                break;
            }
            case 'L': {
                load(S_addr, E_addr, E);
                break;
            }
            case 'S': {
                store(S_addr, E_addr, E);
                break;
            }
            case 'M': {
                load(S_addr, E_addr, E);
                store(S_addr, E_addr, E);
                break;
            }
            default:
                break;
        }
        if(verbose) {
            puts("");
        }
    }

    printSummary(hit, miss, eviction);
    fclose(fp);
    return 0;
}
