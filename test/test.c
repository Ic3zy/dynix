#define _POSIX_C_SOURCE 200809L

#include "dynix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/time.h>

static inline uint64_t ns(void)
{
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

__attribute__((optimize("O0"))) void benchmark(void)
{
  Dynix *dynix = dynix_new_raw();
  size_t *b = malloc(sizeof(size_t));
  *b = 100;

  uint64_t start = ns();
  for (size_t i = 0; i < 10000000; i++)
  {
    dynix_append_raw(dynix, b);
  }
  uint64_t end = ns();

  printf("Time: %fms\n", (end - start) / 1000000.0);
  // do not free my appended data
  // dynix_soft_free(dynix);
  // free(b);
  dynix_safe_free(dynix);
}

void test_raw_exist(void)
{
  Dynix *dynix = dynix_new_raw();

  size_t *b = malloc(sizeof(size_t));
  *b = 30;
  dynix_append_raw(dynix, b);

  printf("%s\n", dynix_exists(dynix, b) ? "true" : "false");

  dynix_free(dynix);
}

void test_raw_list(void)
{
  Dynix *dynix = dynix_new_raw();

  for (size_t i = 0; i < 10; i++)
  {
    size_t *b = malloc(sizeof(size_t));
    *b = i;
    dynix_append_raw(dynix, b);
  }

  for (size_t i = 0; i < dynix->len; i++)
  {
    printf("%d\n", *(int *)dynix_get(dynix, i));
  }

  printf("%d\n", dynix->capacity);

  dynix_free(dynix);
}

void test_generic_list(void)
{
  Dynix *dynix = dynix_new();

  // append integers, type 1
  for (size_t i = 0; i < 10; i++)
  {
    size_t *b = malloc(sizeof(size_t));
    *b = i;
    dynix_append_generic(dynix, b, 1);
  }

  // append char *, type 2
  // this char method is not safe, but it's only for testing
  char *c = "Hello C";
  dynix_append_generic(dynix, c, 2);

  for (size_t i = 0; i < dynix->len; i++)
  {
    DynixElement *element = dynix_get(dynix, i);
    size_t type = element->type;
    if (type == 1)
    {
      printf("%d\n", *(size_t *)element->data);
    }
    else if (type == 2)
    {
      printf("%s\n", (char *)element->data);
    }
    else
    {
      printf("Unknown\n");
    }
  }

  printf("%d\n", dynix->capacity);
}

void test_generic_exist(void)
{
  Dynix *dynix = dynix_new();

  size_t *b = malloc(sizeof(size_t));
  *b = 30;
  dynix_append_generic(dynix, b, 1);

  printf("%s\n", dynix_exists(dynix, b) ? "true" : "false");

  dynix_free(dynix);
}

void test_raw_remove(void)
{
  Dynix *dynix = dynix_new_raw();

  for (size_t i = 0; i < 10; i++)
  {
    size_t *b = malloc(sizeof(size_t));
    *b = i;
    dynix_append_raw(dynix, b);
  }

  printf("remove bef %d\n", dynix->len);
  dynix_delete(dynix, 3);

  printf("remove after %d\n", dynix->len);
  printf("%d\n", dynix->capacity);

  for (size_t i = 0; i < dynix->len; i++)
  {
    printf("%d\n", *(int *)dynix_get(dynix, i));
  }

  dynix_safe_free(dynix);
}

int main()
{

  printf("Testing generic list\n");
  test_generic_list();
  printf("\n\nTesting raw list\n");
  test_raw_list();
  printf("\n\nTesting raw remove\n");
  test_raw_remove();
  printf("\n\nTesting raw exist\n");
  test_raw_exist();
  printf("\n\nTesting generic exist\n");
  test_generic_exist();
  printf("\n\nBenchmarking\n");
  benchmark();
  return 0;
}