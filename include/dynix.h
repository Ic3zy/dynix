#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DYNIX_START_CHUNK_SIZE 256

typedef struct
{
  void *data;
  size_t type;
} DynixElement;

typedef struct
{
  void **data;
  size_t len;
} DynixChunk;

typedef struct
{
  DynixChunk chunks[64];
  size_t capacity;
  size_t len;
  bool is_raw;
} Dynix;

//
//
//
// Helpers
typedef struct
{
  size_t chunk;
  size_t offset;
} DynixIndex;

DynixIndex dynix_get_chunk_index(size_t index)
{
  size_t x = (index >> 8) + 1;
  size_t chunk = 63 - __builtin_clzll(x);
  size_t start = 256 * ((1ULL << chunk) - 1);

  return (DynixIndex){
      .chunk = chunk,
      .offset = index - start};
}

//
//
//
// Creator
Dynix *dynix_new()
{
  Dynix *dynix = calloc(1, sizeof(Dynix));

  dynix->chunks[0].data = calloc(DYNIX_START_CHUNK_SIZE, sizeof(void *));
  dynix->capacity = DYNIX_START_CHUNK_SIZE;

  dynix->len = 0;
  dynix->is_raw = false;

  return dynix;
}

Dynix *dynix_new_raw(size_t capacity)
{
  Dynix *dynix = calloc(1, sizeof(Dynix));

  dynix->chunks[0].data = calloc(DYNIX_START_CHUNK_SIZE, sizeof(void *));
  dynix->capacity = DYNIX_START_CHUNK_SIZE;

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
  if (__builtin_expect(!dynix->is_raw, false))
  {
    fprintf(stderr, "dynix_append_raw: dynix is not raw\n");
    abort();
    return;
  }

  DynixIndex index = dynix_get_chunk_index(dynix->len);

  if (__builtin_expect((dynix->len >= dynix->capacity), false))
  {
    size_t chunk_size = DYNIX_START_CHUNK_SIZE << index.chunk;
    dynix->capacity += chunk_size;

    dynix->chunks[index.chunk].data = calloc(chunk_size, sizeof(void *));
  }
  dynix->chunks[index.chunk].data[index.offset] = data;
  dynix->len++;
}

void dynix_append_generic(Dynix *dynix, void *data, size_t type)
{
  if (__builtin_expect(dynix->is_raw, false))
  {
    fprintf(stderr, "dynix_append_raw: dynix is not raw\n");
    abort();
    return;
  }

  DynixIndex index = dynix_get_chunk_index(dynix->len);

  if (__builtin_expect((dynix->len >= dynix->capacity), false))
  {
    size_t chunk_size = DYNIX_START_CHUNK_SIZE << index.chunk;
    dynix->capacity += chunk_size;
    dynix->chunks[index.chunk].data = calloc(dynix->capacity, sizeof(void *));
  }

  DynixElement *element = calloc(1, sizeof(DynixElement));
  element->data = data;
  element->type = type;

  dynix->chunks[index.chunk].data[index.offset] = element;
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

  DynixIndex dyn_index = dynix_get_chunk_index(index);

  return dynix->chunks[dyn_index.chunk].data[dyn_index.offset];
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
    DynixIndex index = dynix_get_chunk_index(i);
    if (is_raw)
    {
      free(dynix->chunks[index.chunk].data[index.offset]);
    }
    else
    {
      DynixElement *element = dynix->chunks[index.chunk].data[index.offset];
      free(element->data);
      free(element);
    }
  }

  free(dynix);
}

// this method does not free the data, but only the DynixElement struct
void dynix_soft_free(Dynix *dynix)
{
  bool is_raw = dynix->is_raw;
  for (size_t i = 0; i < dynix->len; i++)
  {
    DynixIndex index = dynix_get_chunk_index(i);
    if (!is_raw)
    {
      DynixElement *element = dynix->chunks[index.chunk].data[index.offset];
      free(element);
    }
  }
}