//#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <fcntl.h>
//#include <dlfcn.h>
//#include <sys/mman.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define MIN_SIZE_BLOCK 32

typedef struct BlockHeader {
    size_t size;  // Размер блока данных
    struct BlockHeader* next;  // Указатель на следующий блок
    struct BlockHeader* buddy;  // Указатель на близнеца
    bool is_free;  // Флаг, указывающий, свободен ли блок
} BlockHeader;

typedef struct Allocator {
    void* memory;
    size_t size;
    uint8_t* bitmap;
    size_t block_size;
    BlockHeader** free_lists;
    size_t max_pow;
    size_t total_free_lists_size;
} Allocator;



Allocator* allocator_create(void* const memory, const size_t size) {
    if (!memory || size == 0) return NULL;

    Allocator* allocator = (Allocator*)memory;
    allocator->memory = (void*)((char*)memory + sizeof(Allocator));
    allocator->size = size - sizeof(Allocator);
    allocator->block_size = 32;
    allocator->bitmap = (uint8_t*)allocator->memory;

    size_t bitmap_size = allocator->size / allocator->block_size / 8;
    memset(allocator->bitmap, 0, bitmap_size); // Все блоки свободны
    allocator->memory = (uint8_t*)allocator->bitmap + bitmap_size;

    allocator->free_lists = (BlockHeader**)allocator->memory;
    size_t max_pow = 0;
    size_t cur = MIN_SIZE_BLOCK;
    while (cur < size) {
        cur *= 2;
        max_pow++;
    }
    allocator->max_pow = max_pow;
    allocator->total_free_lists_size = (max_pow * (size / MIN_SIZE_BLOCK)) * sizeof(BlockHeader);
    memset(allocator->free_lists, 0, (max_pow * (size / MIN_SIZE_BLOCK)) * sizeof(BlockHeader));

    allocator->free_lists[max_pow]->is_free = true;
    allocator->free_lists[max_pow]->size = size;
    allocator->free_lists[max_pow]->next = NULL;

    return allocator;
}

void allocator_destroy(Allocator* const allocator) {
    if (allocator) {
        memset(allocator, 0, allocator->size);
    }
}

void add_to_list(BlockHeader** free_list_first, BlockHeader* to_insert) {
    BlockHeader* current = *free_list_first;
    BlockHeader* prev = NULL;
    if (current->next == NULL) { //если всего один элемент в списке
        if (to_insert > current) {
            current->next = to_insert;
            to_insert->next = current->next;
        }
        else {
            *free_list_first = to_insert;
            (*free_list_first)->next = current;
        }
    }
    else {
        while (current->next != NULL) {
            if (current->next > to_insert) {
                BlockHeader* tmp = current->next;
                current->next = to_insert;
                to_insert->next = tmp;
                break;
            }
            current = current->next;
        }
        if (current->next == NULL) {
            current->next = to_insert;
            to_insert->next = NULL;
        }
    }
}

size_t roud_to_2_pow(size_t num) {
    size_t cur = MIN_SIZE_BLOCK;
    while (num > cur) {
        cur *= 2;
    }
    return cur;
}

void* allocator_alloc(Allocator* const allocator, size_t size) {
    if (!allocator || size == 0 || size > allocator->size) return NULL;

    size = roud_to_2_pow(size);
    size_t pow = (size_t)log2(size);
  
    size_t min_exist_pow = pow;
    while (allocator->free_lists[min_exist_pow] == NULL) {
        min_exist_pow++;
        if (min_exist_pow == allocator->max_pow) {
            return NULL;
        }
    }

    BlockHeader* result_block;
    BlockHeader* cur = NULL;
    BlockHeader* block1 = allocator->free_lists[min_exist_pow];
    BlockHeader* block2;
    while (min_exist_pow != pow) {
        cur = allocator->free_lists[min_exist_pow];
        allocator->free_lists[min_exist_pow] = allocator->free_lists[min_exist_pow]->next;
        block1 = allocator->free_lists[min_exist_pow] + sizeof(BlockHeader);
        block2 = allocator->free_lists[min_exist_pow] + sizeof(BlockHeader) + allocator->free_lists[min_exist_pow]->size / 2 + sizeof(BlockHeader);
        block1->size = allocator->free_lists[min_exist_pow]->size / 2;
        block1->is_free = true;
        block2->size = allocator->free_lists[min_exist_pow]->size / 2;
        block2->is_free = true;
        min_exist_pow--;
        add_to_list(&(allocator->free_lists[min_exist_pow]), block1);
        add_to_list(&(allocator->free_lists[min_exist_pow]), block2);
    }
    block1 = allocator->free_lists[min_exist_pow];
    allocator->free_lists[min_exist_pow] = allocator->free_lists[min_exist_pow]->next;
    block1->is_free = false;
    return (char*)allocator->free_lists[min_exist_pow] + sizeof(BlockHeader);
}

void allocator_free(Allocator* const allocator, void* const memory) {
    BlockHeader* cur = (BlockHeader*)((char*)memory - sizeof(BlockHeader));
    while (true) {       
        cur->is_free = true;
        size_t pow = log2(cur->size);
        BlockHeader* current = allocator->free_lists[pow];
        BlockHeader* prev = NULL;

        bool has_merged = false;
        while (current != NULL) {
            if (current->size == cur->size) {
                if (prev == NULL) {
                    allocator->free_lists[pow] = allocator->free_lists[pow]->next;
                }
                else {
                    prev->next = current->next;
                }
                BlockHeader* new_block = current < cur ? current : cur;
                new_block->is_free = true;
                new_block->size = current->size * 2;
                has_merged = true;
                cur = new_block;
                break;
            }
            current = current->next;
        }
        if (!has_merged) {
            break;
        }
   }
}
