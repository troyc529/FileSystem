/**************************************************************
* Class:  CSC-415
* Name: 
* Student ID: 
* Project: 
*
* File: 
* Description: 
*	
*	
*	
*
**************************************************************/

#include "Bitmap.h"

void ClearBit(int map[], int i)
{
    map[i / 32] |= 1 << (i % 32);
}

void SetBit(int map[], int i)
{
    map[i / 32] &= ~(1 << (i % 32));
}

int findBit(int map[], int i)
{
    //return i if bit is 1
    if((map[i / 32] & (1 << (i % 32)))){
        return i;
    }
    //return -1 if bit is 0
    else{
        return -1;
    }
}