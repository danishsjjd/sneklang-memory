#include <string.h>

#include "munit/munit.h"
#include "sneknew.h"
#include "snekobject.h"
#include "stack/stack.h"
#include "vm.h"

snek_object_t* snek_add(vm_t* vm, snek_object_t* a, snek_object_t* b);

static MunitResult test_stack_new_and_basic_push_pop(
    const MunitParameter params[], void* user_data) {
  (void)params;
  (void)user_data;

  stack_mt* stack = stack_new(2);
  munit_assert_not_null(stack);
  munit_assert_int(stack->capacity, ==, 2);
  munit_assert_int(stack->count, ==, 0);

  munit_assert_ptr_equal(stack_push(stack, (void*)1), stack);
  munit_assert_ptr_equal(stack_push(stack, (void*)2), stack);
  munit_assert_int(stack->count, ==, 2);

  munit_assert_ptr_equal(stack_push(stack, (void*)3), stack);
  munit_assert_int(stack->capacity, ==, 4);
  munit_assert_int(stack->count, ==, 3);

  munit_assert_ptr_equal(stack_pop(stack), (void*)3);
  munit_assert_ptr_equal(stack_pop(stack), (void*)2);
  munit_assert_ptr_equal(stack_pop(stack), (void*)1);
  munit_assert_null(stack_pop(stack));
  munit_assert_int(stack->count, ==, 0);

  stack_free(stack);
  return MUNIT_OK;
}

static MunitResult test_stack_remove_nulls_mixed(const MunitParameter params[],
                                                 void* user_data) {
  (void)params;
  (void)user_data;

  stack_mt* stack = stack_new(8);
  munit_assert_not_null(stack);

  stack_push(stack, (void*)10);
  stack_push(stack, NULL);
  stack_push(stack, (void*)20);
  stack_push(stack, NULL);
  stack_push(stack, (void*)30);

  stack_remove_nulls(stack);

  munit_assert_int(stack->count, ==, 3);
  munit_assert_ptr_equal(stack->data[0], (void*)10);
  munit_assert_ptr_equal(stack->data[1], (void*)20);
  munit_assert_ptr_equal(stack->data[2], (void*)30);

  stack_free(stack);
  return MUNIT_OK;
}

static MunitResult test_stack_remove_nulls_empty_and_null_input(
    const MunitParameter params[], void* user_data) {
  (void)params;
  (void)user_data;

  stack_remove_nulls(NULL);

  stack_mt* stack = stack_new(4);
  munit_assert_not_null(stack);
  munit_assert_int(stack->count, ==, 0);

  stack_remove_nulls(stack);
  munit_assert_int(stack->count, ==, 0);

  stack_free(stack);
  return MUNIT_OK;
}

