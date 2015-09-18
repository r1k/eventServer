#pragma once

#if __cplusplus > 201100

#include <memory>

#if __cplusplus > 201103L
// C++14 --- just import the library implementation.
using std::make_unique;
#else
/// std::make_unique for single objects
// Copied from libstdc++
template<typename T, typename... Args>
//inline typename _MakeUniq<_Tp>::__single_object
inline std::unique_ptr<T> make_unique(Args&&... __args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(__args)...));
}
#endif


#endif // __cplusplus > 201100
