#pragma once
#include "operator_base.hpp"
#include "util.hpp"
#include <cmath>
#include <limits>

namespace operators {

class StdOperator : public OperatorBase {
public:
    std::string getName() const override {
        return "std";
    }
    
    std::vector<double> calculate(const std::vector<double>& x, size_t n) const override {
        if (x.empty() || n == 0 || n > x.size()) {
            return std::vector<double>();
        }
        
        std::vector<double> result;
        result.reserve(x.size() - n + 1);
        
        // Calculate initial sums for first window
        double sum = 0.0;
        double sq_sum = 0.0;
        size_t valid_count = 0;
        
        for (size_t i = 0; i < n; ++i) {
            if (isValid(x[i])) {
                sum += x[i];
                sq_sum += x[i] * x[i];
                ++valid_count;
            }
        }
        
        // Calculate std for first window using E(X^2) - E(X)^2
        if (valid_count > 1) {  // Need at least 2 points for standard deviation
            double mean = sum / valid_count;
            double variance = (sq_sum / valid_count) - (mean * mean);
            // Ensure variance is not negative due to floating point errors
            result.push_back(variance > 0 ? std::sqrt(variance) : 0.0);
        } else {
            result.push_back(std::numeric_limits<double>::quiet_NaN());
        }
        
        // Use sliding window for subsequent calculations
        for (size_t i = 1; i <= x.size() - n; ++i) {
            // Update sums by removing old value and adding new value
            if (isValid(x[i-1])) {
                sum -= x[i-1];
                sq_sum -= x[i-1] * x[i-1];
                --valid_count;
            }
            
            if (isValid(x[i+n-1])) {
                sum += x[i+n-1];
                sq_sum += x[i+n-1] * x[i+n-1];
                ++valid_count;
            }
            
            // Calculate std using updated sums
            if (valid_count > 1) {  // Need at least 2 points for standard deviation
                double mean = sum / valid_count;
                double variance = (sq_sum / valid_count) - (mean * mean);
                // Ensure variance is not negative due to floating point errors
                result.push_back(variance > 0 ? std::sqrt(variance) : 0.0);
            } else {
                result.push_back(std::numeric_limits<double>::quiet_NaN());
            }
        }
        
        return result;
    }
};

} // namespace operators
