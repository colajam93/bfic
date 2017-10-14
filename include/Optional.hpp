#pragma once

#if __has_include(<optional>)

#include <optional>

namespace vrlk::Optional {
template <typename T>
using Optional = std::optional<T>;

static auto&& NullOpt = std::nullopt;
} // namespace vrlk::Optional

#elif __has_include(<experimental/optional>)

#include <experimental/optional>

namespace vrlk::Optional {
template <typename T>
using Optional = std::experimental::optional<T>;

static auto&& NullOpt = std::experimental::nullopt;
} // namespace vrlk::Optional

#else
#error "There is no optional implementation"
#endif
