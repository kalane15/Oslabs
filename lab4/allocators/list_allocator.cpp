#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/mman.h>
#include <math.h>
#include "list_allocator.h"
//#include <unistd.h>

#define MIN_BLOCK_SIZE 16



// Функция создания аллокатора
Allocator* allocator_create(void* memory, size_t size) {
    if (!memory || size < sizeof(Allocator)) {
        return NULL;
    }

    Allocator* allocator = (Allocator*)memory;
    allocator->base_addr = memory;
    allocator->memory_start = (char*)memory + sizeof(Allocator);
    allocator->total_size = size - sizeof(Allocator);
    allocator->free_list_first = (BlockHeader*)allocator->memory_start;

    // Инициализируем метаданные первого блока
    allocator->free_list_first->size = allocator->total_size - sizeof(BlockHeader);
    allocator->free_list_first->next = NULL;
    allocator->free_list_first->is_free = true;

    return allocator;
}

// Функция выделения памяти
void* allocator_alloc(Allocator* allocator, size_t size) {
    if (!allocator || size == 0 || size > allocator->total_size) {
        return NULL;
    }

    size = (size + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE * MIN_BLOCK_SIZE;

    BlockHeader* prev = NULL;
    BlockHeader* current = allocator->free_list_first;
    while (current != NULL) {
        if (current->size >= size) {
            current->is_free = false;
            if (current->size == size) {
                if (prev != NULL) {
                   prev->next = current->next;
                }
                else {
                    allocator->free_list_first = allocator->free_list_first->next;
                }   
                current->size = size;
            }
            else {
                size_t remain_size = current->size - size;
                current->size = size;
                if (remain_size > MIN_BLOCK_SIZE + sizeof(BlockHeader)) {
                    BlockHeader* new_block = (BlockHeader*) ((char*)current + sizeof(BlockHeader) + size);
                    new_block->size = remain_size - sizeof(BlockHeader);
                    new_block->is_free = true;
                    new_block->next = current->next;
                    current->next = new_block;

                    if (prev != NULL) {
                        prev->next = current->next;
                    }
                    else {
                        allocator->free_list_first = allocator->free_list_first->next;
                    }
                }
            }
            break;
        }
        current = current->next;
    }


    return (char*)current + sizeof(BlockHeader);
}

// Функция освобождения памяти
void allocator_free(Allocator* allocator, void* ptr) {
    if (!allocator || !ptr) {
        return;
    }

    BlockHeader* block_header_to_free = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    if (!block_header_to_free) return;
    block_header_to_free->is_free = true;

    BlockHeader* current = allocator->free_list_first;
    BlockHeader* prev = NULL;

    if (current->next == NULL) { //если всего один элемент в списке
        if (block_header_to_free > current) {
            current->next = block_header_to_free;
            block_header_to_free->next = current->next;
        }
        else {
            allocator->free_list_first = block_header_to_free;
            allocator->free_list_first->next = current;
        }
    }
    else {
        while (current->next != NULL) {
            if (current->next > block_header_to_free) {
                BlockHeader* tmp = current->next;
                current->next = block_header_to_free;
                block_header_to_free->next = tmp;
                break;
            }
            current = current->next;
        }
        if (current->next == NULL) {
            current->next = block_header_to_free;
            block_header_to_free->next = NULL;
        }
    }

    // Пытаемся слить соседние блоки
    current = allocator->free_list_first;
    while (current && current->next) {
        if (((char*)current + sizeof(BlockHeader) + current->size) ==
            (char*)current->next) {
            current->size += current->next->size + sizeof(BlockHeader);
            current->next = current->next->next;           
        }
        else {
            current = current->next;
        }
    }

    current = allocator->free_list_first;
    while (current != NULL) {
        memset((char*)(current) + sizeof(BlockHeader), '0', current->size);
        current = current->next;
    }
}

// Функция уничтожения аллокатора
void allocator_destroy(Allocator* allocator) {
    if (allocator) {
        //munmap(allocator->base_addr, allocator->total_size + sizeof(Allocator));
    }
}