#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "operator_base.hpp"

namespace operators {

class OperatorRegistry {
public:
    static OperatorRegistry& getInstance() {
        static OperatorRegistry instance;
        return instance;
    }

    // Register a new operator
    void registerOperator(std::unique_ptr<OperatorBase> op) {
        if (op) {
            operators_[op->getName()] = std::move(op);
        }
    }

    // Get operator by name
    OperatorBase* getOperator(const std::string& name) {
        auto it = operators_.find(name);
        return it != operators_.end() ? it->second.get() : nullptr;
    }

private:
    OperatorRegistry() = default;
    std::unordered_map<std::string, std::unique_ptr<OperatorBase>> operators_;
};

} // namespace operators
