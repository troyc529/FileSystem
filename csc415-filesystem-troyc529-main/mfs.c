/**************************************************************
* Class:  CSC-415
* Name: 
* Student ID: N/A
* Project: Basic File System
*
* File: mfs.c
*
* Description: 
*	
*
**************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "freespace.h"
#include "VCB.h"
#include "Bitmap.h"

#define MAX_DIR_ENTRIES 50      //number entries each directory can hold
#define BLOCK_SIZE 512

/**GLOBAL DIRECTORY INDEX COUNTER */

int d_index = 0;
char * delim = "/";
    /** */
int initialized = 0;
int sizeOfDirEntries = MAX_DIR_ENTRIES * sizeof(dE);    //size of directory entry
int freeBlock;
int space = sizeof(dE) * MAX_DIR_ENTRIES / BLOCK_SIZE;  //allocation size for directories in blocks
dE *cd = NULL;   //current directory
dE *openCD = NULL;  //accessed directory
struct fs_diriteminfo *f;

//sets current working directory upon start up
dE *setcd(int location)
{

  cd = (dE *)malloc(sizeOfDirEntries);
  LBAread(cd, space, location);

  return cd;
}


//looks for directory by name, returns index 
int p_de(char * name, dE * temp_dE)
{
  for (size_t i = 0; i < temp_dE[0].numberOfEntriesInArray; i++)
  {
   
    if (strcmp(name, temp_dE[i].name)==0)
    {
      return i;
    }
    
  }
  return -1;
}

//looks for first null index or returns last index of directory
int null_de(dE * temp_dE)
{
  int i;
  for (i = 0; i < temp_dE[0].numberOfEntriesInArray; i++)
  {
   
    if (temp_dE[i].name[0] == '\0')
    {
      return i;
    }
    
  }
  return i;
}

//searches for directory by path, starting from current directory returns 1 success 0 fail
int parseDir(char * path){

     char *token;
     char *temp_S = calloc(1,sizeof(path));
     strcpy(temp_S,path);
    dE * temp = malloc(sizeOfDirEntries);
    LBAread(temp, space, cd[0].currentLBA);

    token = strtok_r(temp_S, delim, &temp_S);
    while (token != NULL) //parses path, searches for directory or returns invalid if not found
    {
     
      int i = p_de(token, temp);
       
      if (i == -1)
      {
        printf("Invalid path\n");
  
        temp = NULL;
        temp_S = NULL;
        return 0; 
      }
      else
      {
        LBAread(temp, space, temp[i].currentLBA);
      }
     
      token = strtok_r(NULL, delim, &temp_S);
      
    }
     free(temp);
        temp = NULL;
    free(temp_S);
        temp_S = NULL;
    return 1;

}

//initializes root directory
int createRootDirectory()
{

  dE *d = malloc(sizeOfDirEntries);
  char *root_folder[5] = {".", ".."};
  d[0].path[0] = '/';
  for (int i = 0; i < 2; i++)
  {
    if (i == 1)
    {
      d[i].currentLBA = d[0].currentLBA;
      strcpy(d[i].name, root_folder[i]);
      d[i].parentLBA = d[i].currentLBA;
      d[i].path[0] = d[0].path[0];
      d[i].type = 'd';
    }
    else
    {
      d[i].currentLBA = allocate_space(space);
      d[i].parentLBA = d[0].currentLBA;
      strcpy(d[i].name, root_folder[i]);
      d[i].type = 'd';
   
    }
    d[0].numberOfEntriesInArray++;
  }

    d[0].fd.stats.st_size = (sizeof(dE)*2);

  LBAwrite(d, space, d[0].currentLBA);

  setcd(d[0].currentLBA);


  return d[0].currentLBA;
}

//loads root directory if already initialized
int initCD()
{

	setcd(vPtr->rootDirectoryLocation);
  printf("%s\n", cd[0].path);
	return cd[0].currentLBA;
}


