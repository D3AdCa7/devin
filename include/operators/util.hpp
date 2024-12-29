#pragma once
#include <cmath>

namespace operators {

/**
 * Check if a value is valid (not NaN or infinity)
 * @param val The value to check
 * @return true if the value is finite (not NaN or infinity), false otherwise
 */
inline bool isValid(double val) {
    return std::isfinite(val);
}

} // namespace operators
