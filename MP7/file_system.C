/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"


/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    numblocks = 0;
    Console::puts("In file system constructor.\n");
    Console::puti(numblocks);
    blockptr = -1;
    
    node = (iNode*)inodeData;
    
    for(int i = 0;i<NUM_BLOCKS;i++)
    blockStatus[i] = false;
    for(int i = 0;i<200;i++)
    {
        fileIdMap[i].id = -1;
        fileIdMap[i].file = NULL;
    }    
    //assert(false);
}

/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/

bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system form disk\n");
    disk = _disk;   
    return true;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) {
    Console::puts("formatting disk\n");
    char buf[512];
    memset(buf,0,512);
    for(int i=0;i<100;i++)
    _disk->write(i,(unsigned char*)buf);
    return true;
}

File * FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file\n");

    for(int i = 1;i<=numblocks;i++)
    {
        if(fileIdMap[i].id == _file_id)
            return fileIdMap[i].file;
    }
    
    return NULL;
    //assert(false);
}

bool FileSystem::CreateFile(int _file_id) {
    
    if(LookupFile(_file_id)!=NULL)
    return false;
    
    Console::puts("creating file \n");
    File *file = (File*) new File(_file_id, disk,blockStatus);
    
    fileIdMap[++numblocks].file  = file;
    fileIdMap[numblocks].id  = _file_id;
    //Console::puti(numblocks);
    node->id = _file_id;
    node->fileSize = 0;
    node->numBlocks = 0;
    blockptr = (blockptr +1 ) % 25;
    disk->write(blockptr,inodeData);
    return true;
    //assert(false);
}

bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("deleting file\n");
    
    for(int i = 1;i<=numblocks;i++)
    {
       if(fileIdMap[i].id == _file_id)
       {
         File *file = fileIdMap[i].file;
         
         for(int j=0;j<=file->last_pos;j++)
            blockStatus[file->blockAllocated[j]] = false;
         
         File *f = fileIdMap[i].file;
         delete f;
         fileIdMap[i].file = NULL;
         fileIdMap[i].id = -1;
         return true;
       }
    }
     return false;
    
    return true;
    //assert(false);
}
