#ifndef _VCB_H
#define _VCB_H

typedef struct VCB{
 
    int initialized;
    char * volName;
    int blockSize;
    int totalBlocks;
    int totalFreeBlocks;
    int freeSpaceLocation; 
    int rootDirectoryLocation;
    int magicNumber; 

} vcb;

vcb *vPtr;


void initializeVCB(/*char * fileName,*/ uint64_t vSize, uint64_t blockSize);
int getBlockSize();
int getTotalBlocks();
int readVolumeHeader();
int writeVolumeHeader();

#endif