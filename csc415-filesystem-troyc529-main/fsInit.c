/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Names: 
* Student IDs:
* GitHub Name:
* Group Name:
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "VCB.h"
#include "freespace.h"

typedef struct
{
	char name[256];			 /* name */
	unsigned char type;		 /* D or F */
	unsigned short d_reclen; /* total length of this record */
	unsigned short length;	 /* which directory entry position, like file pos */
	short int ParentLBA;
	uint64_t locationLBA; /* Starting LBA of directory */
	time_t dateCreated;	  /* created */
	time_t lastModified;  /* last modified */
} test;


int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize)
	{
	printf ("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);

	int vol_size = numberOfBlocks*blockSize; //volume size
	
	vPtr = malloc(blockSize); //vcb pointer


	LBAread(vPtr,1,0);
	
	if (vPtr->magicNumber == 11111)	//checks if volume is already initialized
		{
		printf("Magic Number matched\n");
		fs_ptr = malloc(5*blockSize);
		LBAread(fs_ptr, 5, vPtr->freeSpaceLocation);
		int res = initCD();
		printf("RESULT OF INIT CD: %d \n", res);
	}
	else
	{
		printf("Magic Number did NOT match\n");
		printf("Initializing system\n");
		initializeVCB(vol_size,blockSize);
		LBAwrite(vPtr,1,0);
		Init_freeSpace(blockSize, vol_size);
		int res = createRootDirectory();
		printf("RESULT OF CREATE ROOT: %d \n", res);

	}

	return 0;
	}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}