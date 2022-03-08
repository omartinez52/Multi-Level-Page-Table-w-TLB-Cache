#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "PageTable.h"
#include "tracereader.h"
#include "output_mode_helpers.h"

using namespace std;


bool ProcessCommandLineArguments(int, char**, FILE*, PageTable*);
void ProcessBitmaskAry(int, PageTable*, int, int);

int main(int argc, char **argv){
    PageTable *pgTable = new PageTable;
    FILE *ifp;	        /* trace file */
    p2AddrTr trace;	/* traced address */
    unsigned long i = 0;  /* instructions processed */
    if(!ProcessCommandLineArguments(argc,argv,ifp,pgTable)){
        exit(1);
    }
    // allocates level 0. sets depth to 0 and sizes array to entry count
    AllocateFirstLevel(pgTable);
    // reade trace file
    // while (!feof(ifp)) {
    //     /* get next address and process */
    //     if (NextAddress(ifp, &trace)) {
    //     AddressDecoder(&trace, stdout);
    //     i++;
    //     if ((i % 100000) == 0)
    //     fprintf(stderr,"%dK samples processed\r", i/100000);
    //     }
    // }	
    /* clean up and return success */
    delete(pgTable);
    fclose(ifp);
    return 0;
}

bool ProcessCommandLineArguments(int argc, char **argv,FILE* ifp, PageTable* pgTable){
    // terminate program if no mandatory arguments present
    if(argc <= 1){return false;}
    // 32-bit
    int bits = 32;
    // unsigned long num_processes = -1; /* number of memory access to process */
    //cmd->numProcesses = -1; // default val, if negative process all addresses
    int total_level_bits = 0;   /* total bits amongst all pages */
    int option; /* getopt() option */
    //int cache_capacity = 0;
    int level = 0; /* number of page levels */

    //string output_mode = "";

    while((option = getopt(argc, argv, "n:c:o:")) != -1){
        switch(option){
            case 'n':
                //cmd->numProcesses = atoi(optarg);
                //cout << "Process only the first "<< optarg <<" memory accesses/references.\n";            
                break;
            case 'c':
                if(atoi(optarg) < 0){
                    cout << "Cache capacity must be a number, greater than or equal to 0\n";
                    return false;
                }
                //cmd->cacheCapacity = atoi(optarg);
                //cout << "Cache capacity:\t"<<optarg<<endl;
                break;
            case 'o':
                //cmd->outputMode = optarg;
                //cout << "Output mode:\t"<<optarg<<endl;
            default:
                break;
        }
    }
    // no page level bits included
    if(optind + 1 == argc){
        cout << "Must provide Page Level Bits as argument\n";
        return false;
    }
    /* attempt to open trace file */
    if ((ifp = fopen(argv[optind],"rb")) == NULL) {
        cout << "Unable to open <<"<<argv[optind]<<">>\n";
        return false;
    }
    // loop through size of pages
    for(int i = optind+1; i < argc; i++){
        int level_bits = atoi(argv[i]);
        // return false if no level bits provided
        if(level_bits < 1){
            cout <<"Level "<<level<<" page table must be at least 1 bit\n";
            return false;
        }
        // process bitmasking for each level
        ProcessBitmaskAry(bits, pgTable, level_bits, level);
        // bits change based on bits each level occupies
        bits -= level_bits;
        // bits to shift for each level
        pgTable->ShiftAry.push_back(bits);
        // store entry count for each level
        pgTable->EntryCount.push_back(pow(2,level_bits));
        cout <<"Level "<<level<<" has "<<level_bits<<" bits\n";
        // increment level count
        level++;
        total_level_bits += level_bits;
    }
    // store total number of levels into struct
    pgTable->levelCount = level;
    // check if total number of bits is less than 28
    if(total_level_bits > 28){
        cout << "Too many bits used in page tables\n";
        return false;
    }
    return true;
}
// calculates bit masking for each page based on page size
void ProcessBitmaskAry(int bitsToShift, PageTable* pgTable, int level_bits, int curLevel){
    uint32_t maskVal = 0;
    for(int i = 0; i < bitsToShift; i++){
        maskVal = maskVal << 1;
        maskVal = maskVal | 1;
    }
    // maskVal = maskVal & int(pow(2,(bitsToShift - level_bits))-1);
    // unwanted bits in the bitmask
    int remove = pow(2,(bitsToShift - level_bits))-1;
    // 0xFFFFFFFF ^ 0x00FFFFFF = 0xFF000000
    maskVal = maskVal ^ remove;
    pgTable->BitmaskAry.push_back(maskVal);
}