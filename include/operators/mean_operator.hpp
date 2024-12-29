#pragma once
#include "operator_base.hpp"
#include "util.hpp"
#include <numeric>
#include <limits>

namespace operators {

class MeanOperator : public OperatorBase {
public:
    std::string getName() const override {
        return "mean";
    }
    
    std::vector<double> calculate(const std::vector<double>& x, size_t n) const override {
        if (x.empty() || n == 0 || n > x.size()) {
            return std::vector<double>();
        }
        
        std::vector<double> result;
        result.reserve(x.size() - n + 1);
        
        // Calculate initial window sum and valid count
        double window_sum = 0.0;
        size_t valid_count = 0;
        for (size_t i = 0; i < n; ++i) {
            if (isValid(x[i])) {
                window_sum += x[i];
                ++valid_count;
            }
        }
        
        // Push first window result
        result.push_back(valid_count > 0 ? window_sum / valid_count : std::numeric_limits<double>::quiet_NaN());
        
        // Use sliding window for subsequent calculations
        for (size_t i = 1; i <= x.size() - n; ++i) {
            // Subtract element leaving the window if valid
            if (isValid(x[i-1])) {
                window_sum -= x[i-1];
                --valid_count;
            }
            
            // Add element entering the window if valid
            if (isValid(x[i+n-1])) {
                window_sum += x[i+n-1];
                ++valid_count;
            }
            
            // Calculate mean or NaN if no valid values
            result.push_back(valid_count > 0 ? window_sum / valid_count : std::numeric_limits<double>::quiet_NaN());
        }
        
        return result;
    }
};

} // namespace operators
