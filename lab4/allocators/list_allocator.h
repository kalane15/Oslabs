#pragma once

// Структура для метаданных блока
typedef struct BlockHeader {
    size_t size;  // Размер блока данных
    struct BlockHeader* next;  // Указатель на следующий блок
    bool is_free;  // Флаг, указывающий, свободен ли блок
} BlockHeader;

// Структура аллокатора
typedef struct Allocator {
    BlockHeader* free_list_first;  // Указатель на начало списка свободных блоков
    void* memory_start;      // Начало выделенной памяти
    size_t total_size;  // Общий размер выделенной памяти
    void* base_addr;  // Указатель на начало выделенной памяти mmap
} Allocator;

Allocator* allocator_create(void* memory, size_t size);

void* allocator_alloc(Allocator* allocator, size_t size);

void allocator_free(Allocator* allocator, void* ptr);

void allocator_destroy(Allocator* allocator);
