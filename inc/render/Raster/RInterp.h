/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <vector>
#pragma once

namespace Raster {
    inline std::vector<double> lerp(double i0, double d0, double i1, double d1) {

        std::vector<double> list;
        double working_value = d0;
        double slope = (d1 - d0) / (i1 - i0);

        if (i0 > i1)
            slope *= -1;

        for (int i = 0; i <= abs(i1 - i0); i++) {
            list.emplace_back(working_value);
            working_value += slope;
        }

        return list;
    }
}