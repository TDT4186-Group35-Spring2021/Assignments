#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int has_initialized = 0;

// our memory area we can allocate from, here 64 kB
#define MEM_SIZE (64*1024)
uint8_t heap[MEM_SIZE];

// start and end of our own heap memory area
void *managed_memory_start;

// this block is stored at the start of each free and used block
struct mem_control_block {
    int size;
    struct mem_control_block *next;
};

// pointer to start of our free list
struct mem_control_block *free_list_start;


void mymalloc_init() {

    // our memory starts at the start of the heap array
    managed_memory_start = &heap;

    // allocate and initialize our memory control block
    // for the first (and at the moment only) free block
    struct mem_control_block *m = (struct mem_control_block *)managed_memory_start;
    m->size = MEM_SIZE - sizeof(struct mem_control_block);

    // no next free block
    m->next = (struct mem_control_block *)0;

    // initialize the start of the free list
    free_list_start = m;

    // We're initialized and ready to go
    has_initialized = 1;
}



//-------------------- Some utility functions --------------------

int heap_offset(struct mem_control_block *block) {
//    Provides the offset from the start of the heap, makes debugging simpler
    return (int) ((long) block - (long) &heap);
}


int address_ok(struct mem_control_block *block) {
//  Asserts whether block is within the heap
    return (heap_offset(block) >= 0) * (heap_offset(block) < MEM_SIZE -sizeof(struct mem_control_block));
}


int are_neighbors(struct mem_control_block *left, struct mem_control_block *right) {
    /*Considering from the start of left there is 12 bytes for left an allocated block whose size is atleast 8, and 4 free bytes
    between the end of the allocated memory and the begining of right as mem control blocks require 12 bytes (one integer and pointer),
    which implies the address at which the next memory control block is stored will be 4 above as such an increase is required when the original
    nuumber (end of allocated memory) is divisible by 8. Thus, no other memory block can exist between two blocks seperated by 24 bytes.
    
    This will waste available memory, especially if many small blocks are allocated. However, this is carried out by
    the compiler, hence we are currently unaware if any remedies exists, if it should be addressed in the first place...*/
    if ((heap_offset(right) - ((heap_offset(left) + left->size))) <= 24) {
        left->size += right->size;
        left->next = right->next;
        return 1;
    }
    return 0;
}


void remove_from_free_list(struct mem_control_block *block) {
    
    if (!address_ok(block)) {
        printf("\n\n\nERROR! Address %d is outside the heap\n\n\n", heap_offset(block));
        return;
    }
//    printf("\nTrying to remove %d", heap_offset(block));
    struct mem_control_block *current_block = free_list_start;
    
    if (free_list_start == NULL) {
//      There are no free blocks, i.e, none can be removed
        return;
    }
    
    while (current_block != NULL) {
        
        if (current_block == block) {
//          This will only happen when trying to remove the first free memory control block
            free_list_start = current_block->next;
            current_block->next = NULL;
            return;
        }
        
        if (current_block->next == block) {
            if (current_block->next->next != NULL) {
//              As the next block after current is block which is to be removed, we must instead point to the free block after block
                current_block->next = current_block->next->next;
                current_block->next->next = NULL;
            }
            else {
                current_block->next = (void *)0;
            }
            return;
        }
        current_block = current_block->next;
    }
    printf("\n\nThe memory control block to be removed could not be found\n\n");
}


void add_to_free_list(struct mem_control_block *block) {
    
    if (!address_ok(block)) {
        printf("\n\n\nERROR! Address %p is outside the heap\n\n\n", block);
        return;
    }
    
    struct mem_control_block *current_block = free_list_start;
    
    if (current_block == NULL) {
//      There are currently no free block, thus block must become the first element in the free list
        free_list_start = block;
        return;
    }
    
    if (heap_offset(block) < heap_offset(current_block)) {
//      block is situated before the start of the free list, it must therefore become the new head of free list
        if (!are_neighbors(block, free_list_start)) {
            block->next = current_block;
        }
        free_list_start = block;
        return;
    }
    
    while (heap_offset(block) > heap_offset(current_block)) {
        if (current_block->next == NULL) {
//          current_block is the last element in the free list and has an address starting before block
            if (!are_neighbors(current_block, block)) {
                current_block->next = block;
            }
            return;
        }
        else if (heap_offset(current_block) < heap_offset(block) && heap_offset(current_block->next) > heap_offset(block)) {
            
//          Lets current_block->next become part of block if neighbour with block
            if (!are_neighbors(block, current_block->next)) {
                block->next = current_block->next;
            }
            
//          Lets block become part of current_block if neighbour with block
            if (!are_neighbors(current_block, block)) {
                current_block->next = block;
            }
            return;
        }
        current_block = current_block->next;
    }
    printf("\n\nERROR! Could not add block %p", block);
}



//-------------------- mymalloc and myfree -------------------

