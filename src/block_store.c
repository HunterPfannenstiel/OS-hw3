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

    //Will need to store arbitrary data (void types)
    //Will need to store the data in blocks of bytes (if user wants to store more than a byte they will need to do multiple requests)
  struct block_store
  {
    block_t* store;
  };
  

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

/*This function creates a new block store and returns a pointer to it. 
It first allocates memory for the block store and initializes it to zeros using the memset 
Then it sets the bitmap field of the block store to an overlay of a bitmap with size BITMAP_SIZE_BITS on the blocks starting at index BITMAP_START_BLOCK. 
Finally, it marks the blocks used by the bitmap as allocated using the block_store_request function.*/
block_store_t *block_store_create()
{
    block_store_t* block_store = (struct block_store*)malloc(sizeof(block_store));
    block_store->store = malloc(sizeof(block_t) * BLOCK_STORE_NUM_BLOCKS);
    memset(block_store->store, 0, BLOCK_STORE_NUM_BLOCKS);
    bitmap_t* free_map = bitmap_create(BITMAP_SIZE_BITS);
    block_t *block = &(block_store->store[127]);
    memcpy(block->data, free_map, BLOCK_SIZE_BYTES);
    bitmap_destroy(free_map);
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

bool block_store_request(block_store_t *const bs, const size_t block_id)
{
    UNUSED(bs);
    UNUSED(block_id);
    return false;
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