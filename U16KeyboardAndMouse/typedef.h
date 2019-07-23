#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include "stdio.h"

#define TRUE 1
#define FALSE 0

typedef unsigned char bool_t;

#ifdef FEATURE_TESTCODE
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
#endif /* FEATURE_TESTCODE */

#endif /* __TYPEDEF_H__ */
