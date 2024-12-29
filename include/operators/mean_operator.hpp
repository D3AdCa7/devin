#pragma once
#include "operator_base.hpp"
#include <numeric>

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
        
        // Calculate initial window sum
        double window_sum = std::accumulate(x.begin(), x.begin() + n, 0.0);
        result.push_back(window_sum / n);
        
        // Use sliding window for subsequent calculations
        for (size_t i = 1; i <= x.size() - n; ++i) {
            // Subtract element leaving the window and add element entering the window
            window_sum = window_sum - x[i-1] + x[i+n-1];
            result.push_back(window_sum / n);
        }
        
        return result;
    }
};

} // namespace operators
