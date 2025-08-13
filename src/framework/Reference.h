//
// Created by tanhao on 2025/8/8.
//

#ifndef REFERENCE_H
#define REFERENCE_H
#include <memory>
namespace Ref
{
// 共享指针
template <typename T>
using share = std::shared_ptr<T>;

// 独占指针
template <typename T>
using unique = std::unique_ptr<T>;

// 创建共享指针
template <typename T, typename... Args>
share<T> makeshare(Args &&...args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

// 创建独占指针
template <typename T, typename... Args>
unique<T> makeunique(Args &&...args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}
}        // namespace Ref
#endif        // REFERENCE_H
