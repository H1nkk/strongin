#pragma once
#include "dheap.h"

template<typename T>
class pqueue {
	dheaplimited_multimap<T> heap;

public:
	pqueue(size_t limit): heap(2,0,limit) { }

	void insert(T el) {
		heap.insert(el);
	}

	T pop() {
		return heap.pop();
	}

	T get() {
		return heap.getMax();
	}

	void clear() {
		heap.clear();
	}

	void erase(T elem) {
		heap.erase(elem);
	}
};
