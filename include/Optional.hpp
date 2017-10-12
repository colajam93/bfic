#pragma once

#if __has_include(<optional>)

#include <optional>

namespace vrlk {
template <typename T>
using Optional = std::optional<T>;

static auto&& NullOpt = std::nullopt;
} // namespace vrlk

#elif __has_include(<experimental/optional>)

#include <experimental/optional>

namespace vrlk {
template <typename T>
using Optional = std::experimental::optional<T>;

static auto&& NullOpt = std::experimental::nullopt;
} // namespace vrlk

#else
#error "There is no optional implementation"
#endif
