/* To be able to use snprintf with any compiler including MSVC2008. */
#include <private_config.h>

#include "libcublas.h"
#include "dyn_load.h"
#include "gpuarray/error.h"

#define DEF_PROC(name, args) t##name *name
#define DEF_PROC_V2(name, args) DEF_PROC(name, args)
#define DEF_PROC_OPT(name, args) DEF_PROC(name, args)

#include "libcublas.fn"

#undef DEF_PROC_OPT
#undef DEF_PROC_V2
#undef DEF_PROC

#define STRINGIFY(X) #X

#define DEF_PROC(name, args)                      \
  name = (t##name *)ga_func_ptr(lib, #name, e);   \
  if (name == NULL) {                             \
    return e->code;                               \
  }

#define DEF_PROC_OPT(name, args)                \
  name = (t##name *)ga_func_ptr(lib, #name, e);

#define DEF_PROC_V2(name, args)                                   \
  name = (t##name *)ga_func_ptr(lib, STRINGIFY(name##_v2), e);    \
  if (name == NULL) {                                             \
    return e->code;                                               \
  }

static int loaded = 0;

int load_libcublas(int major, int minor, error *e) {
  void *lib;

  if (loaded)
    return GA_NO_ERROR;

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
  {
    char libname[64];
    int n;
    #ifdef DEBUG
    fprintf(stderr, "Loading cuBLAS %d.%d.\n", major, minor);
    #endif
    n = snprintf(libname, 64, "cublas64_%d%d.dll", major, minor);
    if (n < 0 || n >= 64)
      return error_set(e, GA_SYS_ERROR, "cublas library name too long.");
    lib = ga_load_library(libname, e);
  }
#else /* Unix */
#ifdef __APPLE__
  {
    char libname[128];
    int n;
    #ifdef DEBUG
    fprintf(stderr, "Loading cuBLAS %d.%d.\n", major, minor);
    #endif
    n = snprintf(libname, 128, "/Developer/NVIDIA/CUDA-%d.%d/lib/libcublas.dylib", major, minor);
    if (n < 0 || n >= 128)
      return error_set(e, GA_SYS_ERROR, "cublas library path too long.");
    lib = ga_load_library(libname, e);
  }
#else
  lib = ga_load_library("libcublas.so", e);
#endif
#endif
  if (lib == NULL)
    return e->code;

#include "libcublas.fn"

  loaded = 1;
  return GA_NO_ERROR;
}