void *mymalloc(long numbytes) {
    if (has_initialized == 0) {
        mymalloc_init();
    }
    /* add your code here! */
    
    if (numbytes < 1) {
        printf("\n...\n");
        return NULL;
    }
    //Assuring space allocated is sufficient while divisible by 8. A little hairy, but nothing beats branchless programming :)
    numbytes += ((numbytes % 8) != 0) * (8-(numbytes % 8));
    
    struct mem_control_block *current_block = free_list_start;
    
    do {
        if (current_block->size >= numbytes + sizeof(struct mem_control_block)) {
            /*Calculating the address in memory for the new memory block to come after the allocated section.
             Finding such an address is accomplished through casting the hex value represented in the pointer to the
             current block to a long. Then we simply add numbytes, the size of the area occupied by current_block
             to find the first byte not occupied */
            long new_address = (long) current_block + numbytes + (long) sizeof(struct mem_control_block);
            
//          This is a pointer to the starting byte at which the new mem_control_block element will be stored
            struct mem_control_block *new_block = (struct mem_control_block *) new_address;

            if (current_block->next != NULL) {
//              The size of new_block is the range of bytes between it and the block currently being pointed to by current_block minus the size of new_block
                new_block->size = current_block->size - (int) numbytes - sizeof(struct mem_control_block);
//              When inserting a new block between two existing ones we need new_block to point to the next mem_control_block
                new_block->next = current_block->next;
            }
            
            else {
//              Here the size for new_block is simply the remainder of the heap minus the size of new_block
                new_block->size = MEM_SIZE - heap_offset(new_block) - sizeof(struct mem_control_block);
            }
            
//          The size of memory controlled by current_block is now memory used for itself and the number of bytes allocated
            current_block->size = (int) numbytes + sizeof(struct mem_control_block);
            
//          As current block is no longer free and is removed from the free list
            remove_from_free_list(current_block);
            
//          Adding new block to the free llist
            add_to_free_list(new_block);
            
            return current_block;
        }
        current_block = current_block->next;
    }
    while (current_block != NULL);
    
    printf("\nMemory allocation failed!\nNo available block of memory large enough...\n");
    
    return (void *)0;
}


void myfree(void *firstbyte) {

    /* add your code here! */
    add_to_free_list((struct mem_control_block *) firstbyte);
}


int main(int argc, char **argv) {

  /* add your test cases here! */
    
    printf("\n\nAvailable heap memory before allocation: %d\n\n", MEM_SIZE-16);
    
//  Starting to allocate memory:
    printf("\nStarting to allocate memory:\n");
    printf("Allocating first block\n");
    void *first = mymalloc(32000);
    printf("Allocating second block\n");
    void *second = mymalloc(16000);
    printf("Allocating third block\n");
    void *third  = mymalloc(8000);
    printf("Allocating fourth block\n");
    void *fourth = mymalloc(8000);
//  Have now used up 64,000 + 60 (five mem_control_blocks) = 64,060. Should fail as 64,060 + 1,600 + 12 = 65,672 > 1024 * 64 = 65,536
    printf("Allocating fifth block, this should fail...\n");
    void *fifth = mymalloc(1600);
    
//  Now trying to free up allocated memory:
    printf("\nStarting to free up memory:");
    printf("\n\nTrying to free the first allocated block of memmory locking down 32,000 bytes");
    myfree(first);
    printf("\nfirst should be the head of free list, head size is %d",  free_list_start->size);
        
    printf("\n\nTrying to free the second allocated block of memmory locking down 16,000 bytes");
    myfree(second);
    printf("\nThe size of the head should have increased by the size of second, its size is %d", free_list_start->size);
    
    printf("\n\nTrying to free the third allocated block of memmory locking down 8,000 bytes");
    myfree(third);
    printf("\nThe size of the head should have increased by the size of third, its size is %d", free_list_start->size);
       
    printf("\n\nTrying to free the fourth allocated block of memmory locking down 8,000 bytes");
    myfree(fourth);
    printf("\nThe size of the head should now equal that of the heap as fourth was the last allocated memory-block, head size is %d\n", free_list_start->size);
    
//  Allocating and removing
    printf("\nAllocating and removing memory:");
    first = mymalloc(8192);
    printf("\nfirst address  : %p", first);
    second = mymalloc(16384);
    printf("\nsecond address : %p", second);
    printf("\nHaving allocated both first and second, the head of the free list has the size of %d at address %p\n", free_list_start->size, free_list_start);
    myfree(first);
    printf("\nNow that first is free, it should become the head of free list %p with the size of %d, while the second element is the previous head %p\n", free_list_start, free_list_start->size, free_list_start->next);
    third  = mymalloc(4096);
    printf("\nthird address  : %p", third);
    printf("\nAs first was freed, third now occupies some of that space (1/2), the head will now have been pushed 'upwards' %p, while its size is reduced to %d\n", free_list_start, free_list_start->size);
    fourth = mymalloc(8192);
    printf("\nfourth address : %p", fourth);
    fifth = mymalloc(24576);
    printf("\nfifth address  : %p", fifth);
    myfree(third);
    myfree(fifth);
    printf("\n\nAfter freeing third and fifth we should have two free blocks in our heap.\nfree_list head : %p\nfree list tail : %p\ntail->next : %p (should be null).\nThese addresses should match those of first and third as the head, and fifth as the tail", free_list_start, free_list_start->next, free_list_start->next->next);
}
