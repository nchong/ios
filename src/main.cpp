#define ABSTRACT
#include "clwrapper.h"
#include "abstraction.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

int main(int argc, char **argv) {

  // command line arguments
  if (argc != 3) {
    printf("Usage: %s N <kernel.cl>\n", argv[0]);
    return 1;
  }

  unsigned N = atoi(argv[1]);
  if ((N & (N-1)) != 0) {
    printf("Error: N must be a power of two\n");
    return 1;
  }

  // kernel specific parameters
  size_t global_work_size;
  size_t local_work_size;
  bool is_exclusive;
  std::string kernel = std::string(argv[2]);
  if (kernel == "sklansky.cl") {
    global_work_size = N/2;
    local_work_size  = N/2;
    is_exclusive = false;
  } else if (kernel == "koggestone.cl") {
    global_work_size = N;
    local_work_size  = N;
    is_exclusive = false;
  } else if (kernel == "brentkung.cl") {
    global_work_size = N;
    local_work_size  = N;
    is_exclusive = false;
  } else if (kernel == "blelloch.cl") {
    global_work_size = N;
    local_work_size  = N;
    is_exclusive = true;
  } else {
    printf("Error: unrecognised kernel [%s]\n", kernel.c_str());
    return 1;
  }

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
  const char *filename = kernel.c_str();
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
  clw.run_kernel(k, dim, &global_work_size, &local_work_size);

  // memcpy back the result
  clw.memcpy_from_dev(d_out, ArraySize, out);

  // print results
  for (unsigned i=0; i<N; i++) {
    printf("out[%d] = (%d,%d)\n", i, GET_LOWER(out[i]), GET_UPPER(out[i]));
  }

  // check results
  if (is_exclusive) {
    assert(out[0] == IDENTITY);
    for (unsigned i=1; i<N; i++) {
      assert(GET_LOWER(out[i]) == 0);
      assert(GET_UPPER(out[i]) == i);
    }
  } else /* inclusive */ {
    for (unsigned i=0; i<N; i++) {
      assert(GET_LOWER(out[i]) == 0);
      assert(GET_UPPER(out[i]) == i+1);
    }
  }
  printf("TEST PASSED\n");

  // cleanup
  free(in);
  free(out);
  // device objects will be auto-deleted when clw is destructed
  return 0;
}

