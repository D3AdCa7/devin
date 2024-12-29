#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include "operators/operator_base.hpp"
#include "operators/operator_registry.hpp"
#include "operators/mean_operator.hpp"
#include "operators/std_operator.hpp"
#include "operators/rsi_operator.hpp"

// Helper function to generate test data
std::vector<double> generateTestData(size_t size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(100, 10); // mean=100, std=10

    std::vector<double> data;
    data.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        data.push_back(d(gen));
    }
    return data;
}

// Helper function to measure execution time
template<typename Func>
double measureTime(Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

int main() {
    // Register operators
    auto& registry = operators::OperatorRegistry::getInstance();
    registry.registerOperator(std::make_unique<operators::MeanOperator>());
    registry.registerOperator(std::make_unique<operators::StdOperator>());
    registry.registerOperator(std::make_unique<operators::RSIOperator>());

    // Test invalid value handling
    std::vector<double> invalid_data = {
        1.0, 2.0, std::numeric_limits<double>::quiet_NaN(),
        4.0, std::numeric_limits<double>::infinity(),
        6.0, -std::numeric_limits<double>::infinity(),
        8.0, 9.0, 10.0
    };
    size_t test_window = 3;
    
    std::cout << "Testing invalid value handling (window=" << test_window << "):\n";
    std::cout << "Test data: ";
    for (const auto& val : invalid_data) {
        if (std::isnan(val)) std::cout << "NaN ";
        else if (std::isinf(val)) std::cout << (val > 0 ? "+Inf " : "-Inf ");
        else std::cout << val << " ";
    }
    std::cout << "\n\n";

    // Test each operator with invalid values
    for (const auto& op_name : {"mean", "std", "rsi"}) {
        if (auto op = registry.getOperator(op_name)) {
            std::cout << op_name << " results:\n";
            auto results = op->calculate(invalid_data, test_window);
            for (size_t i = 0; i < results.size(); ++i) {
                std::cout << "Window " << i + 1 << ": ";
                if (std::isnan(results[i])) std::cout << "NaN";
                else if (std::isinf(results[i])) std::cout << (results[i] > 0 ? "+Inf" : "-Inf");
                else std::cout << std::fixed << std::setprecision(4) << results[i];
                std::cout << "\n";
            }
            std::cout << "\n";
        }
    }

    // Test with small dataset first to verify correctness
    std::vector<double> small_data = {10, 12, 14, 11, 13, 15, 12, 14, 16};
    size_t small_window = 3;
    
    std::cout << "Testing with small dataset (size=" << small_data.size() << ", window=" << small_window << "):\n";
    if (auto mean_op = registry.getOperator("mean")) {
        auto results = mean_op->calculate(small_data, small_window);
        std::cout << "Mean values: ";
        for (auto val : results) std::cout << std::fixed << std::setprecision(2) << val << " ";
        std::cout << "\n";
    }

    // Now test with large dataset
    const size_t data_size = 10000;
    const size_t window_size = 100;
    std::cout << "\nGenerating large test data (size=" << data_size << ")...\n";
    auto data = generateTestData(data_size);
    
    std::cout << "Testing operators with window size " << window_size << ":\n";
    std::cout << std::fixed << std::setprecision(3);


    // Test mean operator
    if (auto mean_op = registry.getOperator("mean")) {
        std::vector<double> mean_results;
        double mean_time = measureTime([&]() {
            mean_results = mean_op->calculate(data, window_size);
        });
        std::cout << "\nMean operator:\n";
        std::cout << "- Execution time: " << mean_time << "ms\n";
        std::cout << "- Result size: " << mean_results.size() << "\n";
        std::cout << "- First 3 values: " << mean_results[0] << ", " 
                 << mean_results[1] << ", " << mean_results[2] << "\n";
    }

    // Test std operator
    if (auto std_op = registry.getOperator("std")) {
        std::vector<double> std_results;
        double std_time = measureTime([&]() {
            std_results = std_op->calculate(data, window_size);
        });
        std::cout << "\nSTD operator:\n";
        std::cout << "- Execution time: " << std_time << "ms\n";
        std::cout << "- Result size: " << std_results.size() << "\n";
        std::cout << "- First 3 values: " << std_results[0] << ", "
                 << std_results[1] << ", " << std_results[2] << "\n";
    }

    // Test RSI operator
    if (auto rsi_op = registry.getOperator("rsi")) {
        std::vector<double> rsi_results;
        double rsi_time = measureTime([&]() {
            rsi_results = rsi_op->calculate(data, window_size);
        });
        std::cout << "\nRSI operator:\n";
        std::cout << "- Execution time: " << rsi_time << "ms\n";
        std::cout << "- Result size: " << rsi_results.size() << "\n";
        std::cout << "- First 3 values: " << rsi_results[0] << ", "
                 << rsi_results[1] << ", " << rsi_results[2] << "\n";
    }

    return 0;
}
