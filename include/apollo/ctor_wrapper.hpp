// Part of the apollo library -- Copyright (c) Christian Neumüller 2015
// This file is subject to the terms of the BSD 2-Clause License.
// See LICENSE.txt or http://opensource.org/licenses/BSD-2-Clause

#ifndef APOLLO_CTOR_WRAPPER_HPP_INCLUDED
#define APOLLO_CTOR_WRAPPER_HPP_INCLUDED

#include <apollo/detail/smart_ptr.hpp>

#include <utility>

namespace apollo {

template <typename T, typename... Args>
T ctor_wrapper(Args... args)
{
    return T(std::forward<Args>(args)...);
}

template <
    typename T, typename... Args,
    typename _ptr = typename std::conditional<
        detail::pointer_traits<T>::is_valid, T, T*>::type>
_ptr new_wrapper(Args... args)
{
    using obj_t = typename detail::pointer_traits<T>::pointee_type;
    return static_cast<_ptr>(new obj_t(std::forward<Args>(args)...));
}

} // namespace apollo

#endif // APOLLO_CTOR_WRAPPER_HPP_INCLUDED
