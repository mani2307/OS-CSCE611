
/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/
#define GB * (0x1 << 30)
#define MB * (0x1 << 20)
#define KB * (0x1 << 10)

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "page_table.H"
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
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
    
    base_address = _base_address;
    size = _size;

    frame_pool = _frame_pool;
    page_table = _page_table;
    
    page_table->register_pool(this);
    
    vmpool_frame = (unsigned long*)(base_address);
    pool_pointer = -1;

    Console::puts("Constructed VMPool object.\n");

}


unsigned long VMPool::allocate(unsigned long _size) {

    unsigned long pre_add = 0;
    
    int page_size = PageTable::PAGE_SIZE;
    
    int x_size = _size / (page_size);
    
    if (_size % (page_size)>0)
        x_size++; 
     
    if (pool_pointer<0)
    {
        pre_add = base_address + (4 KB) + 1;
    }
    else 
    {
        pre_add = vmpool_frame[pool_pointer];
    }

    unsigned long new_add = pre_add + x_size*(4 KB);
    
    unsigned long limit= base_address + size ;
    
    if( (new_add<limit) && (new_add>=(base_address)))
    {
     
        vmpool_frame[++pool_pointer] = pre_add;
        vmpool_frame[++pool_pointer] = new_add;               
    
        return pre_add;
    }

    else
    {
      Console::puts("Address out of range for this segment....");
      assert(false);
      
    }
}

void VMPool::release(unsigned long _start_address) {
    // assert(false);
    
    int pos = 0;
    for (int i =0;i<=pool_pointer;i=i+2)
    {
        if (vmpool_frame[i]==_start_address)
        {pos = i;break;}
    
    }

    unsigned long  start = vmpool_frame[pos];
    unsigned long  end = vmpool_frame[pos+1];
    unsigned long int size = end - start;

    int page_size = PageTable::PAGE_SIZE;
    
    for (int i = 0;i<(size/page_size);i++)
    {
        page_table->free_page(start);
        start = start + page_size;
    }
    
    page_table->load();
    
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {

    //Console::puts("Checked whether address is part of an allocated region.\n");
    
    
    unsigned long limit= base_address + size;
    
    if( (_address<limit) && (_address>=base_address))
       return true;
    else
       return false;
    
}

