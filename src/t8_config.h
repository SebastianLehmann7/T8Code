#ifndef _SRC_T__CONFIG_H
#define _SRC_T__CONFIG_H 1
 
/* src/t8_config.h. Generated automatically at end of configure. */
/* src/pre_config.h.  Generated from pre_config.h.in by configure.  */
/* src/pre_config.h.in.  Generated from configure.ac by autoheader.  */

/* C compiler */
#ifndef T8_CC
#define T8_CC "mpicc"
#endif

/* C compiler flags */
#ifndef T8_CFLAGS
#define T8_CFLAGS "-Wall -g -O0"
#endif

/* C preprocessor */
#ifndef T8_CPP
#define T8_CPP "mpicc -E"
#endif

/* C preprocessor flags */
#ifndef T8_CPPFLAGS
#define T8_CPPFLAGS ""
#endif

/* DEPRECATED (use T8_ENABLE_CPPSTD instead) */
#ifndef T8_CPPSTD
#define T8_CPPSTD 1
#endif

/* Define to 1 if your C++ compiler doesn't accept -c and -o together. */
/* #undef CXX_NO_MINUS_C_MINUS_O */

/* DEPRECATED (use P4EST_ENABLE_DEBUG instead) */
#ifndef T8_DEBUG
#define T8_DEBUG 1
#endif

/* Undefine if: c++ standard library (optionally use
   --enable-cppstd=<CPP_LIBS>) */
#ifndef T8_ENABLE_CPPSTD
#define T8_ENABLE_CPPSTD 1
#endif

/* enable debug mode (assertions and extra checks) */
#ifndef T8_ENABLE_DEBUG
#define T8_ENABLE_DEBUG 1
#endif

/* Undefine if: use aligned malloc (optionally use --enable-memalign=<bytes>)
   */
#ifndef T8_ENABLE_MEMALIGN
#define T8_ENABLE_MEMALIGN 1
#endif

/* Define to 1 if we are using MPI */
#ifndef T8_ENABLE_MPI
#define T8_ENABLE_MPI 1
#endif

/* Define to 1 if we can use MPI_COMM_TYPE_SHARED */
#ifndef T8_ENABLE_MPICOMMSHARED
#define T8_ENABLE_MPICOMMSHARED 1
#endif

/* Define to 1 if we are using MPI I/O */
#ifndef T8_ENABLE_MPIIO
#define T8_ENABLE_MPIIO 1
#endif

/* Define to 1 if we are using MPI_Init_thread */
#ifndef T8_ENABLE_MPITHREAD
#define T8_ENABLE_MPITHREAD 1
#endif

/* Define to 1 if we can use MPI_Win_allocate_shared */
#ifndef T8_ENABLE_MPIWINSHARED
#define T8_ENABLE_MPIWINSHARED 1
#endif

/* enable OpenMP (optionally use --enable-openmp=<OPENMP_CFLAGS>) */
/* #undef ENABLE_OPENMP */

/* enable POSIX threads (optionally use --enable-pthread=<PTHREAD_CFLAGS>) */
/* #undef ENABLE_PTHREAD */

/* Define to dummy `main' function (if any) required to link to the Fortran
   libraries. */
/* #undef F77_DUMMY_MAIN */

/* Define to a macro mangling the given C identifier (in lower and upper
   case), which must not contain underscores, for linking with Fortran. */
#ifndef T8_F77_FUNC
#define T8_F77_FUNC(name,NAME) name ## _
#endif

/* As F77_FUNC, but for C identifiers containing underscores. */
#ifndef T8_F77_FUNC_
#define T8_F77_FUNC_(name,NAME) name ## _
#endif

/* Define to 1 if your Fortran compiler doesn't accept -c and -o together. */
/* #undef F77_NO_MINUS_C_MINUS_O */

/* Define to dummy `main' function (if any) required to link to the Fortran
   libraries. */
/* #undef FC_DUMMY_MAIN */

/* Define if F77 and FC dummy `main' functions are identical. */
/* #undef FC_DUMMY_MAIN_EQ_F77 */

/* Define to a macro mangling the given C identifier (in lower and upper
   case), which must not contain underscores, for linking with Fortran. */
#ifndef T8_FC_FUNC
#define T8_FC_FUNC(name,NAME) name ## _
#endif

/* As FC_FUNC, but for C identifiers containing underscores. */
#ifndef T8_FC_FUNC_
#define T8_FC_FUNC_(name,NAME) name ## _
#endif

/* Define to 1 if your Fortran compiler doesn't accept -c and -o together. */
/* #undef FC_NO_MINUS_C_MINUS_O */

/* Define to 1 if you have the `aligned_alloc' function. */
#ifndef T8_HAVE_ALIGNED_ALLOC
#define T8_HAVE_ALIGNED_ALLOC 1
#endif

/* Have we found function new. */
/* #undef HAVE_CPPSTD */

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef T8_HAVE_DLFCN_H
#define T8_HAVE_DLFCN_H 1
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef T8_HAVE_INTTYPES_H
#define T8_HAVE_INTTYPES_H 1
#endif

/* Have we found function pthread_create. */
#ifndef T8_HAVE_LPTHREAD
#define T8_HAVE_LPTHREAD 1
#endif

/* Have we found function lua_createtable. */
/* #undef HAVE_LUA */

