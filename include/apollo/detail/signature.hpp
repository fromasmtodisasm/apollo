// Part of the apollo library -- Copyright (c) Christian Neumüller 2015
// This file is subject to the terms of the BSD 2-Clause License.
// See LICENSE.txt or http://opensource.org/licenses/BSD-2-Clause

#ifndef APOLLO_DETAIL_SIGNATURE_HPP_INCLUDED
#define APOLLO_DETAIL_SIGNATURE_HPP_INCLUDED

#include <apollo/detail/integer_seq.hpp>
#include <apollo/detail/meta_util.hpp>

#include <tuple>

namespace apollo { namespace detail {

// Plain function pointer:
template <typename R, typename... Args>
std::tuple<R, Args...> signature_tuple_of_impl(R(*)(Args...));

// Function object (std::function, boost::function, etc.):
template <template <class> class FObj, typename R, typename... Args>
std::tuple<R, Args...> signature_tuple_of_impl(FObj<R(Args...)> const&);

// Member function pointer:
template <class C, typename R, typename... Args>
std::tuple<R, C&, Args...> signature_tuple_of_impl(R(C::*)(Args...));

// Const member function pointer:
template <class C, typename R,  typename... Args>
std::tuple<R, C const&, Args...> signature_tuple_of_impl(
    R(C::*)(Args...) const);

// msvc_decltype_helper
template <typename F, F FVal> struct msvc_inlining_helper;

template <typename F, F FVal>
decltype(signature_tuple_of_impl(FVal)) signature_tuple_of_impl(
    msvc_inlining_helper<F, FVal>);

template <typename SignatureTuple>
using arg_idxs_seq = iseq_n_t<std::tuple_size<
    detail::remove_cvr<SignatureTuple>>::value - 1, 1>;

template <typename F>
using signature_tuple_of = decltype(signature_tuple_of_impl(std::declval<F>()));

template <std::size_t I, typename F>
struct signature_element_impl { // Woraround MSVC 12's confusion.
    using type = typename std::tuple_element<I, signature_tuple_of<F>>::type;
};

template <std::size_t I, typename F>
using signature_element = typename signature_element_impl<I, F>::type;

template <typename F>
using return_type_of = signature_element<0, F>;

} } // namespace apollo::detail


#endif // APOLLO_DETAIL_SIGNATURE_HPP_INCLUDED
