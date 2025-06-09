#include <iostream>
#include "queue.h"


Queue* init(void) {
	Queue* q = new Queue;
	q->head = nullptr;
	q->tail = nullptr;
	return q;
}


void release(Queue* queue) {
	if (!queue) return;
	Node* curr = queue->head;
	while (curr) {
		Node* next = curr->next;
		nfree(curr);
		curr = next;
	}
	delete queue;
}


Node* nalloc(Item item) {
	Node* node = new Node;
	node->item.key = item.key;
	node->item.value_size = item.value_size;

	if (item.value_size > 0 && item.value) {
		node->item.value = malloc(item.value_size);
		memcpy(node->item.value, item.value, item.value_size);
	}
	else {
		node->item.value = nullptr;
	}
	node->next = nullptr;
	return node;
}

void nfree(Node* node) {
	if (node->item.value) {
		free(node->item.value);
	}
	delete node;
}


Node* nclone(Node* node) {
	if (!node) return nullptr;
	return nalloc(node->item);
}


Reply enqueue(Queue* queue, Item item) {
	Node* new_node = nalloc(item);
	if (!new_node) return { false, item };
	queue->lock.lock();

	if (!queue->head) {
		queue->head = queue->tail = new_node;
		queue->lock.unlock();
		return { true, item };
	}

	Node* prev = nullptr;
	Node* curr = queue->head;
	while (curr && curr->item.key > item.key) {
		prev = curr;
		curr = curr->next;
	}

	if (curr && curr->item.key == item.key) {
		if (!item.value || item.value_size <= 0) {
			queue->lock.unlock();
			return { false, item };
		}
		free(curr->item.value);
		curr->item.value = malloc(item.value_size);
		memcpy(curr->item.value, item.value, item.value_size);
		curr->item.value_size = item.value_size;
		queue->lock.unlock();
		return { true, curr->item };
	}
	
	if (!prev) {
		new_node->next = queue->head;
		queue->head = new_node;
	}
	else {
		prev->next = new_node;
		new_node->next = curr;
	}

	if (!curr) queue->tail = new_node;
	queue->lock.unlock();
	return { true, item };
}

Reply dequeue(Queue* queue) {
	queue->lock.lock();
	if (!queue->head) {
		queue->lock.unlock();
		return { false, {} };
	}

	Node* node = queue->head;
	queue->head = node->next;
	if (!queue->head) queue->tail = nullptr;

	Item item = node->item;
	nfree(node);
	queue->lock.unlock();
	return { true, item };
}

Queue* range(Queue* queue, Key start, Key end) {
	queue->lock.lock();
	Queue* new_q = init();
	if (!new_q) return nullptr;

	Node* curr = queue->head;

	while (curr) {
		if (curr->item.key < start) {
			curr = curr->next;
			continue;
		}

		if (curr->item.key > end) break;

		Node* clone = nclone(curr);
		if (!clone) {
			release(new_q);
			queue->lock.unlock();
			return nullptr;
		}

		if (!new_q->head) {
			new_q->head = new_q->tail = clone;
		}
		else {
			new_q->tail->next = clone;
			new_q->tail = clone;
		}

		curr = curr->next;
	}
	queue->lock.unlock();
	return new_q;
}