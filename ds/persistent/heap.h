#pragma once

#include "../../base/random.h"
#include "../../base/memory.h"

template <typename T, typename MemMgr = DefaultMemoryManager>
struct PersistentHeap {
	PersistentHeap* son[2];
	T key;

	explicit PersistentHeap(const T& _key): son{nullptr, nullptr}, key(_key) {}

	friend PersistentHeap* clone(PersistentHeap* node) {
		if (!node) {
			return nullptr;
		}
		PersistentHeap* res = new PersistentHeap(node->key);
		res->son[0] = node->son[0];
		res->son[1] = node->son[1];
		return res;
	}

	friend PersistentHeap* push(PersistentHeap* node, const T& key) {
		if (!node || key < node->key) {
			PersistentHeap* res = new PersistentHeap(key);
			res->son[0] = node;
			return res;
		}
		int rd = rand() % 2;
		auto res = clone(node);
		res->son[rd] = push(res->son[rd], key);
		return res;
	}
};