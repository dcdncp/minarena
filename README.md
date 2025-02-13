# Minimal Arena Allocator

This project implements a minimal arena allocator in C. An arena allocator is a fast and efficient memory allocation strategy that allocates memory in large chunks and manages it in a stack-like manner.

### IMPORTANT
I wrote this library only for personal use, so it may have some bugs or issues. Use it at your own risk. 

## Features

- Fast memory allocation and deallocation
- Customizable chunk size and alignment
- Simple API for allocation, reallocation, and formatted string allocation

## API

### Structures

- `Min_Arena`: Represents the memory arena.
- `Min_Arena_Chunk`: Represents a chunk of memory within the arena.

### Functions

- `void destroy_minarena(Min_Arena *arena)`: Destroys the memory arena and frees all allocated memory.
- `void *mina_alloc(Min_Arena *arena, size_t size)`: Allocates memory of the given size from the arena.
- `void *mina_realloc(Min_Arena *arena, void *ptr, size_t old_size, size_t new_size)`: Reallocates memory from the arena.
- `const char *mina_format(Min_Arena *arena, const char *fmt, ...)`: Allocates memory for a formatted string.

## Usage

Include the `minarena.h` header file in your project and define `MINARENA_IMPLEMENTATION` in one source file to include the implementation.

```c
#define MINARENA_IMPLEMENTATION
#include "minarena.h"

int main() {
    Min_Arena arena = {0};
    
    // Allocate memory
    void *memory = mina_alloc(&arena, 1024);
    
    // Reallocate memory
    memory = mina_realloc(&arena, memory, 1024, 2048);
    
    // Allocate formatted string
    const char *formatted = mina_format(&arena, "Hello, %s!", "world");
    
    // Destroy the arena and free all memory
    destroy_minarena(&arena);
    
    return 0;
}
```

## License

This project is licensed under the MIT License.