//creates a new directory in cd
int fs_mkdir(const char *pathname, mode_t mode)
{
   time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime (&rawtime);
  



  //checks if name is already taken
  for (size_t i = 0; i < cd[0].numberOfEntriesInArray; i++)
  {

    if (strcmp(pathname, cd[i].name) == 0)
    {
      printf("DIRECTORY ALREADY EXISTS\nPATH: %s\n cdname: %s\n", pathname, cd[i].name);
      return -1;
    }
  }

  dE *dir = malloc(sizeOfDirEntries);
  dir[0].fd.stats.st_createtime = (time_t)asctime(timeinfo);

  dir[0].numberOfEntriesInArray = 0;
  dir[0].fd.stats.st_size = 0;
  int length = strlen(pathname);
  char *folder[2] = {".", ".."};
  for (int i = 0; i < 2; i++)
  {
    if (i == 1)
    {
      dir[i].currentLBA = dir[0].parentLBA;
      strcpy(dir[i].name, folder[i]);
      dir[i].parentLBA = cd[0].parentLBA;
      dir[i].path[0] = cd[0].path[0];
      dir[i].type = 'd';
    }
    else
    {
      dir[i].currentLBA = allocate_space(space);
      dir[i].parentLBA = cd[0].currentLBA;
      strcpy(dir[i].name, folder[i]);
      strcpy(dir[i].parent_name, pathname);
      dir[i].type = 'd';
      strcpy(dir[i].path, cd[0].path);
      strcat(dir[i].path, pathname);
      strcat(dir[i].path, "/");
    }
    dir[0].numberOfEntriesInArray++;

  }
  dir[0].fd.stats.st_size += (sizeof(dE)*2);

  int write_index = null_de(cd);
 if (write_index = cd[0].numberOfEntriesInArray)
     {
          cd[0].numberOfEntriesInArray++;
          cd[0].fd.stats.st_size += (sizeof(dE)*2);
       }
      
     

  
  cd[write_index] = dir[0];
  strcpy(cd[write_index].name, pathname);

  int err = LBAwrite(dir, space, dir[0].currentLBA);
  
//error detection if the proper amount of bytes were not written returns error
  if (err == space)
  {
     free(dir);
     dir = NULL;
    LBAwrite(cd, space, cd[0].currentLBA);
    return 1;
  }
  else
  {
    printf("Failed to make directory\n");

    return 0;
  }
}


//creates file in cd
int makeFile(const char *pathname, mode_t mode)
{

  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime (&rawtime);
  

  //checkes if file name already exists
  for (size_t i = 0; i < cd[0].numberOfEntriesInArray; i++)
  {

    if ((strcmp(pathname, cd[i].name) == 0 ) && cd[i].type == 'f')
    {
      printf("FILE ALREADY EXISTS\nPATH: %s\n cdname: %s\n", pathname, cd[i].name);
      return -1;
    }
  }


  dE * dir;
 

    dir = malloc(sizeof(dE));
  dir[0].fd.stats.st_createtime = (time_t)asctime(timeinfo);

    dir[0].currentLBA = allocate_space(space);
      dir[0].parentLBA = cd[0].currentLBA;
       strcpy(dir[0].name, pathname);
      strcpy(dir[0].parent_name, pathname);
      dir[0].type = 'f';
      strcpy(dir[0].path, cd[0].path);
      strcat(dir[0].path, pathname);
      strcat(dir[0].path, "/");
  
    
  int write_index = null_de(cd);
  if (write_index >= cd[0].numberOfEntriesInArray)
      {
        cd[0].numberOfEntriesInArray++;
      }
    cd[write_index] = dir[0];
    strcpy(cd[write_index].name, pathname);

    int err = LBAwrite(dir, space, dir[0].currentLBA);
    

    if (err == space)
    {
     free(dir);
     dir = NULL;
    LBAwrite(cd, space, cd[0].currentLBA);
    return 1;
  }
  else
  {
    printf("Failed to make file\n");

    return 0;
  }
}


