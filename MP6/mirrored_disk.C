/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "mirrored_disk.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

MirroredDisk::MirroredDisk(DISK_ID _disk_id, DISK_ID _disk_id1 ,unsigned int _size)  {


    queue = new Queue(); 
	queue->front = queue->rear = NULL; 
	master = new SimpleDisk(MASTER,_size);
    slave = new SimpleDisk(SLAVE,_size);
    
  Console::puts("Constructed Mirrored Blocking Disk..n");

}

/*--------------------------------------------------------------------------*/
/* DISK CONFIGURATION */
/*--------------------------------------------------------------------------*/

void MirroredDisk::read(unsigned long _block_no, unsigned char * _buf) {
/* Reads 512 Bytes in the given block of the given disk drive and copies them 
   to the given buffer. No error check! */

  //master->read(_block_no, _buf);
  master->issue_operation(READ, _block_no);
  //slave->read(_block_no, _buf);
  slave->issue_operation(READ, _block_no);
  
  wait_until_ready();
  
  int i;
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = Machine::inportw(0x1F0);
    _buf[i*2]   = (unsigned char)tmpw;
    _buf[i*2+1] = (unsigned char)(tmpw >> 8);
  }

  
}

void MirroredDisk::write(unsigned long _block_no, unsigned char * _buf) {
/* Writes 512 Bytes from the buffer to the given block on the given disk drive. */
  //master->write(_block_no, _buf);
  //slave->write(_block_no, _buf);
  
    master->issue_operation(WRITE, _block_no);

  wait_until_ready();

  /* write data to port */
  int i; 
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }

  slave->issue_operation(WRITE, _block_no);

  wait_until_ready();

  /* write data to port */
  //int i; 
  //unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }


}

void MirroredDisk::add(Thread * _thread) {
    
    queue->enQueue(_thread);
}
