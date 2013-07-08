#define ABSTRACT
#include "abstraction.h"
#define __stringify_inner(x) #x
#define __stringify(x) __stringify_inner(x)
#include __stringify(INNER)

__kernel void init_abstract(__global TYPE *input) {
  unsigned i = get_global_id(0);  
  input[i] = MAKE_PAIR(i, i+1);
}

__kernel void check_abstract_inclusive(__global TYPE *output, __global unsigned *error) {
  unsigned i = get_global_id(0);
  if (output[i] != MAKE_PAIR(0, i+1)) {
    *error = 1;
  }
}

__kernel void meta1(__global TYPE *input, __global TYPE *output, __global TYPE *sum) {
  unsigned tid = get_local_id(0);
  unsigned bid = get_group_id(0);
  prefixsum(input, output);
  barrier(CLK_LOCAL_MEM_FENCE|CLK_GLOBAL_MEM_FENCE);
  if (tid == 0) {
    sum[bid] = output[(bid+1)*N-1];
  }
}

__kernel void meta2(__global TYPE *output, __global TYPE *sum) {
  unsigned bid = get_group_id(0);
  unsigned gid = get_global_id(0);
  output[gid] = OPERATOR(sum[bid], output[gid]);
}

__kernel void check_abstract_exclusive(__global TYPE *output, __global unsigned *error) {
  unsigned i = get_global_id(0);
  if ((i == 0) && (output[i] != IDENTITY)) {
    *error = 1;
  }
  if ((i > 0) && (output[i] != MAKE_PAIR(0, i))) {
    *error = 1;
  }
}
