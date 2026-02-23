#pragma once

typedef struct Stack {
  int capacity;
  int count;
  void** data;
} stack_mt;

stack_mt* stack_new(int capacity);
stack_mt* stack_push(stack_mt* stack, void* item);
void* stack_pop(stack_mt* stack);
void stack_free(stack_mt* stack);