// parses path starting from passed in directory, returns directory if found
dE * parseDir2(char * path, dE * temp){

     char *token;


    token = strtok_r(path, delim, &path);
    int i;
    while (token != NULL) //parses read
    {

      i = p_de(token, temp);
      if (i == -1)
      {
        return NULL;
      }
      else
      {
        LBAread(temp, space, temp[i].currentLBA);
      }
     
      token = strtok_r(NULL, delim, &path);
      
    }

    return  temp;

}


int test =0; //tracks number of recursive calls


//recursive rm, sets values of directoy to null and sets space in bitmap to be overwritten
int r_de(const char * pathname, dE * temp)
{
    dE * parent = malloc(sizeOfDirEntries);
  LBAread(parent, space, temp[0].parentLBA);

  int index = p_de(temp[0].parent_name,parent);
  //if directory does not exists returns 0
   if (index == -1 )
  {
    return 0;
  }
  
  //if not first call searches for directory by path
  if (test > 0)
  {
    temp = parseDir2( (char *)pathname, temp);
    
  }

  //sets name/path of directoy to NULL
  parent[index].name[0] ='\0';
  parent[index].path[0] = '\0';
  temp[0].path[0] = '\0';

  //if no subdirectories within directory perform miminal deletion 
  if (temp[0].numberOfEntriesInArray == 2)
  {
     temp[0].name[0] ='\0';
      int space_size = temp[0].currentLBA;
      //clears bitmap so space can be reused
      for (int j = 0; j < 39; j++)
      {
        ClearBit(fs_ptr->bitMap, space_size);
        space_size++;
      }
        LBAwrite(parent, space, parent[0].currentLBA);
      return 0;
  }
  else
  {
 //if subdirectories exist delete function is recursively called on subdirectories
  for (size_t i = 2; i < temp[0].numberOfEntriesInArray; i++)
  {
    if (temp[i].type == 'd')
    {

      temp[i].name[0] ='\0';
      int space_size = temp[i].currentLBA;
      //clears bitmap so space can be reused
      for (int j = 0; j < space; j++)
      {
        ClearBit(fs_ptr->bitMap, space_size);
        space_size++;
      }
       test++;
       LBAwrite(parent, space, parent[0].currentLBA);
       LBAwrite(temp, space, temp[0].currentLBA);
      r_de(temp[i].path, temp);
    }

  } 
     

  }

}

//rm, remove directory main function, returns 0 on fail 1 on success
int fs_rmdir(const char *pathname){
     dE * temp = malloc(sizeOfDirEntries);
     dE* temp_cd = malloc(sizeOfDirEntries); 
     LBAread(temp_cd, space, cd[0].currentLBA);
    temp = parseDir2( (char *)pathname, temp_cd);
    if (temp == NULL)
    {
      printf("FAILED TO FIND\n");
      return 0;
    }
    
  r_de(pathname,temp);
  free(temp);
  temp = NULL;
  LBAread(cd, space, cd[0].currentLBA);
  test = 0;

  return 1;

};

