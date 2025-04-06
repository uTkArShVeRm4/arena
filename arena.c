#include "arena.h"
#include <stdio.h>
#include <common_types.h>

usize next_power_of_2(usize n) {
    if (n == 0) {
        return 1; // Special case for 0
    }

    n--;  // Subtract 1 to handle cases where n is already a power of 2
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;

    return n + 1;
}

ArenaChunk* create_arena_chunk(usize capacity){
    void* buffer = malloc(capacity);
    if (buffer == NULL){
        fprintf(stderr, "Error: malloc failed to allocate buffer in create_arena_chunk\n");
        return NULL; 

    }
    ArenaChunk* arena_chunk = (ArenaChunk*)malloc(sizeof(ArenaChunk));
    if (arena_chunk == NULL) {
        fprintf(stderr, "Error: malloc failed to allocate ArenaChunk in create_arena_chunk\n");
        free(buffer); 
        return NULL; 
    }    
    
    arena_chunk->buffer = buffer;
    arena_chunk->used = 0;
    arena_chunk->capacity = capacity;
    arena_chunk->next_chunk = NULL;
    return arena_chunk;
}

void destroy_arena_chunk(ArenaChunk* arena_chunk){
    free(arena_chunk->buffer);
    free(arena_chunk);
}

usize align_up(usize n, usize alignment) {
    usize remainder = n % alignment;
    if (remainder == 0){
        return n;
    } else{
        return n + alignment - remainder;
    }
}

void* arena_chunk_allocate(ArenaChunk* arena_chunk, usize size){
    usize alignment_used = align_up(arena_chunk->used, size);
    if (alignment_used + size <= arena_chunk->capacity){ 
        void* ptr = (char*)arena_chunk->buffer + alignment_used;
        arena_chunk->used = alignment_used + size;
        return  ptr;
    } else {
      return NULL; // Owner Arena should create new chunk?
    }
}

Arena create_arena(usize chunk_size){
    ArenaChunk* arena_chunk = create_arena_chunk(chunk_size);
    if (arena_chunk == NULL){
        fprintf(stderr, "Arena creation failed, exiting...");
        exit(1);
    }

    Arena arena = ( Arena ){.chunk_size=chunk_size, .first_chunk=arena_chunk};
    return arena;
}

void* arena_allocate(Arena* arena, usize size) {
    if (arena == NULL) return NULL;

    ArenaChunk* current_chunk = arena->first_chunk;

    // If the arena is empty, create the first chunk
    if (current_chunk == NULL) {
        current_chunk = create_arena_chunk(arena->chunk_size);
        if (current_chunk == NULL) return NULL;
        arena->first_chunk = current_chunk; // Assign the created chunk to the arena
    }

    while (current_chunk != NULL) {
        // Try allocating in this chunk
        void* allocated = arena_chunk_allocate(current_chunk, size);

        if (allocated != NULL) {
            // Allocation was successful
            return allocated;
        }

        // Check if there's another chunk
        if (current_chunk->next_chunk != NULL) {
            // Try the next chunk in the next iteration
            current_chunk = current_chunk->next_chunk;
        } else {
            // Create a new chunk and try allocation in this new chunk
            // also double chunk size
            arena->chunk_size *= 2;
            if (!(size <= arena->chunk_size)){
                // set chunk_size to the next biggest power of 2
                // if the memory requirement wont fit in the current size chunk
                arena->chunk_size = next_power_of_2(size);
            }
            ArenaChunk* new_chunk = create_arena_chunk(arena->chunk_size);
            if (new_chunk == NULL) {
                // Creating a chunk failed
                return NULL;
            }

            current_chunk->next_chunk = new_chunk;
            current_chunk = new_chunk;
        }
    }

    // If all chunks have been tried and we have failed to create any chunks then return NULL
    return NULL;
}

// We need to add this aligned allocation function to your arena implementation
void* arena_allocate_aligned(Arena* arena, usize size, usize alignment) {
    if (arena == NULL) return NULL;
    ArenaChunk* current_chunk = arena->first_chunk;
    
    // If the arena is empty, create the first chunk
    if (current_chunk == NULL) {
        current_chunk = create_arena_chunk(arena->chunk_size);
        if (current_chunk == NULL) return NULL;
        arena->first_chunk = current_chunk;
    }
    
    while (current_chunk != NULL) {
        // Calculate the aligned address within this chunk
        usize current_offset = current_chunk->used;
        usize aligned_offset = align_up(current_offset, alignment);
        usize new_used = aligned_offset + size;

        if (new_used <= current_chunk->capacity) {
            void* ptr = (char*)current_chunk->buffer + aligned_offset;
            current_chunk->used = new_used;
            return ptr;
        }
        
        // Move to next chunk or create a new one
        if (current_chunk->next_chunk != NULL) {
            current_chunk = current_chunk->next_chunk;
        } else {
            // Create a new chunk
            arena->chunk_size *= 2;
            if (!(size <= arena->chunk_size)){
                // set chunk_size to the next biggest power of 2
                // if the memory requirement wont fit in the current size chunk
                arena->chunk_size = next_power_of_2(size);
            }
            ArenaChunk* new_chunk = create_arena_chunk(arena->chunk_size);
            if (new_chunk == NULL) return NULL;
            current_chunk->next_chunk = new_chunk;
            current_chunk = new_chunk;
        }
    }
    
    return NULL;
}
    

void destroy_arena(Arena* arena) {
    if (arena == NULL){
        return;
    }
    
    ArenaChunk* current_chunk = arena->first_chunk;
    while (current_chunk != NULL){
        ArenaChunk* next_chunk = current_chunk->next_chunk;
        destroy_arena_chunk(current_chunk);
        current_chunk = next_chunk;
    }
    arena->first_chunk = NULL;
}