static MunitResult test_new_object_constructors(const MunitParameter params[],
                                                void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  snek_object_t* i = new_snek_integer(vm, 42);
  snek_object_t* f = new_snek_float(vm, 3.5f);
  snek_object_t* s = new_snek_string(vm, "hello");

  munit_assert_not_null(i);
  munit_assert_not_null(f);
  munit_assert_not_null(s);

  munit_assert_int(i->kind, ==, INTEGER);
  munit_assert_int(i->data.v_int, ==, 42);

  munit_assert_int(f->kind, ==, FLOAT);
  munit_assert_float(f->data.v_float, ==, 3.5f);

  munit_assert_int(s->kind, ==, STRING);
  munit_assert_string_equal(s->data.v_string, "hello");
  munit_assert_ptr_not_equal(s->data.v_string, "hello");

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_new_vector3_null_arg_returns_null(
    const MunitParameter params[], void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  snek_object_t* x = new_snek_integer(vm, 1);
  snek_object_t* y = new_snek_integer(vm, 2);
  munit_assert_not_null(x);
  munit_assert_not_null(y);

  munit_assert_null(new_snek_vector3(vm, x, y, NULL));
  munit_assert_null(new_snek_vector3(vm, NULL, y, x));
  munit_assert_null(new_snek_vector3(vm, x, NULL, y));

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_array_set_get_edge_cases(const MunitParameter params[],
                                                 void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  snek_object_t* array = new_snek_array(vm, 2);
  snek_object_t* value = new_snek_integer(vm, 99);
  snek_object_t* not_array = new_snek_integer(vm, 7);

  munit_assert_not_null(array);
  munit_assert_not_null(value);
  munit_assert_not_null(not_array);

  munit_assert_false(snek_array_set(NULL, 0, value));
  munit_assert_false(snek_array_set(array, 0, NULL));
  munit_assert_false(snek_array_set(not_array, 0, value));
  munit_assert_false(snek_array_set(array, 2, value));
  munit_assert_true(snek_array_set(array, 1, value));

  munit_assert_null(snek_array_get(NULL, 0));
  munit_assert_null(snek_array_get(not_array, 0));
  munit_assert_null(snek_array_get(array, 2));
  munit_assert_ptr_equal(snek_array_get(array, 1), value);

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_snek_add_null_and_type_mismatch(
    const MunitParameter params[], void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  snek_object_t* i = new_snek_integer(vm, 2);
  snek_object_t* s = new_snek_string(vm, "x");
  munit_assert_not_null(i);
  munit_assert_not_null(s);

  munit_assert_null(snek_add(vm, NULL, i));
  munit_assert_null(snek_add(vm, i, NULL));
  munit_assert_null(snek_add(vm, i, s));

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_snek_add_numeric(const MunitParameter params[],
                                         void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  snek_object_t* i1 = new_snek_integer(vm, 2);
  snek_object_t* i2 = new_snek_integer(vm, 3);
  snek_object_t* f1 = new_snek_float(vm, 0.5f);
  snek_object_t* f2 = new_snek_float(vm, 1.5f);

  snek_object_t* ii = snek_add(vm, i1, i2);
  snek_object_t* ifv = snek_add(vm, i1, f1);
  snek_object_t* fi = snek_add(vm, f1, i1);
  snek_object_t* ff = snek_add(vm, f1, f2);

  munit_assert_not_null(ii);
  munit_assert_not_null(ifv);
  munit_assert_not_null(fi);
  munit_assert_not_null(ff);

  munit_assert_int(ii->kind, ==, INTEGER);
  munit_assert_int(ii->data.v_int, ==, 5);

  munit_assert_int(ifv->kind, ==, FLOAT);
  munit_assert_float(ifv->data.v_float, ==, 2.5f);

  munit_assert_int(fi->kind, ==, FLOAT);
  munit_assert_float(fi->data.v_float, ==, 2.5f);

  munit_assert_int(ff->kind, ==, FLOAT);
  munit_assert_float(ff->data.v_float, ==, 2.0f);

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_snek_add_string_concat(const MunitParameter params[],
                                               void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  snek_object_t* a = new_snek_string(vm, "hello");
  snek_object_t* b = new_snek_string(vm, " world");
  snek_object_t* out = snek_add(vm, a, b);

  munit_assert_not_null(out);
  munit_assert_int(out->kind, ==, STRING);
  munit_assert_string_equal(out->data.v_string, "hello world");

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_snek_add_vector3(const MunitParameter params[],
                                         void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  snek_object_t* a =
      new_snek_vector3(vm, new_snek_integer(vm, 1), new_snek_integer(vm, 2),
                       new_snek_integer(vm, 3));
  snek_object_t* b =
      new_snek_vector3(vm, new_snek_integer(vm, 4), new_snek_integer(vm, 5),
                       new_snek_integer(vm, 6));

  munit_assert_not_null(a);
  munit_assert_not_null(b);

  snek_object_t* out = snek_add(vm, a, b);
  munit_assert_not_null(out);
  munit_assert_int(out->kind, ==, VECTOR3);

  munit_assert_int(out->data.v_vector3.x->kind, ==, INTEGER);
  munit_assert_int(out->data.v_vector3.y->kind, ==, INTEGER);
  munit_assert_int(out->data.v_vector3.z->kind, ==, INTEGER);

  munit_assert_int(out->data.v_vector3.x->data.v_int, ==, 5);
  munit_assert_int(out->data.v_vector3.y->data.v_int, ==, 7);
  munit_assert_int(out->data.v_vector3.z->data.v_int, ==, 9);

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_snek_add_array_concat(const MunitParameter params[],
                                              void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  snek_object_t* a = new_snek_array(vm, 2);
  snek_object_t* b = new_snek_array(vm, 1);
  snek_object_t* v1 = new_snek_integer(vm, 10);
  snek_object_t* v2 = new_snek_integer(vm, 20);
  snek_object_t* v3 = new_snek_integer(vm, 30);

  munit_assert_true(snek_array_set(a, 0, v1));
  munit_assert_true(snek_array_set(a, 1, v2));
  munit_assert_true(snek_array_set(b, 0, v3));

  snek_object_t* out = snek_add(vm, a, b);
  munit_assert_not_null(out);
  munit_assert_int(out->kind, ==, ARRAY);
  munit_assert_size(out->data.v_array.size, ==, 3);
  munit_assert_ptr_equal(out->data.v_array.elements[0], v1);
  munit_assert_ptr_equal(out->data.v_array.elements[1], v2);
  munit_assert_ptr_equal(out->data.v_array.elements[2], v3);

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_vm_collect_garbage_sweeps_unreferenced(
    const MunitParameter params[], void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  frame_t* frame = vm_new_frame(vm);
  munit_assert_not_null(frame);

  snek_object_t* keep = new_snek_integer(vm, 1);
  snek_object_t* remove = new_snek_integer(vm, 2);
  munit_assert_not_null(keep);
  munit_assert_not_null(remove);

  frame_reference_object(frame, keep);

  munit_assert_size(vm->objects->count, ==, 2);
  vm_collect_garbage(vm);

  munit_assert_size(vm->objects->count, ==, 1);
  munit_assert_ptr_equal(vm->objects->data[0], keep);
  munit_assert_false(keep->is_marked);

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_vm_collect_garbage_traces_nested_children(
    const MunitParameter params[], void* user_data) {
  (void)params;
  (void)user_data;

  vm_t* vm = vm_new();
  munit_assert_not_null(vm);

  frame_t* frame = vm_new_frame(vm);
  munit_assert_not_null(frame);

  snek_object_t* x = new_snek_integer(vm, 1);
  snek_object_t* y = new_snek_integer(vm, 2);
  snek_object_t* z = new_snek_integer(vm, 3);
  snek_object_t* vec = new_snek_vector3(vm, x, y, z);
  snek_object_t* orphan = new_snek_integer(vm, 999);

  munit_assert_not_null(vec);
  munit_assert_not_null(orphan);

  frame_reference_object(frame, vec);

  munit_assert_size(vm->objects->count, ==, 5);
  vm_collect_garbage(vm);

  munit_assert_size(vm->objects->count, ==, 4);

  bool saw_vec = false;
  bool saw_x = false;
  bool saw_y = false;
  bool saw_z = false;
  for (int i = 0; i < vm->objects->count; i++) {
    snek_object_t* obj = vm->objects->data[i];
    if (obj == vec) saw_vec = true;
    if (obj == x) saw_x = true;
    if (obj == y) saw_y = true;
    if (obj == z) saw_z = true;
  }

  munit_assert_true(saw_vec);
  munit_assert_true(saw_x);
  munit_assert_true(saw_y);
  munit_assert_true(saw_z);

  vm_free(vm);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {(char*)"/stack/new-push-pop", test_stack_new_and_basic_push_pop, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/stack/remove-nulls-mixed", test_stack_remove_nulls_mixed, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/stack/remove-nulls-empty-and-null",
     test_stack_remove_nulls_empty_and_null_input, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},

    {(char*)"/new/constructors", test_new_object_constructors, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/new/vector3-null-arg", test_new_vector3_null_arg_returns_null,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/array/set-get-edge-cases", test_array_set_get_edge_cases, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {(char*)"/add/null-and-type-mismatch", test_snek_add_null_and_type_mismatch,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/add/numeric", test_snek_add_numeric, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/add/string-concat", test_snek_add_string_concat, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/add/vector3", test_snek_add_vector3, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/add/array-concat", test_snek_add_array_concat, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},

    {(char*)"/gc/sweep-unreferenced",
     test_vm_collect_garbage_sweeps_unreferenced, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {(char*)"/gc/trace-nested", test_vm_collect_garbage_traces_nested_children,
     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {
    (char*)"/sneklang-memory", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE,
};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