/* Define to 1 if you have the <memory.h> header file. */
#ifndef T8_HAVE_MEMORY_H
#define T8_HAVE_MEMORY_H 1
#endif

/* Have we found function omp_get_thread_num. */
#ifndef T8_HAVE_OPENMP
#define T8_HAVE_OPENMP 1
#endif

/* Define to 1 if you have the `posix_memalign' function. */
#ifndef T8_HAVE_POSIX_MEMALIGN
#define T8_HAVE_POSIX_MEMALIGN 1
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef T8_HAVE_STDINT_H
#define T8_HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef T8_HAVE_STDLIB_H
#define T8_HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef T8_HAVE_STRINGS_H
#define T8_HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef T8_HAVE_STRING_H
#define T8_HAVE_STRING_H 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef T8_HAVE_SYS_STAT_H
#define T8_HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef T8_HAVE_SYS_TYPES_H
#define T8_HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef T8_HAVE_UNISTD_H
#define T8_HAVE_UNISTD_H 1
#endif

/* Have we found function adler32_combine. */
#ifndef T8_HAVE_ZLIB
#define T8_HAVE_ZLIB 1
#endif

/* Linker flags */
#ifndef T8_LDFLAGS
#define T8_LDFLAGS ""
#endif

/* Libraries */
#ifndef T8_LIBS
#define T8_LIBS "-lgomp -lpthread   -lz -lm    -lstdc++"
#endif

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#ifndef T8_LT_OBJDIR
#define T8_LT_OBJDIR ".libs/"
#endif

/* DEPRECATED (use T8_ENABLE_MEMALIGN instead) */
#ifndef T8_MEMALIGN
#define T8_MEMALIGN 1
#endif

/* desired alignment of allocations in bytes */
#ifndef T8_MEMALIGN_BYTES
#define T8_MEMALIGN_BYTES (T8_SIZEOF_VOID_P)
#endif

/* DEPRECATED (use T8_WITH_METIS instead) */
/* #undef METIS */

/* DEPRECATED (use T8_ENABLE_MPI instead) */
#ifndef T8_MPI
#define T8_MPI 1
#endif

/* DEPRECATED (use T8_ENABLE_MPIIO instead) */
#ifndef T8_MPIIO
#define T8_MPIIO 1
#endif

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* DEPRECATED (use T8_ENABLE_OPENMP instead) */
/* #undef OPENMP */

/* DEPRECATED (use T8_WITH_P4EST instead) */
/* #undef P4EST */

/* Name of package */
#ifndef T8_PACKAGE
#define T8_PACKAGE "t8"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef T8_PACKAGE_BUGREPORT
#define T8_PACKAGE_BUGREPORT "burstedde@ins.uni-bonn.de"
#endif

/* Define to the full name of this package. */
#ifndef T8_PACKAGE_NAME
#define T8_PACKAGE_NAME "t8"
#endif

/* Define to the full name and version of this package. */
#ifndef T8_PACKAGE_STRING
#define T8_PACKAGE_STRING "t8 0.3.311-fe06-dirty"
#endif

/* Define to the one symbol short name of this package. */
#ifndef T8_PACKAGE_TARNAME
#define T8_PACKAGE_TARNAME "t8"
#endif

/* Define to the home page for this package. */
#ifndef T8_PACKAGE_URL
#define T8_PACKAGE_URL ""
#endif

/* Define to the version of this package. */
#ifndef T8_PACKAGE_VERSION
#define T8_PACKAGE_VERSION "0.3.311-fe06-dirty"
#endif

/* DEPRECATED (use T8_WITH_PETSC instead) */
/* #undef PETSC */

/* Use builtin getopt */
/* #undef PROVIDE_GETOPT */

/* Use builtin obstack */
/* #undef PROVIDE_OBSTACK */

/* DEPRECATED (use T8_ENABLE_PTHREAD instead) */
/* #undef PTHREAD */

/* DEPRECATED (use T8_WITH_SC instead) */
/* #undef SC */

/* The size of `void *', as computed by sizeof. */
#ifndef T8_SIZEOF_VOID_P
#define T8_SIZEOF_VOID_P 8
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef T8_STDC_HEADERS
#define T8_STDC_HEADERS 1
#endif

/* Version number of package */
#ifndef T8_VERSION
#define T8_VERSION "0.3.311-fe06-dirty"
#endif

/* Package major version */
#ifndef T8_VERSION_MAJOR
#define T8_VERSION_MAJOR 0
#endif

/* Package minor version */
#ifndef T8_VERSION_MINOR
#define T8_VERSION_MINOR 3
#endif

/* Package point version */
#ifndef T8_VERSION_POINT
#define T8_VERSION_POINT 311-fe06-dirty
#endif

/* Define to 1 if BLAS is used */
/* #undef WITH_BLAS */

/* Define to 1 if LAPACK is used */
/* #undef WITH_LAPACK */

/* enable metis-dependent code */
/* #undef WITH_METIS */

/* path to installed package p4est (optional) */
/* #undef WITH_P4EST */

/* enable PETSc-dependent code */
/* #undef WITH_PETSC */

/* path to installed package sc (optional) */
/* #undef WITH_SC */
 
/* once: _SRC_T__CONFIG_H */
#endif
