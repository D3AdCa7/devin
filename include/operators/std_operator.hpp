#pragma once
#include "operator_base.hpp"
#include <cmath>

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
        for (size_t i = 0; i < n; ++i) {
            sum += x[i];
            sq_sum += x[i] * x[i];
        }
        
        // Calculate std for first window using E(X^2) - E(X)^2
        double mean = sum / n;
        double variance = (sq_sum / n) - (mean * mean);
        result.push_back(std::sqrt(variance));
        
        // Use sliding window for subsequent calculations
        for (size_t i = 1; i <= x.size() - n; ++i) {
            // Update sums by removing old value and adding new value
            sum = sum - x[i-1] + x[i+n-1];
            sq_sum = sq_sum - (x[i-1] * x[i-1]) + (x[i+n-1] * x[i+n-1]);
            
            // Calculate std using updated sums
            mean = sum / n;
            variance = (sq_sum / n) - (mean * mean);
            result.push_back(std::sqrt(variance));
        }
        
        return result;
    }
};

} // namespace operators
