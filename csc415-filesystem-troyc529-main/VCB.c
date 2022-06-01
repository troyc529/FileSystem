#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "VCB.h"

vcb *vPtr;


void initializeVCB(uint64_t vSize, uint64_t blockSize){

    

    if(vPtr->initialized == 0){
    vPtr = malloc(blockSize);
    printf("initializing vcb.\n");
    vPtr->initialized = 1;
    vPtr->blockSize = blockSize;
    vPtr->totalBlocks = vSize / blockSize;
    printf("total blocks = %d\n", vPtr->totalBlocks);
    vPtr->freeSpaceLocation = 1;
    vPtr->rootDirectoryLocation = 6;
    vPtr->magicNumber = 11111;

    vPtr->volName = "name!\n";  

    }
    else{
        printf("already initialized. \n");
    }


}


int getBlockSize(){
    return vPtr->blockSize;
}

int getTotalBlocks(){
    return vPtr->totalBlocks;
}


int readVolumeHeader(){
    switch(vPtr->initialized){
        case !0:
        printf("initialized, reading first block. \n");
        return LBAread(vPtr, 1, 0);
        break;

        default:
        printf("NOT initialized! aborting\n");
        return -1;
        break;

    }
}



int writeVolumeHeader(){
     switch(vPtr->initialized){
        case (!0):
        printf("initialized, writing header to LBA block 0. \n");
        return LBAwrite(vPtr, 1, 0);
        break;

        default:
        printf("NOT initialized! aborting\n");
        return -1;
        break;

    }

}