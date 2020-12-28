#pragma once
#include <memory>

namespace Common
{
template <typename T>
using SharedPtr = std::shared_ptr<T>;
}