#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

int main() {
  stack_mt* stack = stack_new(5);

  stack_push(stack, 0);
  stack_push(stack, (void*)1);
  stack_push(stack, (void*)2);
  stack_push(stack, (void*)3);
  stack_push(stack, (void*)4);
  printf("stack->capacity: %d\n", stack->capacity);
  stack_push(stack, (void*)5);
  printf("After pushing 5th element: %d\n", stack->capacity);
  printf("stack->count: %d\n", stack->count);
  printf("popping: %d\n", (int)stack_pop(stack));
  printf("After popping: %d\n", stack->count);

  return 0;
}
