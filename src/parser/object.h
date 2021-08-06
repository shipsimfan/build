#ifndef OBJECT_H
#define OBJECT_H 1

#include "language.h"

typedef struct {
    char* target;
    char* source;
    Language language;
} Object;

typedef struct {
    Object** buffer;
    int buffer_capacity;
    int buffer_length;
} Objects;

Object* create_object();
void destroy_object(Object* object);

Objects* create_objects();
void push_objects(Objects* objects, Object* object);
void display_objects(Objects* objects);
void destroy_objects(Objects* objects);

#endif