//mv, moves a file to another directory returns 1 on success 0 fail
int mv_file(char * source, char * destination){

dE * source_D = malloc(sizeOfDirEntries);
dE * destination_D = malloc(sizeOfDirEntries);
LBAread(destination_D, space, cd[0].currentLBA);
LBAread(source_D, space, cd[0].currentLBA);

destination_D = parseDir2(destination, destination_D);

source_D = parseDir2(source, source_D);
LBAread(source_D, space, source_D[0].parentLBA);

int index_S = p_de(source, source_D);

//checks if file name already exists in target directory
 for (size_t i = 0; i < cd[0].numberOfEntriesInArray; i++)
  {

    if ((strcmp(source, destination_D[i].name) == 0 ) && destination_D[i].type == 'f')
    {
      printf("FILE NAME ALREADY EXISTS\n");
      return 0;
    }
  }

int index_D = null_de(destination_D);
//increments number of entries in target directory
if (index_D == destination_D[0].numberOfEntriesInArray)
{
  destination_D[0].numberOfEntriesInArray++;
}

//assigns target file to destination directory
destination_D[index_D] = source_D[index_S];

LBAwrite(source_D, space, source_D[0].currentLBA);

//deletes original file
fs_delete(source_D[index_S].path); //remove for cpy

destination_D[index_D].parentLBA = destination_D[0].currentLBA;
strcpy(destination_D[index_D].path, destination_D[0].path);
strcat(destination_D[index_D].path, destination_D[index_D].parent_name);

int err = LBAwrite(destination_D, space, destination_D[0].currentLBA);

free(source_D);
free(destination_D);
source_D = NULL;
destination_D = NULL;

if (err = space)
{
  return 1;
}
else
  return 0;

}




// Directory iteration functions
fdDir *fs_opendir(const char *name)
{


  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime (&rawtime);
  

  fdDir *temp = malloc(sizeof(fdDir));
  int res;

 
  //if in current directory switch 1
  if (strcmp(name, cd[0].path)== 0)
  {
    res = 1;
   
  }
  //if not cd access directory
   else
  {
    openCD = malloc(sizeOfDirEntries);
     LBAread(openCD,space,cd[0].currentLBA);
     char * test = calloc(1,sizeof(name));
   strcpy(test, name);
    openCD = parseDir2((char *)test,openCD);
    res = 0;
    //failed to find directory
     if (openCD == NULL)
    { 
        res  = -1;
    }
  
  }
  


  switch (res)
  {
  case 1:

    temp->directoryStartLocation = cd[0].currentLBA;
    temp->d_reclen = cd[0].numberOfEntriesInArray;
    temp->dirEntryPosition = cd[0].currentLBA;
    temp->type = cd[0].type;
    
    strcpy(temp->name, cd[0].name);
    openCD = malloc(sizeOfDirEntries);
    LBAread(openCD,space,cd[0].currentLBA);

    break;

  case 0:

    temp->directoryStartLocation = openCD[0].currentLBA;
    temp->d_reclen = openCD[0].numberOfEntriesInArray;
    temp->dirEntryPosition = openCD[0].currentLBA;
    temp->type = openCD[0].type;

    strcpy(temp->name, openCD[0].name);
    break;

  default:
    printf("cannot find %s \n", name);

    break;
  }
 
  temp->stats.st_accesstime = (time_t)asctime(timeinfo);



  return temp;
}

//index tracking for read
int count = 0;

//populates diriteminfo object
struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{

  //if index exceeds number of entries end read, reset index
  if (count == dirp->d_reclen)
  {
    count = 0;
    return NULL;
  }

 //skip null directory entries
 while (openCD[count].name[0] == '\0')
 {
   count ++;
   if (count >= dirp->d_reclen)
   {
     count = 0;
     return NULL;
   }
   

 }
  

f = malloc(sizeof(struct fs_diriteminfo));


  strcpy(f->d_name, openCD[count].name);

  count++;

  return f;
};

//frees memory after ls call
int fs_closedir(fdDir *dirp)
{
  
  free(dirp);
  free(f);
  f = NULL;
  dirp = NULL;
  free(openCD);
  openCD = NULL;
  count = 0;
  return 0;

};

// returns current directory
char *fs_getcwd(char *buf, size_t size)
{

  return cd[0].path;
}

