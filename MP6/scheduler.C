/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "blocking_disk.H"
#include "mirrored_disk.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/
#define _MIRRORED_DISK_

#ifndef _MIRRORED_DISK_
    extern BlockingDisk * SYSTEM_DISK;

#else
    extern MirroredDisk * SYSTEM_DISK;
#endif

Scheduler::Scheduler() {
  	queue = new Queue(); 
	queue->front = queue->rear = NULL; 

  // assert(false);
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {

    QNode *q = queue->deQueue();

    Console::puts("\n Size of Disk queue: ");
        Console::puti(SYSTEM_DISK->queue->size());
        Console::puts("\n");
    if(SYSTEM_DISK->is_ready() && SYSTEM_DISK->queue->size()>0){
    
        QNode *t = SYSTEM_DISK->queue->deQueue();
        
        resume(t->key);
    
    Console::puts("\nwhile waiting in yield\n");
    }
    
    Thread::dispatch_to(q->key);

 // assert(false);
}

void Scheduler::resume(Thread * _thread) {
   
    queue->enQueue(_thread);
}

void Scheduler::add(Thread * _thread) {
    
    queue->enQueue(_thread);
}

void Scheduler::terminate(Thread * _thread) {
        struct QNode *temp = queue->front;
        Console::puts("Terminating");
        int i = 0;
        int count = queue->size();
        while(i < count)
        {
            if(temp->key == _thread)
            {
             QNode *q = queue->deQueue();   
            }
            else
            {   
                QNode *q = queue->deQueue(); 
                queue->enQueue(temp->key);
            }
            temp = temp->next;
            i++;
        }
        
}
