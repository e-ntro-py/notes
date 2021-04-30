#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A homogeneous variabled-size vector.
struct Vec
{
    // Size of a single element in the vector.
    const size_t elem_size;
    // The number of elements currently in the vector.
    size_t elem_cnt;
    // The capacity of the vector, the maximum number of elements that can be stored in the current
    // memory allocation.
    size_t capacity;
    // The memory allocation backing the vector.
    void *ptr;
};

// Create a homogeneous variable-size vector in which each element is `elem_size` in bytes.
struct Vec new_vec(size_t elem_size)
{
    struct Vec vec = {.elem_size = elem_size, .elem_cnt = 0, .capacity = 0, .ptr = NULL};
    return vec;
}

// Read `elem_cnt` bytes from the given index into the vector, write the bytes to elem. The first
// `elem_cnt` bytes in elem must be writable.
//
// # Return values:
// 0    successful get
// -1   index out of bound
int get(struct Vec *vec, size_t idx, void *elem)
{
    if (idx >= vec->elem_cnt)
    {
        return -1;
    }

    void *ptr = vec->ptr + idx * vec->elem_size;
    memcpy(elem, ptr, vec->elem_size);
}

// Push a new element into the vector, reallocate the vector to larger size when necessary. The
// first `vec->elem_size` bytes of `elem` must be readable.
//
// # Return values:
// 0:   successful push
// -1:  allocation failure
int push(struct Vec *vec, void *elem)
{
    if (vec->elem_cnt >= vec->capacity)
    {
        size_t new_capacity = vec->capacity * 2;
        if (new_capacity < 16)
        {
            new_capacity = 16;
        }

        // no idea how to test for integer overflow
        void *rc = realloc(vec->ptr, new_capacity * vec->elem_size);
        if (rc == NULL)
        {
            return -1;
        }

        vec->ptr = rc;
        vec->capacity = new_capacity;
    }

    memcpy(vec->ptr + vec->elem_size * vec->elem_cnt, elem, vec->elem_size);
    vec->elem_cnt += 1;
    return 0;
}

// Pop the last value in the vector to `elem`. The first `elem_cnt` bytes in `elem` must be
// writable.
//
// # Return values:
// 0    successful pop
// -1   vector is empty
int pop(struct Vec *vec, void *elem)
{
    if (vec->elem_cnt <= 0)
    {
        return -1;
    }

    get(vec, vec->elem_cnt - 1, elem);
    vec->elem_cnt -= 1;
}

// Drop the content of the vector, free the memory allocated to it.
void drop(struct Vec *vec)
{
    free(vec->ptr);
    vec->ptr = NULL;
    vec->elem_cnt = 0;
    vec->capacity = 0;
}

int main(int argc, char const *argv[])
{
    struct Vec vec = new_vec(sizeof(int));
    for (int i = 0; i < 10; i++)
    {
        push(&vec, &i);
    }

    for (size_t i = 0; i < 10; i++)
    {
        int v;
        get(&vec, i, &v);
        printf("%d\n", v);
    }

    for (int i = 0; i < 10; i++)
    {
        int v;
        pop(&vec, &v);
        printf("%d\n", v);
    }

    drop(&vec);
}
