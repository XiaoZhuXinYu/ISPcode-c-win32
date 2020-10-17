#ifndef CJ_INCLUDES_H
#define CJ_INCLUDES_H

#include	<stdio.h>
#include    <stdlib.h>
#include 	<string.h>
#include    <math.h>

#ifdef ISPGLOBAL
#define EXTERN

#else
#define EXTERN extern
#endif

typedef unsigned char           cj_u8;
typedef unsigned short          cj_u16;
typedef unsigned int            cj_u32;
typedef unsigned long long 		cj_u64;

typedef char                    cj_s8;
typedef short                   cj_s16;
typedef int                     cj_s32;
typedef signed long long 		cj_s64;

typedef float                   cj_float;
typedef double                  cj_double;

typedef void                    cj_void;


#define CJ_NULL                 0L
#define CJ_SUCCESS              0
#define CJ_FAILURE              (-1)


#endif // CJ_INCLUDES_H
