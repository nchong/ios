//pass
//--local_size=4096 --num_groups=1 -DABSTRACT -DN=8192 --no-infer sklansky.cl

#include "abstraction.h"

__kernel void prefixsum(__global TYPE *input, __global TYPE *output) {

  __local TYPE result[N];

  unsigned tid = get_local_id(0);
  
  result[2*tid] = READ_INITIAL(input, 2*tid);
  result[2*tid+1] = READ_INITIAL(input, 2*tid+1);
  
  for (unsigned d = 1;
#ifndef NO_INVARIANTS
       __invariant((d & (d-1)) == 0),
#endif
       d < N; d *= 2) {
      barrier(CLK_LOCAL_MEM_FENCE);
      unsigned block = 2 * (tid - (tid & (d - 1)));
      unsigned me = block + (tid & (d - 1)) + d;
      unsigned spine = block + d - 1;
      result[me] = OPERATOR(result[spine], result[me]);
  }
  
  barrier(CLK_LOCAL_MEM_FENCE);

  output[2*tid] = result[2*tid];
  output[2*tid+1] = result[2*tid+1];

#ifdef FORCE_FAIL
  __assert(false);
#endif
}
