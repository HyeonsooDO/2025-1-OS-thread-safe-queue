#include <atomic>
#include <thread>
#ifndef _QTYPE_H  // header guard
#define _QTYPE_H

// ==========이 파일은 수정 가능==========

typedef unsigned int Key;  // 값이 클수록 높은 우선순위
typedef void* Value;

typedef struct {
    Key key;
    Value value;
    int value_size;
} Item;

typedef struct {
    bool success;   // true: 성공, false: 실패
    Item item;
    // 필드 추가 가능
} Reply;

typedef struct node_t {
    Item item;
    struct node_t* next;
    // 필드 추가 가능
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

// 이후 자유롭게 추가/수정: 새로운 자료형 정의 등

#endif
