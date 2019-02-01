/*
     File        : blocking_disk.H

     Author      : 

     Date        : 
     Description : 

*/

#ifndef _MIRRORED_DISK_H_
#define _MIRRORED_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"
#include "thread.H"
#include "queue.H"
#include "scheduler.H"
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* B l o c k i n g D i s k  */
/*--------------------------------------------------------------------------*/

extern Scheduler * SYSTEM_SCHEDULER;

class MirroredDisk{
public:
   Queue *queue;
   SimpleDisk *slave;
   SimpleDisk *master;
   
   MirroredDisk(DISK_ID master, DISK_ID slave , unsigned int _size); 
   /* Creates a BlockingDisk device with the given size connected to the 
      MASTER or SLAVE slot of the primary ATA controller.
      NOTE: We are passing the _size argument out of laziness. 
      In a real system, we would infer this information from the 
      disk controller. */

   /* DISK OPERATIONS */
     /* Return true if disk is ready to transfer data from/to disk, false otherwise. */
    bool is_ready() {
       return slave->is_ready() || master->is_ready();
    }
    
    void wait_until_ready() {
        if (!is_ready()) { 
             Console::puts("\n Inside Mirrored disk wait..\n");
     
            add(Thread::CurrentThread()); 
           
            Console::puts("\n Size of Disk queue Inside wait: ");
        Console::puti(queue->size());
        Console::puts("\n");
           
            SYSTEM_SCHEDULER->yield();
           

        
        }
     }
     
   void add(Thread * _thread);
   
    void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */

    void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

};

#endif
