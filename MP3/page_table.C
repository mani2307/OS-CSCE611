#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

#define MB * (0x1 << 20)
#define KB * (0x1 << 10)

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   //assert(false);
   
   PageTable::kernel_mem_pool = _kernel_mem_pool;
   PageTable::process_mem_pool = _process_mem_pool;
   PageTable::shared_size = _shared_size;
   
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   //assert(false);
   
   int n_frames = 1;
   
   unsigned long frame = PageTable::kernel_mem_pool->get_frames(n_frames);
   
   page_directory = (long unsigned int*)(frame * (4 KB));        
   
   PageTable::current_page_table = this;

   frame = PageTable::kernel_mem_pool->get_frames(1);
   

   unsigned long address = 0; // holds the physical address of where a page is
   unsigned int i;
   
   unsigned long *page_table = (unsigned long *)(frame * (4 KB));
    // map the first 4MB of memory

   unsigned int size= PageTable::shared_size / (4 KB);

   for(i=0; i<size; i++)
   {
        page_table[i] = address | 0X03; // attribute set to: supervisor level, read/write, present
        address = address + 4096; // 4096 = 4kb
   }
   
   page_directory[0] = (unsigned long int)page_table; // attribute set to: supervisor level
   page_directory[0] = page_directory[0] | 0x03;   
   
   for(i=1; i<size; i++)
   {
      page_directory[i] = 0x00 | 0x02; // attribute set to: supervisor level, read/write, not present
   }

   Console::puts("Constructed Page Table object\n");



}


void PageTable::load()
{
   //assert(false);

   write_cr3((unsigned long int)page_directory); // put that page directory address into CR3

   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   //assert(false);
   
   write_cr0(read_cr0() | 0x80000000); // set the paging bit in CR0 to 1
   PageTable::paging_enabled = 1;   
   
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  // assert(false);
    unsigned long int add = read_cr2();
    unsigned long int dir_add= (add & 0xffC00000)>>22;
    unsigned long int pt_add= (add>>12) & 0x3ff;

    unsigned long int diff = (dir_add); 

    if((PageTable::current_page_table->page_directory[diff] & 1) == 0)
    {
    //Console::puts("here ");
       unsigned long frame = PageTable::kernel_mem_pool->get_frames(1);
       unsigned long *page_table = (unsigned long *)(frame * (4 KB));

       PageTable::current_page_table->page_directory[diff] = ((unsigned long int)page_table) | 0x03;

    }


    unsigned long *page_table = (unsigned long int*)(PageTable::current_page_table->page_directory[diff] & 0XFFFFF000);

    unsigned long frame1 = PageTable::process_mem_pool->get_frames(1);

    if(frame1 == 0)
    {
     Console::puts("Frame NOT found.. Release the frame first\n");
     assert(false);
    }
    unsigned long  address = (frame1 * (4 KB));

    page_table[pt_add] = address;
    page_table[pt_add] = page_table[pt_add] | 0X03;

    Console::puts("handled page fault\n");
}

