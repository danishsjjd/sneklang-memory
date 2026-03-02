#include "stack.h"

#include <stdlib.h>

stack_mt* stack_new(int capacity) {
  stack_mt* stack = malloc(sizeof(stack_mt));

  if (stack == NULL) {
    return NULL;
  }

  stack->capacity = capacity;
  stack->count = 0;
  stack->data = malloc(sizeof(void*) * capacity);

  if (stack->data == NULL) {
    free(stack);
    return NULL;
  }

  return stack;
}

stack_mt* stack_push(stack_mt* stack, void* item) {
  if (stack->count == stack->capacity) {
    stack->capacity *= 2;
    void** temp = realloc(stack->data, sizeof(void*) * (stack->capacity));

    if (temp == NULL) {
      stack->capacity /= 2;
      return NULL;
    }

    stack->data = temp;
  }

  stack->data[stack->count] = item;

  stack->count += 1;
  return stack;
}

void* stack_pop(stack_mt* stack) {
  if (stack->count == 0) {
    return NULL;
  }

  stack->count -= 1;
  return stack->data[stack->count];
}

void stack_remove_nulls(stack_mt* stack) {
  if (stack == NULL || stack->count == 0) {
    return;
  }

  int write_index = 0;
  for (int read_index = 0; read_index < stack->count; read_index++) {
    if (stack->data[read_index] != NULL) {
      stack->data[write_index] = stack->data[read_index];
      write_index += 1;
    }
  }

  stack->count = write_index;
}

void stack_free(stack_mt* stack) {
  if (stack == NULL) {
    return;
  }

  if (stack->data != NULL) {
    free(stack->data);
  }

  free(stack);
}
