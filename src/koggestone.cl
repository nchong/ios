//pass
//--local_size=4096 --num_groups=1 -DABSTRACT -DN=8192 --no-infer koggestone.cl

#include "abstraction.h"

__kernel void prefixsum(__global TYPE *input, __global TYPE *output) {
  __local TYPE result[N];

  TYPE temp;
  unsigned tid = get_local_id(0);
  unsigned gid = get_global_id(0);
  result[tid] = input[gid];

  barrier(CLK_LOCAL_MEM_FENCE);

  for (unsigned offset = 1;
#ifndef NO_INVARIANTS
        __invariant(__no_read(output)), __invariant(__no_write(output)),
        __invariant(__no_read(result)), __invariant(__no_write(result)),
        __invariant(0 <= offset),
        __invariant(__is_pow2(offset)),
        __invariant(offset <= N),
#endif
      offset < N;
      offset *= 2) 
  {
    if (tid >= offset)
    {
      temp = result[tid-offset];
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    if (tid >= offset)
    {
      result[tid] = OPERATOR(temp, result[tid]);
    }

    barrier(CLK_LOCAL_MEM_FENCE);
  }

  output[gid] = result[tid];

#ifdef FORCE_FAIL
  __assert(false);
#endif
}
