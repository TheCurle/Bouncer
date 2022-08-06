/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <vector>
#pragma once

namespace Raster {
    inline std::vector<float> lerp(float i0, float d0, float i1, float d1) {

        if (i0 == i1)
            return { d0 };

        std::vector<float> vals;

        float a = (d1 - d0) / (i1 - i0);
        float d = d0;

        for (float i = i0; i <= i1; ++i) {
            vals.emplace_back(d);
            d += a;
        }

        return vals;
    }
}