#include <atomic>
#include <thread>
#ifndef _QTYPE_H  // header guard
#define _QTYPE_H

// ==========�� ������ ���� ����==========

typedef unsigned int Key;  // ���� Ŭ���� ���� �켱����
typedef void* Value;

typedef struct {
    Key key;
    Value value;
    int value_size;
} Item;

typedef struct {
    bool success;   // true: ����, false: ����
    Item item;
    // �ʵ� �߰� ����
} Reply;

typedef struct node_t {
    Item item;
    struct node_t* next;
    // �ʵ� �߰� ����
} Node;

struct SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
        }
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

typedef struct {
    Node* head;
    Node* tail;
    SpinLock lock;
} Queue;

// ���� �����Ӱ� �߰�/����: ���ο� �ڷ��� ���� ��

#endif
