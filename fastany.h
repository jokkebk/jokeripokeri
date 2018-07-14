#pragma once

int optimal_selection_any(int *h, double *p);

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>

using namespace emscripten;

// This should be easy to interface with emscripten
double average_win(int c1, int c2, int c3, int c4, int c5, int s);
//float lerp(float a, float b, float t) {
    //return (1 - t) * a + t * b;
//}

EMSCRIPTEN_BINDINGS(my_module) {
    function("average_win", &average_win);
}
#endif
