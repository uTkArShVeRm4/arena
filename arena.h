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
void* arena_allocate_aligned(Arena* arena, usize size, usize alignment);
void destroy_arena(Arena* arena);

// Type-safe allocation macros
#define ARENA_ALLOC_TYPE(arena, type) \
    ((type*)arena_allocate_aligned((arena), sizeof(type), _Alignof(type)))

#define ARENA_ALLOC_ARRAY(arena, type, count) \
    ((type*)arena_allocate_aligned((arena), sizeof(type) * (count), _Alignof(type)))

#endif
