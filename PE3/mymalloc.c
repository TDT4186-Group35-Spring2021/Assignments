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
//  Determines whether right follows directly after left in memory, i.e, there are no other blocks between left and right and handles the situation.
//  If the bytes used for the left block and its size is only smaller than rights address by 16 or less, there can exist no other block in between
//  16 is used as the number of bytes allocated is divisible by 8 while mem_control_blocks need 12 bytes. Therefore the compiler always leaves 16 bytes between
//  the end of the memory allocated and the begining of the memory used for storing right. 16 is therefore the larges gap there can exist between two neighbors
    if ((heap_offset(right) - ((heap_offset(left) + left->size))) <= 16) {
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
    printf("\nTrying to remove %d", heap_offset(block));
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
        printf("\n\n\nERROR! Address %d is outside the heap\n\n\n", heap_offset(block));
        return;
    }
    
    struct mem_control_block *current_block = free_list_start;
    
    if (current_block == NULL) {
//      There are currently no free block, thus block must become the first element in the free list
        printf("\nAdding to free list : %d", heap_offset(block));
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
//          Current block is the last element in the free list and has an address starting before block
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
    printf("\n\nERROR! Could not add block %d", heap_offset(block));
}


void *mymalloc(long numbytes) {
    if (has_initialized == 0) {
        mymalloc_init();
    }
    

    /* add your code here! */
    
    //Assuring space allocated is sufficient while divisible by 8. A little hairy, but nothing beats branchless programming :)
    numbytes += ((numbytes % 8) != 0) * (8-(numbytes % 8));
    
    struct mem_control_block *current_block = free_list_start;
    
    do {
        if (current_block->size >= numbytes + sizeof(struct mem_control_block)) {
            
            printf("\nSize of current block: %d, i.e, large enough for %d", current_block->size, (int) numbytes);
            
            /*Calculating the address in memory for the new memory block to come after the allocated section.
             Finding such an address is accomplished through casting the hex value represented in the pointer to the
             current block to a long. Then we simply add numbytes, the size of the area occupied by current_block
             to find the first byte not occupied */
            long new_address = (long) current_block + numbytes + (long) sizeof(struct mem_control_block);
            
            
//          This is a pointer to the starting byte at which the new mem_control_block element will be stored
            struct mem_control_block *new_block = (struct mem_control_block *) new_address;

            if (current_block->next != NULL) {
//              The size of new_block is the range of bytes between it and the block currently being pointed to by current_block minus the size of new_block
                new_block->size = heap_offset(current_block->next) - (heap_offset(new_block) + sizeof(struct mem_control_block));
//              When inserting a new block between two existing ones we need new_block to point to the next mem_control_block
                new_block->next = current_block->next;
            }
            
            else {
//              Here the size for new_block is simply the remainder of the heap minus the size of new_block
                new_block->size = MEM_SIZE - heap_offset(new_block) - sizeof(struct mem_control_block);
            }
            
//          The size of memory controlled by current_block is now memory used for itself and the number of bytes of allocated
            current_block->size = (int) numbytes + sizeof(struct mem_control_block);
            
//          As current block is no longer free and is removed from the free list
            remove_from_free_list(current_block);
            
//          Adding new block to the free llist
            add_to_free_list(new_block);
//
            printf("\nMemory allocation completed\ncurrent_block size: %d\n\n", current_block->size);
            return current_block;
        }
        
        printf("\nSize of current block: %p at size %d, i.e, not large enough for %d", current_block, current_block->size, (int) numbytes);
        
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

//  Starting to allocate memory:
    
    printf("Allocating first section");
    void *first = mymalloc(32000);
    printf("Allocating second section");
    void *second = mymalloc(16000);
    printf("Allocating third section");
    void *third  = mymalloc(8000);
    printf("Allocating fourth section");
    void *fourth = mymalloc(8000);
//  Have now used up 64,000 + 60 (five mem_control_blocks) = 64,060. Should fail as 64,060 + 1,600 + 12 = 65,672 > 1024 * 64 = 65,536
    printf("Allocating fifth section, this should fail");
    void *fifth = mymalloc(1600);
    
//  Now trying to free up allocated memory:
    printf("\n\nTrying to free the first allocated block of memmory locking down 32,000 bytes");
    myfree(first);
    printf("\n\nfirst should be the head of free list, its size is %d",  free_list_start->size);
        
    printf("\n\nTrying to free the second allocated block of memmory locking down 16,000 bytes");
    myfree(second);
    printf("\n\nThe size of first should have increased by the size of second, its size is %d", free_list_start->size);
    
    printf("\n\nTrying to free the third allocated block of memmory locking down 8,000 bytes");
    myfree(third);
    printf("\n\nThe size of first should have increased by the size of third, its size is %d", free_list_start->size);
       
    printf("\n\nTrying to free the fourth allocated block of memmory locking down 8,000 bytes");
    myfree(fourth);
    printf("\n\nThe size of first should now equal that of the heap as fourth was the last allocated block of memory, its size is %d", free_list_start->size);
}
