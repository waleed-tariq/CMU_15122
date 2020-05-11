#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "lib/xalloc.h"
#include "lib/stack.h"
#include "lib/contracts.h"
#include "lib/c0v_stack.h"
#include "lib/c0vm.h"
#include "lib/c0vm_c0ffi.h"
#include "lib/c0vm_abort.h"

/* call stack frames */
typedef struct frame_info frame;
struct frame_info {
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* Function body */
  size_t pc;     /* Program counter */
  c0_value *V;   /* The local variables */
};

int pop_int(c0v_stack_t S)
{
  return val2int(c0v_pop(S));
}

void *pop_ptr(c0v_stack_t S)
{
  return val2ptr(c0v_pop(S));
}

void push_int(c0v_stack_t S, int x)
{
  c0v_push(S, int2val(x));
  return;
}

void push_ptr(c0v_stack_t S, void *x)
{
  c0v_push(S, ptr2val(x));
  return;
}

int execute(struct bc0_file *bc0) {
  REQUIRES(bc0 != NULL);

  /* Variables */
  c0v_stack_t S; /* Operand stack of C0 values */
  ubyte *P;      /* Array of bytes that make up the current function */
  size_t pc;     /* Current location within the current byte array P */
  c0_value *V;   /* Local variables (you won't need this till Task 2) */
  (void) V;

  S = c0v_stack_new();
  P = bc0->function_pool[0].code;
  pc = 0;
  V = xmalloc(sizeof(c0_value) * bc0->function_pool[0].num_vars);

  /* The call stack, a generic stack that should contain pointers to frames */
  /* You won't need this until you implement functions. */
  gstack_t callStack;
  callStack = stack_new();

  while (true) {

#ifdef DEBUG
    /* You can add extra debugging information here */
    fprintf(stderr, "Opcode %x -- Stack size: %zu -- PC: %zu\n",
            P[pc], c0v_stack_size(S), pc);
#endif

    switch (P[pc]) {

    /* Additional stack operation: */

    case POP: {
      pc++;
      c0v_pop(S);
      break;
    }

    case DUP: {
      pc++;
      c0_value v = c0v_pop(S);
      c0v_push(S,v);
      c0v_push(S,v);
      break;
    }

    case SWAP: {
      pc++;
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      c0v_push(S, v1);
      c0v_push(S, v2);
      break;
    }


    /* Returning from a function.
     * This currently has a memory leak! You will need to make a slight
     * change for the initial tasks to avoid leaking memory.  You will
     * need to be revise it further when you write INVOKESTATIC. */

    case RETURN: {
      c0_value retval = c0v_pop(S);
      assert(c0v_stack_empty(S));
#ifdef DEBUG
      //fprintf(stderr, "Returning %d from execute()\n", retval);
#endif
      // Free everything before returning from the execute function!
      free(V);
      c0v_stack_free(S);
      if (stack_empty(callStack)) 
      {
        stack_free(callStack, NULL);
        return val2int(retval);
      }
      else
      {
        frame* f = pop(callStack);
        S = f->S;
        P = f->P;
        V = f->V;
        pc = f->pc;
        free(f);
        c0v_push(S, retval);
        break;
      }
    }


    /* Arithmetic and Logical operations */

    case IADD: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      int res = v1 + v2;
      push_int(S, res);
      break;
    }

    case ISUB: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      int res = v2 - v1;
      push_int(S, res);
      break;
    }

    case IMUL: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      int res = v1 * v2;
      push_int(S, res);
      break;
    }

    case IDIV: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      if (v1 == 0) c0_arith_error("Cannot divide by 0");
      else if (v2 == INT_MIN && v1 == -1) c0_arith_error("Cannot divide int_min by -1");
      else {
      	int res = v2 / v1;
        push_int(S, res);
      }
      break;
    }

    case IREM: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      if (v1 == 0) c0_arith_error("Cannot divide by 0");
      else if (v2 == INT_MIN && v1 == -1) c0_arith_error("Cannot divide int_min by -1");
      else {
      	int res = v2 % v1;
        push_int(S, res);
      }
      break;
    }

    case IAND:
    {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      int res = v1 & v2;
      push_int(S, res);
      break;
    }

    case IOR: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      int res = v1 | v2;
      push_int(S, res);
      break;
    }

    case IXOR: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      int res = v2 ^ v1;
      push_int(S, res);
      break;
    }

    case ISHL: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      if (v1 < 0) c0_arith_error("Cannot shift negative amount");
      else if (v1 >= 32) c0_arith_error("Cannot shift greater than 32 bits");
      else {
      	int res = v2 << v1;
        push_int(S, res);
      }
      break;
    }

    case ISHR: {
      pc++;
      int v1 = pop_int(S);
      int v2 = pop_int(S);
      if (v1 < 0) c0_arith_error("Cannot shift negative amount");
      else if (v1 >= 32) c0_arith_error("Cannot shift greater than 32 bits");
      else {
      	int res = v2 >> v1;
        push_int(S, res);
      }
      break;
    }


    /* Pushing constants */

    case BIPUSH: {
      pc++;
      int x = (int) (int8_t)P[pc];
      push_int(S, x);
      pc++;
      break;
    }

    case ILDC: {
      pc++;
      uint32_t c1 = (uint32_t)(P[pc]);
      c1 = c1 << 8;
      pc++;
      uint32_t c2 = (uint32_t) P[pc];
      uint32_t index = c1 | c2;
      if (index > (uint32_t) bc0->int_count) c0_arith_error("not enough values");
      int x = bc0->int_pool[index];
      push_int(S, x);
      pc++;
      break;
    }

    case ALDC: {
      pc++;
      uint32_t c1 = (uint32_t)(P[pc]);
      c1 = c1 << 8;
      pc++;
      uint32_t c2 = (uint32_t) P[pc];
      uint32_t index = c1 | c2;
      if (index > (uint32_t) bc0->string_count) c0_arith_error("not enough values");
      char* x = &bc0->string_pool[index];
      void* z = (void*) x;
      push_ptr(S, z);
      pc++;
      break;
    }

    case ACONST_NULL: {
      pc++;
      push_ptr(S, NULL);
      break;
    }


    /* Operations on local variables */

    case VLOAD: {
      pc++;
      c0v_push(S, V[P[pc]]);
      pc++;
      break;
    }

    case VSTORE: {
      pc++;
      V[P[pc]] = c0v_pop(S);
      pc++;
      break;
    }


    /* Assertions and errors */

    case ATHROW: {
      pc++;
      c0_user_error(pop_ptr(S));
      break;
    }

    case ASSERT: {
      pc++;
      void *a = pop_ptr(S);
      if (pop_int(S) == 0) c0_assertion_failure(a);
      break;
    }


    /* Control flow operations */

    case NOP: {
      pc++;
      break;
    }

    case IF_CMPEQ: {
      pc++;
      if (val_equal(c0v_pop(S), c0v_pop(S)))
      {
        uint16_t o1 = (uint16_t)(P[pc]);
        o1 = o1 << 8;
        pc++;
        uint16_t o2 = (uint16_t) P[pc];
        uint16_t tmp = o1 | o2;
        int16_t index = (int16_t) tmp;
        pc += index - 2;
        break;
      }
      else pc += 2;
      break;
    }

    case IF_CMPNE: {
      pc++;
      if (!val_equal(c0v_pop(S), c0v_pop(S)))
      {
        uint16_t o1 = (uint16_t)(P[pc]);
        o1 = o1 << 8;
        pc++;
        uint16_t o2 = (uint16_t) P[pc];
        uint16_t tmp = o1 | o2;
        int16_t index = (int16_t) tmp;
        pc += index - 2;
        break;
      }
      else pc += 2;
      break;
    }

    case IF_ICMPLT: {
      pc++;
      if (pop_int(S) > pop_int(S))
      {
        uint16_t o1 = (uint16_t)(P[pc]);
        o1 = o1 << 8;
        pc++;
        uint16_t o2 = (uint16_t) P[pc];
        uint16_t tmp = o1 | o2;
        int16_t index = (int16_t) tmp;
        pc += index - 2;
        break;
      }
      else pc += 2;
      break;
    }

    case IF_ICMPGE: {
      pc++;
      if (pop_int(S) <= pop_int(S))
      {
        uint16_t o1 = (uint16_t)(P[pc]);
        o1 = o1 << 8;
        pc++;
        uint16_t o2 = (uint16_t) P[pc];
        uint16_t tmp = o1 | o2;
        int16_t index = (int16_t) tmp;
        pc += index - 2;
        break;
      }
      else pc += 2;
      break;
    }

    case IF_ICMPGT: {
      pc++;
      if (pop_int(S) < pop_int(S))
      {
        uint16_t o1 = (uint16_t)(P[pc]);
        o1 = o1 << 8;
        pc++;
        uint16_t o2 = (uint16_t) P[pc];
        uint16_t tmp = o1 | o2;
        int16_t index = (int16_t) tmp;
        pc += index - 2;
        break;
      }
      else pc += 2;
      break;
    }

    case IF_ICMPLE: {
      pc++;
      if (pop_int(S) >= pop_int(S))
      {
        uint16_t o1 = (uint16_t)(P[pc]);
        o1 = o1 << 8;
        pc++;
        uint16_t o2 = (uint16_t) P[pc];
        uint16_t tmp = o1 | o2;
        int16_t index = (int16_t) tmp;
        pc += index - 2;
        break;
      }
      else pc += 2;
      break;
    }

    case GOTO: {
      pc++;
      uint16_t o1 = (uint16_t)(P[pc]);
      o1 = o1 << 8;
      pc++;
      uint16_t o2 = (uint16_t) P[pc];
      uint16_t tmp = o1 | o2;
      int16_t index = (int16_t) tmp;
      pc += index - 2;
      break;
    }


    /* Function call operations: */

    case INVOKESTATIC: {
      pc++;
      uint16_t c1 = (uint16_t)(P[pc]);
      c1 = c1 << 8;
      pc++;
      uint16_t c2 = (uint16_t) P[pc];
      uint16_t index = c1 | c2;
      pc++;

      struct function_info* fi = &bc0->function_pool[index];

      frame* stack_frame = xmalloc(sizeof(frame));
      stack_frame->S = S;
      stack_frame->P = P;
      stack_frame->pc = pc;
      stack_frame->V = V;

      push(callStack, (void*)stack_frame);

      int length = (int) fi->num_vars;

      V = xmalloc(sizeof(c0_value) * length);
      int num_args = (int) fi->num_args;
      for (int i = num_args - 1; i >= 0; i--)
      {
        V[i] = c0v_pop(S);
      }

      S = c0v_stack_new();
      P = fi->code;
      pc = 0;
      break;
    }

    case INVOKENATIVE: {
      pc++;
      uint16_t c1 = (uint16_t)(P[pc]);
      c1 = c1 << 8;
      pc++;
      uint16_t c2 = (uint16_t) P[pc];
      uint16_t index = c1 | c2;

      int num_args = (int) bc0->native_pool[index].num_args;
      c0_value A[num_args];

      for (int i = num_args - 1; i >= 0; i--) 
      {
        A[i] = c0v_pop(S);
      }

      uint16_t func_index = bc0->native_pool[index].function_table_index;

      c0_value res = (*native_function_table[func_index])(A);

      c0v_push(S, res);
      pc++;
      break;
    }


    /* Memory allocation operations: */

    case NEW: {
      pc++;
      int s = (int)(int8_t)P[pc];
      void *n = xmalloc(sizeof(s));
      push_ptr(S, n);
      pc++;
      break;
    }

    case NEWARRAY: {
      pc++;
      int elt_size = (int)(int8_t)P[pc];
      int n = pop_int(S);
      c0_array* a = xmalloc(sizeof(c0_array));
      a->count = n;
      a->elt_size = elt_size;
      a->elems = xcalloc(n, sizeof(void*));
      push_ptr(S, (void*)a);
      pc++;
      break;
    }

    case ARRAYLENGTH: {
      c0_array* a = (c0_array*) pop_ptr(S);
      if (a == NULL) c0_memory_error("NULL pointer");
      push_int(S, a->count);
      pc++;
      break;
    }


    /* Memory access operations: */

    case AADDF: {
      pc++;
      ubyte f = (ubyte)P[pc];
      char *a = (char*)pop_ptr(S);
      if (a == NULL) c0_memory_error("NULL pointer");
      push_ptr(S, (void*)(a+f));
      pc++;
      break;
    }

    case AADDS: {
      int i = pop_int(S);
      c0_array* a = (c0_array*) pop_ptr(S);
      if (i >= a->count && i >= 0) c0_memory_error("invalid index");
      char *s = (char*)a->elems;
      void *res = (s + (a->elt_size) * i);
      push_ptr(S, res);
      pc++;
      break;
    }

    case IMLOAD: {
      int *a = (int*) pop_ptr(S);
      if (a == NULL) c0_memory_error("NULL pointer");
      int x = *a;
      push_int(S, x);
      pc++;
      break;
    }

    case IMSTORE: {
      int x = pop_int(S);
      int *a = (int*) pop_ptr(S);
      if (a == NULL) c0_memory_error("NULL pointer");
      *a = x;
      pc++;
      break;
    }

    case AMLOAD: {
      void **a = pop_ptr(S);
      if (a == NULL) c0_memory_error("NULL pointer");
      void *b = *a;
      push_ptr(S, b);
      pc++;
    }

    case AMSTORE: {
      void *a = pop_ptr(S);
      void **b = pop_ptr(S);
      if (a == NULL || b == NULL) c0_memory_error("NULL pointer");
      *b = a;
      pc++;
      break;
    }

    case CMLOAD: {
      int *a = (int*)pop_ptr(S);
      if (a == NULL) c0_memory_error("NULL pointer");
      int x = (int)(int32_t) *a;
      push_int(S, x);
      pc++;
      break;
    }

    case CMSTORE: {
      int x = pop_int(S);
      char *a = (char*)pop_ptr(S);
      if (a == NULL) c0_memory_error("NULL pointer");
      *a = x & 0x7f;
      pc++;
      break;
    }

    default:
      fprintf(stderr, "invalid opcode: 0x%02x\n", P[pc]);
      abort();
    }
  }

  /* cannot get here from infinite loop */
  assert(false);
}
