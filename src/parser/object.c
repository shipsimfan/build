#include "object.h"

#include <stdio.h>
#include <stdlib.h>

Object* create_object() {
    Object* object = malloc(sizeof(Object));
    object->target = NULL;
    object->source = NULL;
    object->language = LANGUAGE_INVALID;
    return object;
}

void destroy_object(Object* object) {
    if (object == NULL)
        return;

    free(object->target);
    free(object->source);
    free(object);
}

Objects* create_objects() {
    Objects* objects = malloc(sizeof(Objects));
    objects->buffer = malloc(sizeof(Object*));
    objects->buffer_capacity = 1;
    objects->buffer_length = 0;
    return objects;
}

void push_objects(Objects* objects, Object* object) {
    if (objects == NULL || object == LANGUAGE_INVALID)
        return;

    if (objects->buffer_capacity == objects->buffer_length) {
        objects->buffer_capacity *= 2;
        objects->buffer = realloc(objects->buffer, objects->buffer_capacity * sizeof(Object*));
    }

    objects->buffer[objects->buffer_length] = object;
    objects->buffer_length++;
}

void display_objects(Objects* objects) {
    printf("{\n");
    for (int i = 0; i < objects->buffer_length; i++) {
        printf("\t%s --> %s (", objects->buffer[i]->source, objects->buffer[i]->target);
        display_language(objects->buffer[i]->language);
        printf(")\n");
    }

    printf("}\n");
}

void destroy_objects(Objects* objects) {
    if (objects == NULL)
        return;

    for (int i = 0; i < objects->buffer_length; i++)
        destroy_object(objects->buffer[i]);

    free(objects->buffer);
    free(objects);
}