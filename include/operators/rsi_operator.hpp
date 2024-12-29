#pragma once
#include "operator_base.hpp"
#include "util.hpp"
#include <deque>
#include <limits>

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
        size_t valid_count = 0;
        
        // Process first n-1 price changes for initial window
        for (size_t i = 1; i < n; ++i) {
            if (isValid(x[i]) && isValid(x[i-1])) {
                double change = x[i] - x[i-1];
                if (isValid(change)) {  // Check if the difference is valid
                    if (change > 0) {
                        gain_sum += change;
                    } else {
                        loss_sum += -change;
                    }
                    ++valid_count;
                }
            }
        }
        
        // Calculate first RSI
        if (valid_count > 0) {
            double avg_gain = gain_sum / valid_count;
            double avg_loss = loss_sum / valid_count;
            double rs = avg_loss == 0 ? 100 : avg_gain / avg_loss;
            double rsi = 100 - (100 / (1 + rs));
            result.push_back(rsi);
        } else {
            result.push_back(std::numeric_limits<double>::quiet_NaN());
        }
        
        // Use sliding window for subsequent calculations
        for (size_t i = n; i < x.size(); ++i) {
            // Remove oldest change from sums if it was valid
            if (isValid(x[i-n+1]) && isValid(x[i-n])) {
                double old_change = x[i-n+1] - x[i-n];
                if (isValid(old_change)) {
                    if (old_change > 0) {
                        gain_sum -= old_change;
                    } else {
                        loss_sum -= -old_change;
                    }
                    --valid_count;
                }
            }
            
            // Add newest change to sums if valid
            if (isValid(x[i]) && isValid(x[i-1])) {
                double new_change = x[i] - x[i-1];
                if (isValid(new_change)) {
                    if (new_change > 0) {
                        gain_sum += new_change;
                    } else {
                        loss_sum += -new_change;
                    }
                    ++valid_count;
                }
            }
            
            // Calculate RSI if we have valid changes
            if (valid_count > 0) {
                double avg_gain = gain_sum / valid_count;
                double avg_loss = loss_sum / valid_count;
                double rs = avg_loss == 0 ? 100 : avg_gain / avg_loss;
                double rsi = 100 - (100 / (1 + rs));
                result.push_back(rsi);
            } else {
                result.push_back(std::numeric_limits<double>::quiet_NaN());
            }
        }
        
        return result;
    }
};

} // namespace operators
