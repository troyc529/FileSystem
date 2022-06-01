#ifndef _freespace_H
#define _freespace_H

typedef struct FreeSpaceAllocation
{
	int blockBegin;				   //starting block of our free space
	int blockcount;		   //total blocks used
	int bitMapBlockCount; //bitmap for used blocks in current partition, single number to indicate
	int bitMap_size;		   //611 ints
	int bitMap[];
} freeSpace;

freeSpace *fs_ptr;

void Init_freeSpace(uint64_t blockSize, uint64_t volSize);
int findFirstFreeBlock();
int allocate_space(int blocks);

#endif