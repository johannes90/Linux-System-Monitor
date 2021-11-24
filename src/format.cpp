#include <string>

#include "format.h"
#include <math.h>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
//string Format::ElapsedTime(long seconds[[maybe_unused]]) { return string(); }

// Convert single digit integer to (0 int)
string Format::int_to_string(int num) {
  if (num < 10)
    return "0" + std::to_string(num);
  else
    return std::to_string(num);
}

string Format::ElapsedTime(long seconds) 
{ 
    string output;
    int HH, MM, SS;

    SS = seconds % 60;

    MM = ((seconds-SS) / 60) % (60);

    HH = int(seconds/3600);

    output.append(int_to_string(HH));
    output.append(":");
    output.append(int_to_string(MM));
    output.append(":");
    output.append(int_to_string(SS));

    return output;

}