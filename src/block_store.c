#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bitmap.h"
#include "block_store.h"
// include more if you need

struct block_store
{
    char* store; //The storage for the block_store. A char is stored as one byte
    bitmap_t* bitmap_overlay; // The bit map overlay for the bit map stored in the block_store's store
};

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

///
/// Checks if the block_id is within the range of the store
/// \param block_id The block id to check
/// \return A bool denoting whether block_id is in range or not
///
bool block_id_in_range(size_t block_id);

bool block_id_in_range(size_t block_id)
{
    return block_id < BLOCK_STORE_NUM_BLOCKS; //Return true if block_id is less than the number of blocks in the store. *block_id is unsigned (don't worry about negatives)
}

///
/// Gets the starting index into the store that block_id represents
/// \param block_id The block id to get the index for
/// \return The starting index of block_id
///
size_t get_block_id_index(size_t block_id);

size_t get_block_id_index(size_t block_id)
{
    return block_id * BLOCK_SIZE_BYTES; // Each block is BLOCK_SIZE_BYTES so multiplying it by the block_id will give the correct offset (index).
}

block_store_t *block_store_create()
{
    block_store_t* block_store = (block_store_t*)malloc(sizeof(block_store_t)); //Allocate memory for the block store
    if(block_store == NULL)
    {
        return NULL; // Return NULL if memory wasn't able to be allocated
    }
    block_store->store = malloc(BLOCK_STORE_NUM_BYTES); // Allocate memory for the block store's store

    memset(block_store->store, 0, BLOCK_STORE_NUM_BYTES); // Clear the store so its empty

    block_store->bitmap_overlay = bitmap_overlay(BITMAP_SIZE_BITS, block_store->store + get_block_id_index(BITMAP_START_BLOCK)); // Create a bitmap overlay where the bitmap is stored in the block starting at BITMAP_START_BLOCK

    for(int i = 0; i < BITMAP_NUM_BLOCKS; i++) // Iterate over the number of blocks the bitmap takes up
    {
        if(!block_store_request(block_store, BITMAP_START_BLOCK + i)) //If the block is not able to be requested
        {
            block_store_destroy(block_store); //Destroy the block store
            return NULL; //Return NULL because the bitmap couldn't be stored
        }
    }

    return block_store; //Return the block store after successful initialization
}

void block_store_destroy(block_store_t *const bs)
{
    if(bs != NULL) // If the block store is not NULL
    {
        bitmap_destroy(bs->bitmap_overlay); //Destroy the bit map
        free(bs->store); //Free the store
        free(bs); //Free the block store
    }
}

size_t block_store_allocate(block_store_t *const bs)
{
    if(bs == NULL)
    {
        return SIZE_MAX; // Return SIZE_MAX because the block store was NULL
    }
    size_t first_free_block = bitmap_ffz(bs->bitmap_overlay); // Find the first zero in the bitmap (the first free block)
    if(!block_store_request(bs, first_free_block))
    {
        return SIZE_MAX; //Return SIZE_MAX if requesting the block fails
    }
    return first_free_block; // Return the first free block after requesting it
}

bool block_store_request(block_store_t *const bs, const size_t block_id)
{
    if(bs == NULL || !block_id_in_range(block_id))
    {
        return false; // Return false if the block store is NULL or the block id is not in range of the store
    }
    bitmap_t* overlay = bs->bitmap_overlay; // Get the bitmap overlay
    if(bitmap_test(overlay, block_id))
    {
        return false; // Return false if the block id is already taken
    }
    bitmap_set(overlay, block_id); // Mark the block id as taken
    if(!bitmap_test(overlay, block_id))
    {
        return false; // Return false if the block id couldn't be set
    }
    return true; // Return true because the block id was requested successfully
}

void block_store_release(block_store_t *const bs, const size_t block_id)
{
    if(bs == NULL || !block_id_in_range(block_id))
    {
        return; // Return if block store is NULL or the block id is not in range of the store
    }
    bitmap_t* overlay = bs->bitmap_overlay; // Get the bitmap overlay
    bitmap_reset(overlay, block_id); // Mark the block as available (*don't have to clear the block's data because when a block is written to it will overwrite it because we always write 'BLOCK_SIZE_BYTES' bytes)
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