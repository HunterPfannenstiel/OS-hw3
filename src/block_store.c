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
    char* store; //The storage for the block_store. A char is stored as one byte
    bitmap_t* bitmap_overlay; // The bit map overlay for the bit map stored in the block_store's store
};


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

///
/// Gets the block id for the index
/// \param index The index in the block store
/// \return The starting block id of the index
///
size_t index_to_block_id(int index);

size_t index_to_block_id(int index)
{
    return index / BLOCK_SIZE_BYTES; // Divide the index by the number of bytes per block (integer division)
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
    if(bs == NULL)
    {
        return SIZE_MAX; // Return SIZE_MAX (denoting an error) if bs is NULL
    }
    return bitmap_total_set(bs->bitmap_overlay); // Return the number of set bits in the bitmap
}

size_t block_store_get_free_blocks(const block_store_t *const bs)
{
    if(bs == NULL)
    {
        return SIZE_MAX; // Return SIZE_MAX (denoting an error) if bs is NULL
    }
    return BLOCK_STORE_NUM_BLOCKS - block_store_get_used_blocks(bs); // Return the total blocks minus the used blocks
}

size_t block_store_get_total_blocks()
{
    return BLOCK_STORE_NUM_BLOCKS; // Return the total block constant
}

size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer)
{
    if(bs == NULL || !block_id_in_range(block_id) || buffer == NULL)
    {
        return 0; // Return 0 if the block store is NULL, the block being accessed is not in range, or the write buffer is NULL
    }
    int block_index = get_block_id_index(block_id); // Get the associated index for the block id
    memcpy(buffer, bs->store + block_index, BLOCK_SIZE_BYTES); // Starting at the block index in the block store, read one block worth of contents into the buffer
    return BLOCK_SIZE_BYTES; // Return the number of bytes read
}

size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer)
{
    if(bs == NULL || !block_id_in_range(block_id) || buffer == NULL)
    {
        return 0; // Return 0 if the block store is NULL, the block being accessed is not in range, or the read buffer is NULL
    }
    int block_index = get_block_id_index(block_id); // Get the associated index for the block id
    memcpy(bs->store + block_index, buffer, BLOCK_SIZE_BYTES); // Starting at the block index in the block store, write one block worth of contents into the store from the buffer
    return BLOCK_SIZE_BYTES; // Return the number of bytes written
}

block_store_t *block_store_deserialize(const char *const filename)
{
    if(filename == NULL)
    {
        return NULL; // Return NULL if the filename is NULL
    }
    int file_descriptor = open(filename, O_RDONLY, S_IRWXO | S_IRWXG | S_IRWXU); // Open the file with the name denoted by output_filename in read only mode (and with permissions)
    if (file_descriptor < 0)
    {
        return NULL; // Return NULL if the file wasn't able to be opened
    }
    block_store_t* block_store = block_store_create(); // Create a block store 
    int read_bytes = read(file_descriptor, block_store->store, BLOCK_STORE_NUM_BYTES); // Read a block store worth of bytes from the file into the newly created block store
    if(read_bytes != BLOCK_STORE_NUM_BYTES) // If a block store worth of bytes was not read
    {
        block_store_destroy(block_store); // Destroy the block store
        return NULL;
    }
    close(file_descriptor); // Close the file
    for(int i = 0; i < BLOCK_STORE_NUM_BYTES; i++) // Iterate over the number of bytes in the block store
    {
        if(block_store->store[i] != 0x00) // If the current byte has data
        {
            size_t block_id = index_to_block_id(i); // Get the block id of the current index
            block_store_request(block_store, block_id); // Request the block in the block store
            size_t next_block_index = get_block_id_index(block_id + 1); // Get the starting index of the next block
            i = next_block_index - 1; // Set i to the next index (minus 1 because the for loop will increment it)
        }
    }
    return block_store; // Return the block store
}

size_t block_store_serialize(const block_store_t *const bs, const char *const filename)
{
    if(bs == NULL)
    {
        return 0; // Return 0 if the block store is NULL
    }
    int file_descriptor = open(filename, O_WRONLY | O_CREAT, S_IRWXO | S_IRWXG | S_IRWXU); // Open the file with the name denoted by filename in write only and create only mode  (and with permissions)
    if (file_descriptor < 0)
    {
        return 0; // Return 0 if the file could not be opened
    }
    size_t written_bytes = write(file_descriptor, bs->store, BLOCK_STORE_NUM_BYTES); // Write the data in the block store into the file
    close(file_descriptor);                                       // Close the file
    return written_bytes; // Return the number of written bytes
}