//changes current directory returns -1 on fail 0 on success
int fs_setcwd(char *buf)
{
  if (!buf)
  {
    return -1;
  }

  //if accessing previous directory ".."
  if (strcmp(buf, cd[1].name) == 0)
  {
    LBAread(cd, space, cd[0].parentLBA);
    return 0;
  }
  //if accessing current directory "."
  if (strcmp(buf, cd[0].name) == 0)
  {
    printf("in the .\n");
    LBAread(cd, space, cd[0].currentLBA);
    return 0;
  }


  dE * temp = malloc(sizeOfDirEntries);
  LBAread(temp,space,cd[0].currentLBA);
  temp = parseDir2(buf,temp);
  //checks for directory not found via NULL of if directory object is file
  if (temp == NULL || temp[0].type == 'f')
  {
    free(temp);
    temp = NULL;
    return -1;
  }
  else
  {
    LBAread(cd, space, temp[0].currentLBA);
    free(temp);
    temp = NULL;
    return 0;
  }

}; 


//checks if directory object is file, returns 0 on fail 1 on success
int fs_isFile(char *path)
{
  char * test = calloc(1,sizeof(path));
   strcpy(test, path);
dE * temp = malloc(sizeOfDirEntries);

  LBAread(temp, space, cd[0].currentLBA);

  temp = parseDir2(test, temp);
  //if NULL no entry was found
  if (temp == NULL)
  {
    free(test);
    test = NULL;
    free(temp);
    temp = NULL;
    return 0;
  }
  if (temp[0].type == 'f')
  {
    free(test);
    test = NULL;
    free(temp);
    temp = NULL;
    return 1;
  }
  else
  {
    free(test);
    test = NULL;
    free(temp);
    temp = NULL;
    return 0;
  }


}; 

//checks if directory object is directory returns 1 on success 0 fail
int fs_isDir(char *path)
{
  char * test = calloc(1,sizeof(path));
   strcpy(test, path);
dE * temp = malloc(sizeOfDirEntries);

  LBAread(temp, space, cd[0].currentLBA);

  temp = parseDir2(test, temp);
    //if NULL no entry was found
  if (temp == NULL)
  {
    free(test);
    test = NULL;
    free(temp);
    temp = NULL;
    return 0;
  }

  if (temp[0].type == 'd')
  {
    free(test);
    test = NULL;
    free(temp);
    temp = NULL;
    return 1;
  }
  else
  {
    free(test);
    test = NULL;
    free(temp);
    temp = NULL;
    return 0;
  }

}


    int fs_stat(const char *path, struct fs_stat *buf)
{

        if(strcmp(openCD[count].path, path) ==0){
            
          buf->st_createtime = openCD[count].fd.stats.st_createtime; 
          buf->st_accesstime = openCD[count].fd.stats.st_accesstime; 
          
          buf->st_size = sizeof(dE)+(sizeof(dE) * openCD[0].numberOfEntriesInArray);
          buf->st_blksize = BLOCK_SIZE;
          buf->st_blocks = vPtr->totalBlocks - vPtr->totalFreeBlocks;
         
              return 0;

        }



      


  return -1;
};


//deletes file, returns 0 on fail 1 on success
int fs_delete(char* filename){

  dE * temp = malloc(sizeOfDirEntries);
     dE* temp_cd = malloc(sizeOfDirEntries); 
     LBAread(temp_cd, space, cd[0].currentLBA);
    temp = parseDir2(filename, temp_cd);
    if (temp == NULL)
    {
      printf("FAILED TO FIND\n");
      return 0;
    }

     dE * parent = malloc(sizeOfDirEntries);
  LBAread(parent, space, temp[0].parentLBA);

  int index = p_de(temp[0].parent_name,parent);
   if (index == -1 )
  {
    return 0;
  }
  
  parent[index].name[0] ='\0';
  parent[index].path[0] = '\0';
  temp[0].path[0] = '\0';



 temp[0].name[0] ='\0';
      int space_size = temp[0].currentLBA;
      for (int j = 0; j < 39; j++)
      {
        ClearBit(fs_ptr->bitMap, space_size);
        space_size++;
      }
        LBAwrite(parent, space, parent[0].currentLBA);
     

  free(temp);
  temp = NULL;
  LBAread(cd, space, cd[0].currentLBA);

  return 1;

};	

