#define ABSTRACT
#include "abstraction.h"

__kernel void init_abstract(__global TYPE *input) {
  unsigned i = get_global_id(0);  
  input[i] = MAKE_PAIR(i, i+1);
}
