/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <vector>
#pragma once

namespace Raster {
    inline std::vector<double> lerp(double i0, double d0, double i1, double d1) {

        if (i0 == i1)
            return { d0 };

        std::vector<double> vals;

        double a = (d1 - d0) / (i1 - i0);
        double d = d0;

        for (double i = i0; i <= i1; i+=1) {
            vals.emplace_back(d);
            d += a;
        }

        return vals;
    }
}