#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  void *data;
  size_t type;
} DynixElement;

typedef struct
{
  void **data;
  size_t capacity;
  size_t len;
  bool is_raw;
} Dynix;

//
//
//
// Creator
Dynix *dynix_new(size_t capacity)
{
  Dynix *dynix = calloc(1, sizeof(Dynix));
  dynix->data = calloc(1, sizeof(void *) * capacity);
  dynix->capacity = capacity;
  dynix->len = 0;
  dynix->is_raw = false;
  return dynix;
}

Dynix *dynix_new_raw(size_t capacity)
{
  Dynix *dynix = calloc(1, sizeof(Dynix));
  dynix->data = calloc(1, sizeof(void *) * capacity);
  dynix->capacity = capacity;
  dynix->len = 0;
  dynix->is_raw = true;
  return dynix;
}

//
//
//
// Appender
void dynix_append_raw(Dynix *dynix, void *data)
{
  if (__builtin_expect(!dynix->is_raw, true))
  {
    fprintf(stderr, "dynix_append_raw: dynix is not raw\n");
    abort();
    return;
  }

  if (__builtin_expect(dynix->len >= dynix->capacity, false))
  {
    dynix->capacity *= 2;
    dynix->data = realloc(dynix->data, sizeof(void *) * dynix->capacity);
  }

  dynix->data[dynix->len] = data;
  dynix->len++;
}

void dynix_append_generic(Dynix *dynix, void *data, size_t type)
{
  if (__builtin_expect(dynix->is_raw, false))
  {
    fprintf(stderr, "dynix_append_generic: dynix is raw\n");
    return;
  }

  if (__builtin_expect(dynix->len >= dynix->capacity, false))
  {
    dynix->capacity *= 2;
    dynix->data = realloc(dynix->data, sizeof(void *) * dynix->capacity);
  }

  DynixElement *element = calloc(1, sizeof(DynixElement));
  element->data = data;
  element->type = type;

  dynix->data[dynix->len] = element;
  dynix->len++;
}

//
//
//
// Getter
void *dynix_get(Dynix *dynix, size_t index)
{
  if (__builtin_expect(index >= dynix->len, false))
  {
    fprintf(stderr, "dynix_get: index out of range\n");
    return NULL;
  }

  return dynix->data[index];
}

//
//
//
// Free
void dynix_free(Dynix *dynix)
{
  bool is_raw = dynix->is_raw;
  for (size_t i = 0; i < dynix->len; i++)
  {
    if (is_raw)
    {
      free(dynix->data[i]);
    }
    else
    {
      DynixElement *element = dynix->data[i];
      free(element->data);
      free(element);
    }
  }

  free(dynix->data);
  free(dynix);
}