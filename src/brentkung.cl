//pass
//--local_size=4096 --num_groups=1 -DABSTRACT -DN=8192 --no-infer blelloch.cl

#include "abstraction.h"

__kernel void prefixsum(__global TYPE *input, __global TYPE *output) {
  __local TYPE result[N];

  unsigned offset;
  unsigned tid = get_local_id(0);

  if (tid < N/2) {
    result[2*tid] = READ_INITIAL(input, 2*tid);
    result[2*tid+1] = READ_INITIAL(input, 2*tid+1);
  }

  offset = 1;
  for (
    unsigned d=N/2;
#ifndef NO_INVARIANTS
    __invariant((offset & (offset-1)) == 0),
    __invariant((d & (d-1)) == 0),
    __invariant(((d == 0) & (offset == N)) | ((d * offset) == (N/2))),
    __invariant(__no_write(input)),
    __invariant(__implies(__read(result) & (offset == N), tid == 0)),
    __invariant(__implies(__write(result) & (offset == N), tid == 0)),
#endif
    d > 0;
    d >>= 1) {
    barrier(CLK_LOCAL_MEM_FENCE);
    if (tid < d) {
      unsigned ai = offset * (2 * tid + 1) - 1;
      unsigned bi = offset * (2 * tid + 2) - 1;
      result[bi] = OPERATOR(result[ai], result[bi]);
    }
    offset <<= 1;
  }

  for (
    unsigned d = 2;
#ifndef NO_INVARIANTS
    __invariant(__no_write(input)),
    __invariant((offset & (offset-1)) == 0),
    __invariant((d & (d-1)) == 0),
    __invariant(d * offset == (2*N)),
#endif
    d < N;
    d <<= 1) {
    offset >>= 1;
    barrier(CLK_LOCAL_MEM_FENCE);
    if (tid < (d - 1)) {
      unsigned ai = (offset * (tid + 1)) - 1;
      unsigned bi = ai + (offset >> 1);
      result[bi] = OPERATOR(result[ai], result[bi]);
    }
  }

  if (tid < N/2) {
    output[2*tid] = result[2*tid];
    output[2*tid+1] = result[2*tid+1];
  }

#ifdef FORCE_FAIL
  __assert(false);
#endif
}
