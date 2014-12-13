/**
 * @file CS123Common.h
 *
 * Contains data structures and macros commonly used in CS123.
 */
#pragma once
#ifndef __CS123COMMON_H__
#define __CS123COMMON_H__

#include "GL/glew.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <QMessageBox>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// glu.h in different location on macs
#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

// from http://en.wikipedia.org/wiki/Assertion_(computing)
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
static inline float urand(float lower = 0.0f, float upper = 1.0f)
{
    return (rand() % 1000) / 1000.0f * (upper - lower) + lower;
}

// A structure for a color.  Each channel is 8 bits [0-255].
struct BGRA
{
    BGRA() : b(0), g(0), r(0), a(0) {}
    BGRA(QRgb qrgb) : b(qBlue(qrgb)), g(qGreen(qrgb)), r(qRed(qrgb)), a(qAlpha(qrgb)) {}
    BGRA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255)
        : b(blue), g(green), r(red), a(alpha) {}

    // C++ TIP:
    // A union struct. Essentially, this makes b <==> channels[0],
    // g <==> channels[1], etc. In other words: b, g, r, and a are
    // stored at the same memory location as channels[4]. Note that
    // sizeof(b)+sizeof(g)+sizeof(r)+sizeof(a) = sizeof(channels)
    // so the memory overlaps **exactly**.
    //
    // You might want to take advantage of union structs in later
    // assignments, although we don't require or expect you to do so.
    //
    union {
        struct { unsigned char b, g, r, a; };
        unsigned char channels[4];
    };

    // Adds two BGRAs together, rounding values down like this:
    // BGRA(10,50,255,0) + BGRA(45,81,3,99) = BGRA(27,65,129,49)
    // Piecewise operator only affects b,g,r
    BGRA operator+(const BGRA& o) const {
        return BGRA((o.b+b),
                    (o.g+g),
                    (o.r+r),
                    a);
    }

    BGRA operator-(const BGRA& o) const {
        return BGRA((b-o.b),
                    (g-o.g),
                    (r-o.r),
                    a);
    }

    // Piecewise operator only affects b,g,r
    BGRA operator/(const float& o) const {
        return BGRA(b/o,
                    g/o,
                    r/o,
                    a);
    }

    // Piecewise operator only affects b,g,r
    BGRA operator*(const float& o) const {
        return BGRA(b*o,
                    g*o,
                    r*o,
                    a);
    }

    bool operator!=(const BGRA& other) {
      return !(*this == other);
    }

    bool operator==(const BGRA& other) {
      return (*this == other);
    }

    void bound() {
        for (int i=0; i<4; i++) {
            unsigned char comp = channels[i];
            if (comp > 255) comp = 255;
            else if (comp < 0) comp = 0;
            channels[i] = comp;
        }
    }

    BGRA toGrey() {
        float grey = r*0.299 + g*0.587 + b*0.114;
        return BGRA(grey, grey, grey, a);
    }

    float intensity() {
        int sum = r+g+b;
        if (sum == 0) return 0;
        else return sum/3.0;
    }

};

// A structure to store a rectangle on the screen.
struct CS123Rectangle {
   int minX;
   int maxX;
   int minY;
   int maxY;
};


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
