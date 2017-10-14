#pragma once

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

namespace vrlk::String {

inline bool startsWidth(const std::string& s, const std::string& prefix)
{
    return s.compare(0, prefix.size(), prefix) == 0;
}

inline bool stob(const std::string& s)
{
    return !(s == "false" || s.empty());
}

inline std::string join(const std::string& delimiter,
                        const std::vector<std::string> v)
{
    if (v.empty()) {
        return "";
    }

    std::ostringstream ss;
    ss << v[0];

    for (auto it = ++(std::begin(v)); it != std::end(v); ++it) {
        ss << delimiter << *it;
    }
    return ss.str();
}

} // namespace vrlk::String
