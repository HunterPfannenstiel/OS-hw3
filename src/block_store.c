#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bitmap.h"
#include "block_store.h"
// include more if you need

typedef struct block 
{
    char data[BLOCK_SIZE_BYTES]; //Even though it is a char type it can be interpreted as any type (char type is 1 byte)
} block_t;

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

bool block_id_in_range(size_t block_id);

bool block_id_in_range(size_t block_id)
{
    return block_id < BLOCK_STORE_NUM_BLOCKS;
}

block_store_t *block_store_create()
{
    block_store_t* block_store = (block_store_t*)malloc(sizeof(block_store_t));
    if(block_store == NULL)
    {
        return NULL;
    }
    block_store->store = malloc(BLOCK_STORE_NUM_BYTES);// sizeof(block_t) = BLOCK_SIZE_BYTES, BLOCK_STORE_NUM_BYTES = BLOCK_SIZE_BYTES * BLOCK_STORE_NUM_BLOCKS
    if(block_store->store == NULL)
    {
        return false;
    }
    memset(block_store->store, 0, BLOCK_STORE_NUM_BYTES);

    block_store->bitmap_overlay = bitmap_overlay(BITMAP_SIZE_BITS, block_store->store[BITMAP_START_BLOCK].data);

    return block_store;
}

void block_store_destroy(block_store_t *const bs)
{
    if(bs != NULL)
    {
        bitmap_destroy(bs->bitmap_overlay);
        free(bs->store);
        free(bs);
    }
}
size_t block_store_allocate(block_store_t *const bs)
{
    if(bs == NULL)
    {
        return SIZE_MAX;
    }
    return bitmap_ffz(bs->bitmap_overlay);
}

bool block_store_request(block_store_t *const bs, const size_t block_id)
{
    if(bs == NULL || !block_id_in_range(block_id)) //*block_id is unsigned (don't check less than 0)
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