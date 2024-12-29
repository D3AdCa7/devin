#pragma once
#include "operator_base.hpp"
#include <deque>

namespace operators {

class RSIOperator : public OperatorBase {
public:
    std::string getName() const override {
        return "rsi";
    }
    
    std::vector<double> calculate(const std::vector<double>& x, size_t n) const override {
        if (x.size() < 2 || n == 0 || n > x.size()) {
            return std::vector<double>();
        }
        
        std::vector<double> result;
        result.reserve(x.size() - n);
        
        if (x.size() < n + 1) {
            return result;
        }
        
        // Calculate initial sums for first window
        double gain_sum = 0.0;
        double loss_sum = 0.0;
        
        // Process first n-1 price changes for initial window
        for (size_t i = 1; i < n; ++i) {
            double change = x[i] - x[i-1];
            gain_sum += (change > 0 ? change : 0);
            loss_sum += (change < 0 ? -change : 0);
        }
        
        // Calculate first RSI
        double avg_gain = gain_sum / n;
        double avg_loss = loss_sum / n;
        double rs = avg_loss == 0 ? 100 : avg_gain / avg_loss;
        double rsi = 100 - (100 / (1 + rs));
        result.push_back(rsi);
        
        // Use sliding window for subsequent calculations
        for (size_t i = n; i < x.size(); ++i) {
            // Remove oldest change from sums
            double old_change = x[i-n+1] - x[i-n];
            if (old_change > 0) {
                gain_sum -= old_change;
            } else {
                loss_sum -= -old_change;
            }
            
            // Add newest change to sums
            double new_change = x[i] - x[i-1];
            if (new_change > 0) {
                gain_sum += new_change;
            } else {
                loss_sum += -new_change;
            }
            
            // Calculate RSI
            avg_gain = gain_sum / n;
            avg_loss = loss_sum / n;
            rs = avg_loss == 0 ? 100 : avg_gain / avg_loss;
            rsi = 100 - (100 / (1 + rs));
            result.push_back(rsi);
        }
        
        return result;
    }
};

} // namespace operators
