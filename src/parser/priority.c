#include "priority.h"

#include "../util.h"
#include <stdlib.h>

Priority* create_priority() {
    Priority* priority = malloc(sizeof(Priority));
    priority->queue = malloc(sizeof(char*));
    priority->queue_capacity = 1;
    priority->queue_length = 0;
    return priority;
}

void push_priority(Priority* priority, const char* name) {
    if (priority == NULL || name == NULL)
        return;

    if (priority->queue_capacity == priority->queue_length) {
        priority->queue_capacity *= 2;
        priority->queue = realloc(priority->queue, priority->queue_capacity * sizeof(char*));
    }

    priority->queue[priority->queue_length] = copy_string(name);
    priority->queue_length++;
}

void destroy_priority(Priority* priority) {
    if (priority == NULL)
        return;

    for (int i = 0; i < priority->queue_length; i++)
        free(priority->queue[i]);

    free(priority->queue);
    free(priority);
}