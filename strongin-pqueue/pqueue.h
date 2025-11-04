#pragma once
#include "dheap.h"

template<typename T>
class pqueue {
	dheap<T> heap = dheap<T>(2);

public:
	pqueue() {}

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