#define MINARENA_VERSION "0.0.1"

#ifndef MINARENA_H_
#define MINARENA_H_

#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_CHUNK_CAPACITY (8 * 1024)
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

typedef struct Min_Arena_Chunk Min_Arena_Chunk;

struct Min_Arena_Chunk {
  Min_Arena_Chunk *next;
  uint8_t *ptr;
  size_t size, capacity;
};

typedef struct {
  Min_Arena_Chunk *start, *last;
} Min_Arena;

void destroy_minarena(Min_Arena *arena);

void *mina_alloc(Min_Arena *arena, size_t size);
void *mina_realloc(Min_Arena *arena, void *ptr, size_t old_size,
                   size_t new_size);
const char *mina_format(Min_Arena *arena, const char *fmt, ...);

#endif // MINARENA_H_

#ifdef MINARENA_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

Min_Arena_Chunk *new_mina_chunk(size_t capacity) {
  Min_Arena_Chunk *chunk = malloc(sizeof(Min_Arena_Chunk));
  chunk->capacity =
      capacity > DEFAULT_CHUNK_CAPACITY ? capacity : DEFAULT_CHUNK_CAPACITY;
  chunk->ptr = malloc(chunk->capacity);
  chunk->size = 0;
  chunk->next = NULL;
  return chunk;
}

void free_mina_chunk(Min_Arena_Chunk *chunk) {
  free(chunk->ptr);
  free(chunk);
}

void destroy_minarena(Min_Arena *arena) {
  Min_Arena_Chunk *chunk = arena->start;
  while (chunk) {
    Min_Arena_Chunk *next = chunk->next;
    free_mina_chunk(chunk);
    chunk = next;
  }
}

void *minarena_alloc_with_align(Min_Arena *arena, size_t size, size_t align) {
  if (size == 0)
    return NULL;
  if (!arena->start)
    arena->start = arena->last = new_mina_chunk(size);
  Min_Arena_Chunk *chunk = arena->last;
  while (true) {
    uint8_t *ptr =
        (uint8_t *)(((uintptr_t)(chunk->ptr + chunk->size + (align - 1))) &
                    ~(align - 1));
    size_t real_size = (size_t)((ptr + size) - (chunk->ptr + chunk->size));
    if (chunk->size + real_size >= chunk->capacity) {
      if (chunk->next) {
        chunk = chunk->next;
        continue;
      }
      size_t word_size = size + (align - 1);
      Min_Arena_Chunk *nchunk = new_mina_chunk(word_size);
      chunk = arena->last = chunk->next = nchunk;
      continue;
    } else {
      memset(ptr, 0, real_size);
      chunk->size += real_size;
      return ptr;
    }
  }
}

void *mina_alloc(Min_Arena *arena, size_t size) {
  return minarena_alloc_with_align(arena, size, DEFAULT_ALIGNMENT);
}

void *mina_realloc(Min_Arena *arena, void *ptr, size_t old_size,
                   size_t new_size) {
  void *nptr = mina_alloc(arena, new_size);
  memcpy(nptr, ptr, old_size);
  return nptr;
}
const char *mina_format(Min_Arena *arena, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int n = vsnprintf(NULL, 0, format, args);
  va_end(args);

  assert(n >= 0);
  char *result = (char *)mina_alloc(arena, n + 1);

  va_start(args, format);
  vsnprintf(result, n + 1, format, args);
  va_end(args);
  return result;
}

#endif // MINARENA_IMPLEMENTATION