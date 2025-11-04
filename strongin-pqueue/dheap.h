#pragma once
#include <vector>
#include <iostream>

using namespace std;

template<typename T>
class dheap {
    vector<T> mem;
    size_t size;
    size_t d;
    int height;

    int find(T x) {
        for (int i = 0; i < size; i++) {
            if (mem[i] == x) {
                return i;
            }
        }
        return -1;
    }
    int maxChildIndex(int x) {
        int res = d * x + 1;
        T max = mem[min(d * x + 1, size - 1)];
        for (int i = 1; i <= d; i++) {
            T child = mem[min(d * x + i, size - 1)];
            if (child > max) {
                res = d * x + i;
                max = child;
            }
        }
        return res;
    }
    void ascend(int i) {
        int p = (i - 1) / d; // индекс предка
        while ((i != 0) && mem[i] > mem[p]) {
            swap(mem[i], mem[p]);
            i = p;
            p = (i - 1) / d;
        }
    }
    void descend(int i) {
        int p = maxChildIndex(i); // индекс потомка
        while ((p < size) && (mem[i] < mem[p])) {
            swap(mem[i], mem[p]);
            i = p;
            p = maxChildIndex(i);
        }
    }
public:
    dheap(size_t d, size_t sz = 0) : mem(sz), size(0), d(d), height(0) {
        if (d < 1) throw "d < 1";
    }
    void insert(T x) {
        if (size == mem.size()) {
            mem.resize(size + pow(d, ++height));
        }
        mem[size] = x;
        ascend(size);
        size++;
    }

    void erase(T elem) {
        int i = find(elem);
        swap(mem[i], mem[size - 1]);
        size--;
        if ((i != 0) && (mem[i] > mem[(i - 1) / d])) {
            ascend(i);
        }
        else {
            descend(i);
        }
    }

    void eraseMax() {
        swap(mem[0], mem[size - 1]);
        size--;
        if (size != 0)
            descend(0);
    }

    T getMax() {
        return mem[0];
    }

    T pop() {
        T res = mem[0];
        eraseMax();
        return res;
    }

    void clear() {
        mem.resize(0);
        size = 0;
        height = 0;
    }

};