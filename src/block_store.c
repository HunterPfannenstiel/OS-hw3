#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bitmap.h"
#include "block_store.h"
// include more if you need
#include <unistd.h>
#include <fcntl.h>

struct block_store
{
    char* store; //Tried defining as 'char store[BLOCK_STORE_NUM_BYTES]' but would get: "double free or corruption (top): 0x00000000007ca6a0 ***"
    bitmap_t* bitmap_overlay;
};

bool block_id_in_range(size_t block_id);

bool block_id_in_range(size_t block_id)
{
    return block_id < BLOCK_STORE_NUM_BLOCKS; //*block_id is unsigned (don't worry about negatives)
}

size_t get_block_id_index(size_t block_id);

size_t get_block_id_index(size_t block_id)
{
    return block_id * BLOCK_SIZE_BYTES;
}

block_store_t *block_store_create()
{
    block_store_t* block_store = (block_store_t*)malloc(sizeof(block_store_t));
    if(block_store == NULL)
    {
        return NULL;
    }
    block_store->store = malloc(BLOCK_STORE_NUM_BYTES);

    memset(block_store->store, 0, BLOCK_STORE_NUM_BYTES);

    block_store->bitmap_overlay = bitmap_overlay(BITMAP_SIZE_BITS, block_store->store + get_block_id_index(BITMAP_START_BLOCK));

    for(int i = 0; i < BITMAP_NUM_BLOCKS; i++)
    {
        if(!block_store_request(block_store, BITMAP_START_BLOCK + i))
        {
            block_store_destroy(block_store);
            return NULL;
        }
    }

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
    size_t first_free_block = bitmap_ffz(bs->bitmap_overlay);
    if(!block_store_request(bs, first_free_block))
    {
        return SIZE_MAX;
    }
    return first_free_block;
}

bool block_store_request(block_store_t *const bs, const size_t block_id)
{
    if(bs == NULL || !block_id_in_range(block_id))
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
    if(bs == NULL || !block_id_in_range(block_id))
    {
        return;
    }
    bitmap_t* overlay = bs->bitmap_overlay;
    bitmap_reset(overlay, block_id);
}

size_t block_store_get_used_blocks(const block_store_t *const bs)
{
    if(bs == NULL)
    {
        return SIZE_MAX;
    }
    return bitmap_total_set(bs->bitmap_overlay);
}

size_t block_store_get_free_blocks(const block_store_t *const bs)
{
    if(bs == NULL)
    {
        return SIZE_MAX;
    }
    return BLOCK_STORE_NUM_BLOCKS - block_store_get_used_blocks(bs);
}

size_t block_store_get_total_blocks()
{
    return BLOCK_STORE_NUM_BLOCKS;
}

size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer)
{
    if(bs == NULL || !block_id_in_range(block_id) || buffer == NULL)
    {
        return 0;
    }
    int block_index = get_block_id_index(block_id);
    memcpy(buffer, bs->store + block_index, BLOCK_SIZE_BYTES);
    return BLOCK_SIZE_BYTES;
}

size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer)
{
    if(bs == NULL || !block_id_in_range(block_id) || buffer == NULL)
    {
        return 0;
    }
    int block_index = get_block_id_index(block_id);
    memcpy(bs->store + block_index, buffer, BLOCK_SIZE_BYTES);
    return BLOCK_SIZE_BYTES;
}

size_t index_to_block_id(int index);

size_t index_to_block_id(int index)
{
    return index % BLOCK_SIZE_BYTES;
}

block_store_t *block_store_deserialize(const char *const filename)
{
    if(filename == NULL)
    {
        return NULL;
    }
    int file_descriptor = open(filename, O_RDONLY); // Open the file with the name denoted by output_filename in write only mode
    if (file_descriptor < 0)
    {
        return NULL;
    }
    block_store_t* block_store = block_store_create();
    int read_bytes = read(file_descriptor, block_store->store, BLOCK_STORE_NUM_BYTES); // Read from the file into dst
    if(read_bytes != BLOCK_STORE_NUM_BYTES)
    {
        printf("Read bytes: %i\n", read_bytes);
        block_store_destroy(block_store);
        return NULL;
    }
    close(file_descriptor);
    for(int i = 0; i < BLOCK_STORE_NUM_BYTES; i++)
    {
        if(block_store->store[i] != 0x00)
        {
            size_t block_id = index_to_block_id(i);
            block_store_request(block_store, block_id);
        }
    }
    return block_store;
}

size_t block_store_serialize(const block_store_t *const bs, const char *const filename)
{
    if(bs == NULL)
    {
        return 0;
    }
    int file_descriptor = open(filename, O_WRONLY | O_CREAT); // Open the file with the name denoted by output_filename in write only mode
    if (file_descriptor < 0)
    {
        return 0;
    }
    size_t written_bytes = write(file_descriptor, bs->store, BLOCK_STORE_NUM_BYTES); // Write the data in src into the file
    close(file_descriptor);                                       // Close the file
    return written_bytes;
}