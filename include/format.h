#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long seconds);  // TODO: See src/format.cpp
std::string int_to_string(int num);
};                                    // namespace Format

#endif