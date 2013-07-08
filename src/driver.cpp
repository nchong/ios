#include "clwrapper.h"
#include "abstraction.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {
  unsigned N = std::stoi(argv[1]);
  printf("N = %d", N);

  // platform info
  std::cout << clinfo();

  // test data
  size_t ArraySize = N * sizeof(TYPE);
  TYPE *in  = (TYPE *)malloc(ArraySize);
  TYPE *out = (TYPE *)malloc(ArraySize);

  // initialise for device 0 on platform 0, with profiling off
  // this creates a context and command queue
  unsigned platform = 0;
  unsigned device = 0;
  bool profiling = false;
  CLWrapper clw(platform, device, profiling);

  // compile the OpenCL code
  const char *filename = "blelloch.cl";
  std::ostringstream oss;
  oss << "-I. -DNO_INVARIANTS -DABSTRACT -DN=" << N;
  cl_program program = clw.compile(filename, oss.str().c_str());

  // get kernel handle
  cl_kernel k = clw.create_kernel(program, "prefixsum");

  // create some memory objects on the device
  cl_mem d_in   = clw.dev_malloc(ArraySize, CL_MEM_READ_ONLY);
  cl_mem d_out  = clw.dev_malloc(ArraySize);

  // memcpy into these objects
  clw.memcpy_to_dev(d_in, ArraySize, in);

  // set kernel arguments
  clw.kernel_arg(k, d_in, d_out);

  // run the kernel
  cl_uint dim = 1;
  size_t global_work_size = N;
  size_t local_work_size  = N;
  clw.run_kernel(k, dim, &global_work_size, &local_work_size);

  // memcpy back the result
  clw.memcpy_from_dev(d_out, ArraySize, out);

  // print results
  for (unsigned i=0; i<N; i++) {
    printf("out[%d] = (%d,%d)\n", i, GET_LOWER(out[i]), GET_UPPER(out[i]));
  }

  // check results
  assert(out[0] == IDENTITY);
  for (unsigned i=1; i<N; i++) {
    assert(GET_LOWER(out[i]) == 0);
    assert(GET_UPPER(out[i]) == i);
  }
//for (unsigned i=0; i<N; i++) {
//  assert(GET_LOWER(out[i]) == 0);
//  assert(GET_UPPER(out[i]) == i+1);
//}
  printf("TEST PASSED\n");

  // cleanup
  free(in);
  free(out);
  // device objects will be auto-deleted when clw is destructed
  return 0;
}

