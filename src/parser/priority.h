#ifndef PRIORITY_H
#define PRIORITY_H 1

typedef struct {
    char** queue;
    int queue_capacity;
    int queue_length;
} Priority;

Priority* create_priority();
void push_priority(Priority* priority, const char* name);
void destroy_priority(Priority* priority);

#endif