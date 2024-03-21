#include <string>
#include <iomanip>
#include <sstream>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
std::string Format::ElapsedTime(long seconds) {
    // Calculate hours, minutes, and remaining seconds
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int remainingSeconds = seconds % 60;

    // Create a string stream to format the output
    std::ostringstream oss;

    // Format the time as HH:MM:SS
    oss << std::setfill('0') << std::setw(2) << hours << ":"; // Hours
    oss << std::setfill('0') << std::setw(2) << minutes << ":"; // Minutes
    oss << std::setfill('0') << std::setw(2) << remainingSeconds; // Seconds

    // Return the formatted string
    return oss.str();
}