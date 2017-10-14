#pragma once

#include <string>

namespace vrlk {

bool startsWidth(const std::string& s, const std::string& prefix)
{
    return s.compare(0, prefix.size(), prefix) == 0;
}

} // namespace vrlk
