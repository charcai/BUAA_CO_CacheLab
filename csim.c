#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
    int ret;
    bool verbose = 0;
    int S, E, b;
    FILE *fp;
    while((ret = getopt(argc, argv, "vs:E:b:t:")) != -1) {
        char *endptr;
        switch(ret) {
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
    printf("%d %d %d %d\n", verbose, S, E, b);
    printSummary(0, 0, 0);
    return 0;
}
