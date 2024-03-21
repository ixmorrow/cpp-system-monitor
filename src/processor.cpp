#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>


#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    // Get active jiffies and total jiffies at the start
    long active_jiffies_start = LinuxParser::ActiveJiffies();
    long total_jiffies_start = LinuxParser::Jiffies();

    // Sleep for a short duration
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Get active jiffies and total jiffies after the sleep
    long active_jiffies_end = LinuxParser::ActiveJiffies();
    long total_jiffies_end = LinuxParser::Jiffies();

    // Calculate the difference in active jiffies and total jiffies
    long active_jiffies_diff = active_jiffies_end - active_jiffies_start;
    long total_jiffies_diff = total_jiffies_end - total_jiffies_start;

    // Calculate CPU utilization as a percentage
    float utilization = static_cast<float>(active_jiffies_diff) / total_jiffies_diff;

    return utilization;
}