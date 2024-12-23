
#include <iostream>
#include "list_allocator.h"

int main()
{
	char memory[1024];
	memset(memory, '0', 1024);
	for (int i = 0; i < 1024; i++) {
		std::cout << memory[i];
	}
	std::cout << '\n';
	Allocator* a = allocator_create((void*)memory, 1024);
	char* m1 = (char*)allocator_alloc(a, 128);
	memset(m1, '1', 128);
	char* m2 = (char*)allocator_alloc(a, 128);
	memset(m2, '1', 128);
	//memory[sizeof(Allocator)] = 's';
	for (int i = 0; i < 1024; i++) {
		std::cout << memory[i];
	}
	std::cout << '\n';
	allocator_free(a, m2);
	//memory[sizeof(Allocator)] = 's';
	for (int i = 0; i < 1024; i++) {
		std::cout << memory[i];
	}
	std::cout << '\n';
	m1 = (char*)allocator_alloc(a, 128);
	memset(m1, '1', 128);
	for (int i = 0; i < 1024; i++) {
		std::cout << memory[i];
	}
}

