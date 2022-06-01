#include <stdio.h>
#include <stdlib.h>

#include "mfs.h"
#include "fsLow.h"
#include "freespace.h"
#include "Bitmap.h"


//initializes free space
void Init_freeSpace(uint64_t blockSize, uint64_t volSize){

    fs_ptr = malloc(5*blockSize);

    int LBA_totalBlocks = (volSize / blockSize);	  //19531 blocks
	int fs_numBytes= ((LBA_totalBlocks+7) / 8); //2442 bytes
    fs_ptr->bitMap_size = ((fs_numBytes/blockSize)+1);
    fs_ptr->bitMap[fs_ptr->bitMap_size];

    for (size_t i = 0; i < LBA_totalBlocks; i++)  //populates bitmap
    {
        if (i <= 5)
        {
			
            SetBit(fs_ptr->bitMap,i);
        }
        else
        {
		
            ClearBit(fs_ptr->bitMap,i);
        }
        
    }

    fs_ptr->blockBegin = 1;
    fs_ptr->blockcount = 5;

    LBAwrite(fs_ptr,fs_ptr->blockcount,fs_ptr->blockBegin);
}


//finds first free block for write
int findFirstFreeBlock()
{
	int foundBlock = 0;
	int i = 0;
	while (foundBlock == 0)
	{
		if ((findBit(fs_ptr->bitMap, i)) != -1)
		{
			foundBlock = 1;
			return i;
		}
		i++;
	}
}


//sets bits in bitmap 
int allocate_space(int blocks)
{
	int freeBlockNum = findFirstFreeBlock();

	int firstFreeBlockNum = freeBlockNum;
	for (int i = 1; i <= blocks; i++)
	{
		if (freeBlockNum != 0)
		{
			SetBit(fs_ptr->bitMap, freeBlockNum); //set bit of this block to 0 to indicate that it is used
		}
		else
		{
			firstFreeBlockNum = -1;
		}
		freeBlockNum = findFirstFreeBlock();
	}
	LBAwrite(fs_ptr, fs_ptr->blockcount, fs_ptr->blockBegin); //writes the changes of the bitmap to memory
	
	return firstFreeBlockNum;
}