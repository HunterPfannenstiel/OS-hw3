#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bitmap.h"
#include "block_store.h"
// include more if you need

typedef struct block 
{
    char data[BLOCK_SIZE_BYTES]; //Even though it is a char type it can be interpreted as any type by using the mem functions (char type is 1 byte)
} block_t;

//Will need to store the data in blocks of bytes (if user wants to store more than a byte they will need to do multiple requests)
  struct block_store
  {
    block_t* store;
    bitmap_t* bitmap_overlay;
  };
  

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

void print_bytes(void* data, size_t num_bytes);

void print_bytes(void* data, size_t num_bytes)
{
    char* bytes = (char*)data;
    for(size_t i = 0; i < num_bytes; i++)
    {
        printf("%02X", bytes[i]);
    }
    printf("\n");
}

/*This function creates a new block store and returns a pointer to it. 
It first allocates memory for the block store and initializes it to zeros using the memset 
Then it sets the bitmap field of the block store to an overlay of a bitmap with size BITMAP_SIZE_BITS on the blocks starting at index BITMAP_START_BLOCK. 
Finally, it marks the blocks used by the bitmap as allocated using the block_store_request function.*/
block_store_t *block_store_create()
{
    block_store_t* block_store = (struct block_store*)malloc(sizeof(block_store));
    if(block_store == NULL)
    {
        return NULL;
    }
    int store_size = sizeof(block_t) * BLOCK_STORE_NUM_BLOCKS;
    block_store->store = malloc(store_size);
    memset(block_store->store, 0, store_size);

    block_store->bitmap_overlay = bitmap_overlay(BITMAP_SIZE_BITS, block_store->store[BITMAP_START_BLOCK].data); //Might have to remove the block_t type if bitmap would take more than 1 block

    return block_store;
}

void block_store_destroy(block_store_t *const bs)
{
    UNUSED(bs);
}
size_t block_store_allocate(block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

/*This function marks a specific block as allocated in the bitmap. 
It first checks if the pointer to the block store is not NULL and if the block_id is within the range of valid block indices. 
If the block is already marked as allocated, it returns false. 
Otherwise, it marks the block as allocated and checks that the block was indeed marked as allocated by testing the bitmap. 
It returns true if the block was successfully marked as allocated, false otherwise.*/
bool block_store_request(block_store_t *const bs, const size_t block_id)
{
    if(bs == NULL || block_id >= BLOCK_STORE_NUM_BLOCKS) //*block_id is unsigned (don't check less than 0)
    {
        return false;
    }
    bitmap_t* overlay = bs->bitmap_overlay;
    if(bitmap_test(overlay, block_id))
    {
        return false;
    }
    bitmap_set(overlay, block_id);
    if(!bitmap_test(overlay, block_id))
    {
        return false;
    }
    return true;
}

void block_store_release(block_store_t *const bs, const size_t block_id)
{
    UNUSED(bs);
    UNUSED(block_id);
}

size_t block_store_get_used_blocks(const block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

size_t block_store_get_free_blocks(const block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

size_t block_store_get_total_blocks()
{
    return 0;
}

size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer)
{
    UNUSED(bs);
    UNUSED(block_id);
    UNUSED(buffer);
    return 0;
}

size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer)
{
    UNUSED(bs);
    UNUSED(block_id);
    UNUSED(buffer);
    return 0;
}

block_store_t *block_store_deserialize(const char *const filename)
{
    UNUSED(filename);
    return NULL;
}

size_t block_store_serialize(const block_store_t *const bs, const char *const filename)
{
    UNUSED(bs);
    UNUSED(filename);
    return 0;
}