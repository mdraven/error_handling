/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef FSET_HPP_
#define FSET_HPP_

#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/functional/invocation.hpp>

namespace error_handling {

namespace detail {

namespace f = boost::fusion;

template <class... Args>
f::list<Args...> FSet(Args... args) {
	return f::make_list(std::forward<Args>(args)...);
}

template <class FSet>
auto popFront(FSet fset) -> decltype(f::pop_front(std::forward<FSet>(fset))) {
	return f::pop_front(std::forward<FSet>(fset));
}

template <class FSet>
auto getFront(FSet fset) -> decltype(f::front(std::forward<FSet>(fset))) {
	return f::front(std::forward<FSet>(fset));
}

template <class F, class FSet>
auto invoke(F f, FSet& fset) -> decltype(f::invoke(f, fset)) {
	return f::invoke(f, fset);
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* FSET_HPP_ */
