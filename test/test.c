#include "dynix.h"
#include <stdio.h>

void test_raw_list(void)
{
  Dynix *dynix = dynix_new_raw(10);
  int a = 10;
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
  Dynix *dynix = dynix_new(10);
  int a = 10;

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
int main()
{
  printf("Testing generic list\n");
  test_generic_list();
  printf("\n\nTesting raw list\n");
  test_raw_list();
  return 0;
}