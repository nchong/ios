#define ABSTRACT
#include "abstraction.h"

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

__kernel void check_abstract_exclusive(__global TYPE *output, __global unsigned *error) {
  unsigned i = get_global_id(0);
  if ((i == 0) && (output[i] != IDENTITY)) {
    *error = 1;
  } else if (output[i] != MAKE_PAIR(0, i)) {
    *error = 1;
  }
}
