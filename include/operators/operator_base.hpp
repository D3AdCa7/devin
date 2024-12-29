#pragma once
#include <vector>
#include <string>

namespace operators {

class OperatorBase {
public:
    virtual ~OperatorBase() = default;
    
    // Main operator interface: x is input data, n is window size
    virtual std::vector<double> calculate(const std::vector<double>& x, size_t n) const = 0;
    
    // Get operator name for registration
    virtual std::string getName() const = 0;
};

} // namespace operators
