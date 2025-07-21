#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds)
{
    int sec = seconds % 60;
    int min = seconds / 60;
    int hours = min / 60;
    min = min % 60;

    return std::to_string(hours) + ':' + std::to_string(min) + ':' + std::to_string(sec);;
}