/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: mfs.h
*
* Description: 
*    This is the file system interface.
*    This is the interface needed by the driver to interact with
*    your filesystem.
*
**************************************************************/
#ifndef _MFS_H
#define _MFS_H
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "b_io.h"

#include <dirent.h>
#define FT_REGFILE    DT_REG
#define FT_DIRECTORY DT_DIR
#define FT_LINK    DT_LNK

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif
#ifndef uint32_t
typedef u_int32_t uint32_t;
#endif

// This structure is returned by fs_readdir to provide the caller with information
// about each file as it iterates through a directory
struct fs_diriteminfo
    {
    unsigned short d_reclen;    /* length of this record */
    unsigned char fileType;    
    char d_name[256];             /* filename max filename is 255 characters */
    };




// This is the structure that is filled in from a call to fs_stat
typedef struct fs_stat
    {
    off_t     st_size;            /* total size, in bytes */
    blksize_t st_blksize;         /* blocksize for file system I/O */
    blkcnt_t  st_blocks;          /* number of 512B blocks allocated */
    time_t    st_accesstime;       /* time of last access */
    time_t    st_modtime;       /* time of last modification */
    time_t    st_createtime;       /* time of last status change */
    
    /* add additional attributes here for your file system */
    } fsStat;
// This is a private structure used only by fs_opendir, fs_readdir, and fs_closedir
// Think of this like a file descriptor but for a directory - one can only read
// from a directory.  This structure helps you (the file system) keep track of
// which directory entry you are currently processing so that everytime the caller
// calls the function readdir, you give the next entry in the directory
typedef struct
    {

    /*****TO DO:  Fill in this structure with what your open/read directory needs  *****/
    unsigned short  d_reclen;        /*length of this record */
    unsigned short    dirEntryPosition;    /*which directory entry position, like file pos */
    uint64_t    directoryStartLocation;        /*Starting LBA of directory */
    char name[256];     //object name
    char type;          //type either file or directory
    fsStat stats;       
    } fdDir;


typedef struct
    {
    char name[130];    //directory name 
    char path[130];     //path of directory from root
    int parentLBA;      //parent directory location on disk
    int currentLBA;     //directory location on disk
    unsigned char type;     // type either file or directory
    int numberOfEntriesInArray; //number of subdirectories
    char parent_name[130];      //name of directory in parent 
     fdDir fd;                  
         } dE;



// Key directory functions
int fs_mkdir(const char *pathname, mode_t mode);
int makeFile(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);

// Directory iteration functions
fdDir * fs_opendir(const char *name);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

// Misc directory functions
int mv_file(char * source, char * destination);
char * fs_getcwd(char *buf, size_t size);
int fs_setcwd(char *buf);   //linux chdir
int fs_isFile(char * path);    //return 1 if file, 0 otherwise
int fs_isDir(char * path);        //return 1 if directory, 0 otherwise
int fs_delete(char* filename);    //removes a file




extern dE * cd;

int fs_stat(const char *path, struct fs_stat *buf);

int parseDir(char * path);

int createRootDirectory();
int initCD();

int createDirectory(int destination, int self_block, int parent_block);

#endif