#include "utils.h"

float d2r(float deg)
{
    return deg*M_PI/180.0f;
}

float r2d(float rad)
{
    return rad*180.0f/M_PI;
}

float clamp(float val, float min, float max)
{
    if(val > max)
        return max;
    if(val < min)
        return min;
    return val;
}
