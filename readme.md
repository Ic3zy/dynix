# Dynix

**Dynix** is a lightweight C dynamic array library designed for low-overhead data storage with both **raw** and **generic** modes.

## Features

* Dynamic array with automatic capacity growth
* **Raw mode** for minimal-overhead pointer storage
* **Generic mode** with per-element type metadata
* O(1) indexed access
* Automatic capacity expansion
* Simple C API
* No external dependencies
* Suitable for low-level C projects

## Modes

Dynix provides two different array modes.

### Raw

Raw mode stores `void *` directly without additional type metadata.

```c
Dynix *dynix = dynix_new_raw(10);

dynix_append_raw(dynix, data);
```

This mode is intended when the caller already knows what each element contains and wants to minimize overhead.

### Generic

Generic mode stores each element together with its type information.

```c
Dynix *dynix = dynix_new(10);

dynix_append_generic(dynix, data, type);
```

Each generic element contains:

```c
typedef struct
{
    void *data;
    size_t type;
} DynixElement;
```

This allows different types to coexist in the same array.

## Basic Usage

```c
#include "dynix.h"

int main(void)
{
    Dynix *dynix = dynix_new_raw(10);

    for (size_t i = 0; i < 10; i++)
    {
        size_t *value = malloc(sizeof(*value));
        *value = i;

        dynix_append_raw(dynix, value);
    }

    for (size_t i = 0; i < dynix->len; i++)
    {
        size_t *value = dynix_get(dynix, i);
        printf("%zu\n", *value);
    }

    dynix_free(dynix);

    return 0;
}
```

## API

### Creation

```c
Dynix *dynix_new(size_t capacity);
Dynix *dynix_new_raw(size_t capacity);
```

`dynix_new()` creates a generic Dynix array.

`dynix_new_raw()` creates a raw Dynix array.

### Append

```c
void dynix_append_raw(Dynix *dynix, void *data);
void dynix_append_generic(Dynix *dynix, void *data, size_t type);
```

Both modes automatically grow when `len` reaches `capacity`.

The current implementation doubles the capacity:

```text
capacity → capacity × 2
```

### Get

```c
void *dynix_get(Dynix *dynix, size_t index);
```

Returns the element at the specified index.

Indexed access is **O(1)**.

### Free

```c
void dynix_free(Dynix *dynix);
```

Frees the Dynix container and its stored elements.

Dynix currently assumes ownership of the stored data when `dynix_free()` is called.

## Planned Operations

Additional array operations are planned, including:

* Insert at index
* Remove at index
* Index shifting
* Element replacement
* Push / pop operations
* Additional element manipulation utilities

These operations will extend Dynix beyond simple append/get functionality while keeping the API lightweight.

## Design Goals

Dynix focuses on:

* Low overhead
* Simple data structures
* Predictable memory behavior
* Minimal abstraction
* C-native performance
* Both typed and untyped usage

The goal is not to recreate a high-level container framework, but to provide a small and practical dynamic-array primitive for C.

## License

Dynix is licensed under the **MIT License**.

See [`LICENSE`](LICENSE) for the full license text.
