#ifndef ARENA_H
#define ARENA_H
#include <common_types.h>

typedef struct ArenaChunk {
    void* buffer;
    usize used;
    usize capacity;
    struct ArenaChunk* next_chunk;
} ArenaChunk;

typedef struct Arena{
    usize chunk_size;
    ArenaChunk* first_chunk;
} Arena;

ArenaChunk* create_arena_chunk(usize capacity);
void* arena_chunk_allocate(ArenaChunk* arena_chunk, usize size);
void destroy_arena_chunk(ArenaChunk* arena_chunk);


Arena create_arena(usize chunk_size);
void* arena_allocate(Arena* arena, usize size);
void destroy_arena(Arena* arena);

#endif
