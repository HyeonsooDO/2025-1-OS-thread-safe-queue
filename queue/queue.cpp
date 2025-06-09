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

	if (!queue->head) {
		queue->head = queue->tail = new_node;
		return { true, item };
	}

	Node* prev = nullptr;
	Node* curr = queue->head;
	while (curr && curr->item.key > item.key) {
		prev = curr;
		curr = curr->next;
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
	return { true, item };
}

Reply dequeue(Queue* queue) {
	if (!queue->head) return { false, {} };

	Node* node = queue->head;
	queue->head = node->next;
	if (!queue->head) queue->tail = nullptr;

	Item item = node->item;
	nfree(node);
	return { true, item };
}

Queue* range(Queue* queue, Key start, Key end) {
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

	return new_q;
}