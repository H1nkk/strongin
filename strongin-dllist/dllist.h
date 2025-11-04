#pragma once
#include <vector>

using namespace std;

class dllist {
	struct Node
	{
		double x;
		Node* prev;
		Node* next;
		Node() {
			x = 0.0;
			prev = nullptr;
			next = nullptr;
		}
		Node(double x, Node* prev, Node* next) : x(x), prev(prev), next(next) {}
	};
	Node* lastUsed; // хранение последней использованной node для эффективного прохода по списку
	int lastUsedInd; // индекс последней использованной node
	Node* start;
	int sz;


public:
	dllist() : sz(0) {
		start = nullptr;
		lastUsed = nullptr;
		lastUsedInd = -1;
	}
	dllist(const vector<double>& v) : sz(0) {
		start = nullptr;
		for (int i = v.size() - 1; i >= 0; i--) {
			insert(0, v[i]);
		}
	}
	void insert(int index, double x) { // index на который вставляем
		if (index > sz) throw "dllist index out of range";
		sz++;
		int tmpIndex = index;
		if (index == 0) {
			lastUsed = nullptr;
			if (start == nullptr) {
				start = new Node(x, nullptr, start);
				return;
			}
			Node* tmp = new Node(x, nullptr, start);
			start->prev = tmp;
			start = tmp;
			return;
		}


		Node* cur = start;

		if (lastUsed) {
			if (index > lastUsedInd) {
				cur = lastUsed;
				index -= lastUsedInd;
			}
			else if (index == lastUsedInd) {
				cur = lastUsed->prev;
				index -= lastUsedInd;
			}
			lastUsed = nullptr;

		}
		while (--index) {
			cur = cur->next;
		}
		Node* tmp = cur->next;
		Node* newNode = new Node(x, cur, tmp);
		if (cur->next)
			cur->next->prev = newNode;
		cur->next = newNode;

	}
	double& get(int index) {
		int tmpIndex = index;
		Node* cur = start;
		if (lastUsed) {
			if (index >= lastUsedInd) {
				cur = lastUsed;
				index -= lastUsedInd;
			}
			else {
				int dist = lastUsedInd - index; // расстояние от lastused до искомой node
				if (dist < index) { // если расстояние от lastused меньше чем от start, то идем назад, начиная с lastUsed
					cur = lastUsed;
					while (dist--) {
						cur = cur->prev;
					}
					lastUsed = cur;
					lastUsedInd = tmpIndex;
					return cur->x;
				}
			}
		}

		while (index--) {
			cur = cur->next;
		}
		lastUsed = cur;
		lastUsedInd = tmpIndex;
		return cur->x;
	}
	void change(int index, double x) {
		Node* cur = start;
		while (index--) {
			cur = cur->next;
		}
		cur->x = x;
	}
	int size() {
		return sz;
	}
	double& operator[](int index) {
		return get(index);
	}
	~dllist() {
		vector<Node*> toDel(sz);
		Node* cur = start;
		for (int i = 0; i < sz; i++) {
			toDel[i] = cur;
			cur = cur->next;
		}
		for (int i = 0; i < sz; i++) {
			delete toDel[i];
		}
	}
};