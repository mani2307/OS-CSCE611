/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2017/05/01

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"
int File::position =24;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(int _id, SimpleDisk * _disk, bool *_blockStatus) {
    /* We will need some arguments for the constructor, maybe pointer to disk
     block with file management and allocation data. */
     id = _id;
     blockStatus = _blockStatus;
     disk = _disk;
     last_pos = -1; // initial 25 blocks are saved for inode management
     read_pointer = 0;
     curr = 0;
     read_data = 0;
     curr_w =0;
     write_data = 0;
     
    for (int i=0;i<MAX_BLOCKS;i++)
    blockAllocated[i] = -1;
    Console::puts("In file constructor.\n");

    //assert(false);
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char * _buf) {
    Console::puts("reading from file\n");
    //assert(false);
    
    int i =0;
    int p = 0;
    while(i<_n)
    {
        
        //Console::puts("Before Read disk");
        if(curr==0)
        disk->read(blockAllocated[read_pointer],buf);
        //Console::puts("After Read disk");
        
        while(curr+i<512 && i<_n && read_data+i<=write_data)
        {
            _buf[i] = buf[curr+p];
            i++;
            p++;
        }
        curr = curr +p;
        
        if(curr>=512)
        {
            read_pointer++;
            curr=0;
            p = 0;
        }
        if(read_data+i>write_data)
        {
            i--;
            break;
        }
        
    }
    /*
        for(int j=0;j<i;j++) 
        Console::putch(_buf[j]);
       
        
    Console::puts("\n Count: ");
    Console::puti(i);*/
    read_data = read_data + i;
    return i;

}


void File::Write(unsigned int _n, const char * _buf) {
    Console::puts("writing to file\n");
    int offset = 0;
    int count = _n;
    if(disk==NULL)
        assert(false);
    while(_n>0)
    {
            
            if(curr_w==0)
            {
                unsigned char dest[_n+1];
                memcpy(dest,_buf+offset,_n);
                int x = File::position+1;
                blockAllocated[++last_pos] = x;
                File::position = x;
                blockStatus[blockAllocated[last_pos]] = true; 

                disk->write(blockAllocated[last_pos],dest);
                if(_n>512)
                _n = _n -512;
                else
                {curr_w = curr_w + _n;_n=0;}
            }
            else {
                //Console::puts("\n No Block allocated \n");
                unsigned char dest[512];
                unsigned char wMem[512];
                disk->read(blockAllocated[last_pos],dest);
               
                memcpy(wMem,dest,curr_w);

                if(curr_w+_n<512)
                {

                    memcpy(wMem + curr_w,_buf,_n);
                    
                    curr_w = curr_w + _n;
                    _n = 0;
                }
                else
                {
                    //Console::puts("\n Not enough menory in block \n");
                    memcpy(wMem+curr_w,_buf,512-curr_w);
                    offset = 512 - curr_w;                    
                    _n = _n - (512-curr_w);
                    curr_w = 0;

                }
                
                disk->write(blockAllocated[last_pos],wMem);
                
                
            }
    }
 /*           
 Console::puts("Block Number: ");
 Console::puti(blockAllocated[last_pos]);
 Console::puts("\n");*/
        write_data = write_data + count;
    
}

void File::Reset() {
    Console::puts("reset current position in file\n");
    read_data = 0;
    curr = 0;
    read_pointer = 0;
   // assert(false);
    
}

void File::Rewrite() {
    Console::puts("erase content of file\n");
    for (int i=25;i<last_pos;i++)
    {
        blockStatus[blockAllocated[i]] = false;
        blockAllocated[i] = -1;
    }
    curr_w = 0;
    last_pos = -1;
    write_data = 0;
    //assert(false);
}


bool File::EoF() {
    Console::puts("testing end-of-file condition\n");
    assert(false);
}
