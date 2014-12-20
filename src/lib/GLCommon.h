/**
 * @file GLCommon.h
 *
 * Contains data structures and macros commonly used in CS123.
 */
#pragma once
#ifndef __CS123COMMON_H__
#define __CS123COMMON_H__

#define GLM_FORCE_RADIANS
#include "GL/glew.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <QMessageBox>
#include <vector>

// Relevant GLM includes
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// Loading of GLU
#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

#define COMPILE_TIME_ASSERT(pred) switch(0){case 0:case pred:;}

using std::cerr;
using std::cout;
using std::endl;
using std::max;
using std::min;

typedef double REAL;

/**
 * Returns a uniformly distributed random number on the given interval.
 * ex. urand(-1, 1)  would return a random number between -1 and 1.
 */
static inline float urand(float lower = 0.0f, float upper = 1.0f) {
    return (rand() % 1000) / 1000.0f * (upper - lower) + lower;
}

// ----------------------
// Deallocation Utilities
// ----------------------

#define safeFree(p)  \
   do {              \
      if ((p)) {     \
         free((p));  \
         (p) = NULL; \
      }              \
   } while(0)

#define safeDelete(p)   \
   do {                 \
      if ((p)) {        \
         delete ((p));  \
         (p) = NULL;    \
      }                 \
   } while(0)

#define safeDeleteArray(p) \
   do {                    \
      if ((p)) {           \
         delete[] ((p));   \
         (p) = NULL;       \
      }                    \
   } while(0)


// ---------------------
// Common math utilities
// ---------------------

#ifdef MIN
    #undef MIN
#endif

#ifdef MAX
    #undef MAX
#endif

#ifdef MIN3
    #undef MIN3
#endif

#ifdef MAX3
    #undef MAX3
#endif

#ifdef ABS
    #undef ABS
#endif

#define MIN(x, y) ({          \
   typeof (x) x_ = (x);       \
   typeof (y) y_ = (y);       \
   ((x_ < y_) ? x_ : y_);     \
})

#define MAX(x, y) ({          \
   typeof (x) _x_ = (x);      \
   typeof (y) _y_ = (y);      \
   ((_x_ > _y_) ? _x_ : _y_); \
})

#define MIN3(x, y, z) ({      \
   typeof (x) x__ = (x);      \
   typeof (y) y__ = (y);      \
   typeof (z) z__ = (z);      \
   ((x__ < y__) ? (x__ < z__ ? x__ : z__) : (y__ < z__ ? y__ : z__)); \
})

#define MAX3(x, y, z) ({      \
   typeof (x) __x = (x);      \
   typeof (y) __y = (y);      \
   typeof (z) __z = (z);      \
   ((__x > __y) ? (__x > __z ? __x: __z) : (__y > __z ? __y : __z)); \
})

#define ABS(x) ({                   \
   register typeof(x) ___tx = (x);  \
   (___tx >= 0 ? ___tx : -___tx);   \
})

#ifndef M_PI
    #define M_PI 3.14159265f
#endif

#define _EPSILON_ 1e-8
#define EPSILON _EPSILON_

#define EQ(a, b) (fabs((a) - (b)) < _EPSILON_)
#define NEQ(a, b) (fabs((a) - (b)) > _EPSILON_)


#define NYI(f) { \
       char ss[999]; \
       (sprintf(ss, "Not yet implemented: %s, file %s, line %d\n", \
                              f, __FILE__, __LINE__)); \
       QMessageBox mb; \
       mb.setText(QString(ss)); \
       mb.setIcon(QMessageBox::Critical); \
       mb.exec(); \
       exit(0xf); \
   }

#endif
