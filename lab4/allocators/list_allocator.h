#pragma once

// ��������� ��� ���������� �����
typedef struct BlockHeader {
    size_t size;  // ������ ����� ������
    struct BlockHeader* next;  // ��������� �� ��������� ����
    bool is_free;  // ����, �����������, �������� �� ����
} BlockHeader;

// ��������� ����������
typedef struct Allocator {
    BlockHeader* free_list_first;  // ��������� �� ������ ������ ��������� ������
    void* memory_start;      // ������ ���������� ������
    size_t total_size;  // ����� ������ ���������� ������
    void* base_addr;  // ��������� �� ������ ���������� ������ mmap
} Allocator;

Allocator* allocator_create(void* memory, size_t size);

void* allocator_alloc(Allocator* allocator, size_t size);

void allocator_free(Allocator* allocator, void* ptr);

void allocator_destroy(Allocator* allocator);
