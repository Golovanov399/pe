#pragma once

struct DefaultMemoryManager {
	void* operator new(size_t n) {
		return ::operator new(n);
	}

	void operator delete(void* ptr) {
		::operator delete(ptr);
	}
};

char* static_mem_ = nullptr;
int static_memory_mgr_ptr_ = 0;

template <size_t N>
struct StaticMemoryManager {
	void* operator new(size_t n) {
		if (!static_mem_) {
			static_mem_ = (char*)malloc(N);
		}
		assert(static_memory_mgr_ptr_ + n <= N);
		static_memory_mgr_ptr_ += n;
		return (void*)(static_mem_ + static_memory_mgr_ptr_ - n);
	}

	void operator delete(void*) {}
};

char* cyclic_mem_ = nullptr;
int cyclic_memory_mgr_ptr_ = 0;

template <size_t N>
struct CyclicMemoryManager {
	void* operator new(size_t n) {
		if (!cyclic_mem_) {
			cyclic_mem_ = (char*)malloc(N);
		}
		if (cyclic_memory_mgr_ptr_ + n > N) {
			cyclic_memory_mgr_ptr_ = 0;
		}
		cyclic_memory_mgr_ptr_ += n;
		return (void*)(cyclic_mem_ + cyclic_memory_mgr_ptr_ - n);
	}

	void operator delete(void*) {}
};
