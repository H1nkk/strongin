#pragma once
#include "dheap.h"

template<typename T>
class pqueue_limited_multimap {
	dheaplimited_multimap<T> heap;

public:
	pqueue_limited_multimap(size_t limit) : heap(2, 0, limit) {}

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

template<typename T>
class pqueue_limited_linear {
	dheaplimited_linear<T> heap;

public:
	pqueue_limited_linear(size_t limit): heap(2,0,limit) { }

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

template<typename T>
class pqueue {
	dheap<T> heap;

public:
	pqueue() : heap(2) {}

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