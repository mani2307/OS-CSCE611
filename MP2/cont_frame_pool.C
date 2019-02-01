/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.l/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/
ContFramePool * ContFramePool::poolListHead;
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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // assert(false);
    // Bitmap must fit in a single frame!
    assert(_n_frames <= FRAME_SIZE * 8);
    
    base_frame_no = _base_frame_no;
    nframes = _n_frames;
    nFreeFrames = _n_frames;
    info_frame_no = _info_frame_no;
     // If _info_frame_no is zero then we keep management info in the first
    //frame, else we use the provided frame to keep management info
    if(info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE);
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }
    
    // Number of frames must be "fill" the bitmap!
    assert ((nframes % 8 ) == 0);
    
    
    // Everything ok. Proceed to mark all bits in the bitmap
    for(int i=0; i*8 < _n_frames*2; i++) {
        bitmap[i] = 0xFF;
    }
    unsigned long frame_no = base_frame_no;
    unsigned long start = 0;
    unsigned long count = 0;
    // Mark the first frame as being used if it is being used
    
    
    if(_info_frame_no == 0) {
        _info_frame_no = base_frame_no;
        _n_info_frames = 1;
    }
    	
	while(start < nframes*2 && _n_info_frames>0 && base_frame_no<=_info_frame_no)
	{
		unsigned char mask = 0xC0;
	    for(int i =7;i>0;i=i-2)
	    {

	        if(frame_no >= _info_frame_no)
	        {
                bitmap[start] = bitmap[start] ^ mask;
                nFreeFrames--;
                count ++;
	        }
	        mask = mask >>2;
	        if(count==_n_info_frames)
	    	    break;
	    	    	        frame_no ++;
	    }
	    start++;
	    if(count==_n_info_frames)
	    	break;

    }
    
    if(ContFramePool::poolListHead==NULL)
        ContFramePool::poolListHead = this;
    else
    {   
        ContFramePool *temp = poolListHead;

       ContFramePool::poolListHead = this;
        ContFramePool::poolListHead->poolListNext = temp;     
    
    }
         // printBitmap();
    Console::puts("Frame Pool initialized\n");

}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    // TODO: IMPLEMENlTATION NEEEDED!
    // Any frames left to allocate?
    // assert(nFreeFrames > 0);
    if (nFreeFrames < 0) 
    return 0;
    
    // Find a frame that is not being used and return its frame index.
    // Mark that frame as being used in the bitmap.
    unsigned long frame_no = base_frame_no;
    unsigned long starting_frame = base_frame_no;
    
    unsigned long i = 0;
	unsigned long frame_count=0;
    unsigned long new_start=0;
    unsigned long offset=-1;	
	unsigned long start = 0;
	
	// printBitmap();

	while(start < nframes*2)
	{
		unsigned char temp_mask = 0xC0;
	    for(int i =7;i>0;i=i-2)
	    {
//	    Console::puti(start);
//	        Console::puts(" ");
//	        Console::puti(int(bitmap[start]));
	        if(((bitmap[start] & temp_mask) >> i-1) == 0x03)
	            {frame_count++;}
	        else
	        {
	            frame_count = 0;
	            new_start = -1;
	            offset = -1;
	        }
	        temp_mask = temp_mask >> 2; 
	        	    //Console::puti(temp_mask);
	        if(frame_count==1)
	        {
	            starting_frame = frame_no;
	            new_start = start;
	            offset = i;
	        }
	        
	        if (frame_count==_n_frames)
	            break;
            
            frame_no ++;
	    }
	   if (frame_count==_n_frames)
	            break;
	   start ++;
	}
	
	if(frame_count!=_n_frames)
	{
	    return 0;
	}
    nFreeFrames = nFreeFrames - _n_frames ;
    
    // Update bitmap
    start = new_start;
    frame_count = 0;
	bitmap[start] = bitmap[start] | (0x1 << offset); //head sequence 01, since xor it will make 10
    bitmap[start] = bitmap[start] & ~(0x1 << offset-1);
    
    unsigned long mask = 0x80 >> (7-offset); // -2 to neglect head	
	while(frame_count<_n_frames-1)
	{
		mask = mask >>2;
	    if (mask==0)
	    {
		    mask = 0x80;
		    start++;
	    }  
        bitmap[start] = bitmap[start] ^ mask;
	    frame_count ++;
	}
	
    // printBitmap();
	
	return starting_frame;
//    assert(false);
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
        // Mark all frames in the range as being used.
    
    unsigned long frame_no = base_frame_no;
    unsigned long start = (_base_frame_no - frame_no)/4 ;
    
    // update frame_no
    frame_no = frame_no + start*4;
    unsigned long count = 0;
    
    while(start < nframes*2)
	{
	    for(int i =7;i>0;i=i-2)
	    {
	        if(frame_no >= _base_frame_no)
	        {
                bitmap[start] = bitmap[start] & ~(0x1 << i); 
                bitmap[start] = bitmap[start] & ~(0x1 << i-1);
                nFreeFrames--;
                count ++;
	        }
	        if(count==_n_frames)
	    	    break;
	    	frame_no ++;
	    }
	    start++;
	    if(count==_n_frames)
	    	break;

    }
    // assert(false);
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
 //   Console::puti(_first_frame_no);
 //   Console::puts(" ");
    ContFramePool *current = ContFramePool::poolListHead;

    while(current!=NULL)
    {
        if(current->base_frame_no + current->nframes >= _first_frame_no && _first_frame_no >= current->base_frame_no)
        break;
        
        current = current->poolListNext;
    }
    
    if(current==NULL)
        {
         	Console::puts ("  Pool not found  ");   
            assert(false);
        }
    bool releaseFlag = false;
    unsigned long frame_no = current->base_frame_no;
    unsigned long start = (_first_frame_no - frame_no)/4 ;

    // update frame_no
    frame_no = frame_no + start*4;
    unsigned long count = 0;
    unsigned char * bitmap = current->bitmap;

    //mark frames as free
    while(start < current->nframes*2)
	{
	    for(int i =7;i>0;i=i-2)
	    {
	        if(frame_no == _first_frame_no)
	        {
	            bool check1 = ((bitmap[start] &  (0x1<<i)) == 0x00);	 	
	                
	            bool check2 = ((bitmap[start] &  (0x1<<i-1)) != 0x00);	 	
	            
	            if (check1 || check2)
	            {
	                Console::puts ("  Not a head of sequence  ");
	                assert(false); // not a head of sequence
	            }
	            
                bitmap[start] = bitmap[start] | (0x1 << i); 
                bitmap[start] = bitmap[start] | (0x1 << i-1);
                current->nFreeFrames++;
  	        }
	        else if (frame_no > _first_frame_no)
	        {      bool check1 = (bitmap[start] &  (1<<i)) != 0x00;	 	
	                
	               bool check2 = (bitmap[start] &  (1<<i-1)) == 0x00;	 	
	               
	            if (check1 ||check2)
	            {
	                releaseFlag = true;
	                break; // frames finished for the pool
	            }
	            else 
	            {
                    bitmap[start] = bitmap[start] | (0x1 << i); 
                    bitmap[start] = bitmap[start] | (0x1 << i-1);
                    current->nFreeFrames++;
      	        }
	        }
	        frame_no ++;
            
	    }
	    if(releaseFlag)
    	    break;
    
	    start++;

    }
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    return (_n_frames / (16*1024) + (_n_frames%(16*1024) > 0 ? 1 : 0));
}
