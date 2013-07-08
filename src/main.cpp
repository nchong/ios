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
  if (argc != 4) {
    printf("Usage: %s nelements-per-group ngroups <kernel.cl>\n", argv[0]);
    return 1;
  }

  // number of elements per group
  unsigned N = atoi(argv[1]);
  if ((N & (N-1)) != 0) {
    printf("Error: N must be a power of two\n");
    return 1;
  }

  // number of groups
  unsigned ngroups = atoi(argv[2]);

  // total number of elements
  unsigned nelements = N * ngroups;

  // kernel specific parameters
  size_t global_work_size;
  size_t local_work_size;
  bool is_exclusive;
  std::string kernel = std::string(argv[3]);
  if (kernel == "sklansky.cl") {
    global_work_size = nelements/2;
    local_work_size  = N/2;
    is_exclusive = false;
  } else if (kernel == "koggestone.cl") {
    global_work_size = nelements;
    local_work_size  = N;
    is_exclusive = false;
  } else if (kernel == "brentkung.cl") {
    global_work_size = nelements/2;
    local_work_size  = N/2;
    is_exclusive = false;
  } else if (kernel == "blelloch.cl") {
    global_work_size = nelements/2;
    local_work_size  = N/2;
    is_exclusive = true;
  } else {
    printf("Error: unrecognised kernel [%s]\n", kernel.c_str());
    return 1;
  }

  // platform info
  std::cout << clinfo();

  // test data
  size_t ArraySize = nelements * sizeof(TYPE);

  // initialise for device 0 on platform 0, with profiling off
  // this creates a context and command queue
  unsigned platform = 0;
  unsigned device = 0;
  bool profiling = false;
  CLWrapper clw(platform, device, profiling);

  // compile the OpenCL code
  std::ostringstream oss;
  oss << "-I. -DNO_INVARIANTS -DN=" << N << " -DINNER=" << kernel;
  cl_program meta = clw.compile("meta.cl", oss.str().c_str());

  // create some memory objects on the device
  cl_mem d_in    = clw.dev_malloc(ArraySize);
  cl_mem d_out   = clw.dev_malloc(ArraySize);
  cl_mem d_sum   = clw.dev_malloc(ngroups*sizeof(unsigned));
  cl_mem d_error = clw.dev_malloc(sizeof(unsigned));

  // initialise input
  {
  cl_kernel k = clw.create_kernel(meta, "init_abstract");
  clw.kernel_arg(k, d_in);
  cl_uint dim = 1;
  size_t global_work_size = nelements;
  size_t local_work_size = N;
  clw.run_kernel(k, dim, &global_work_size, &local_work_size);
  }

  // block level scan
  {
  cl_kernel k = clw.create_kernel(meta, "meta1");
  clw.kernel_arg(k, d_in, d_out, d_sum);
  cl_uint dim = 1;
  clw.run_kernel(k, dim, &global_work_size, &local_work_size);
  }

#ifdef PRINT_RESULTS
  TYPE *out = (TYPE *)malloc(ArraySize);

  // memcpy back the result
  clw.memcpy_from_dev(d_out, ArraySize, out);

  // print results
  for (unsigned i=0; i<N; ++i) {
    printf("out[%d] = (%d,%d)\n", i, GET_LOWER(out[i]), GET_UPPER(out[i]));
  }

  free(out);
#endif

  // check results
  {
    unsigned error = 0;
    clw.memcpy_to_dev(d_error, sizeof(unsigned), &error);
    cl_kernel k = clw.create_kernel(meta, is_exclusive ?  "check_abstract_exclusive" : "check_abstract_inclusive");
    clw.kernel_arg(k, d_out, d_error);
    cl_uint dim = 1;
    size_t global_work_size = nelements;
    size_t local_work_size = N;
    clw.run_kernel(k, dim, &global_work_size, &local_work_size);
    clw.memcpy_from_dev(d_error, sizeof(unsigned), &error);
    if (error == 0) {
      printf("TEST PASSED (%s)\n", is_exclusive ? "EXCL" : "INCL");
    } else {
      printf("TEST FAILED\n");
    }
  }

  // device objects will be auto-deleted when clw is destructed
  return 0;
}

