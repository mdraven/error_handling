/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef UNSAFE_ACCESS_TO_INTERNAL_DATA_HPP_
#define UNSAFE_ACCESS_TO_INTERNAL_DATA_HPP_

#include "Ret.hpp"
#include "Any.hpp"
#include <error_handling/detail/Set/Set.hpp>

namespace error_handling {

namespace detail {

template <class Val, class Errors>
Any<Val, Errors>& unsafe_access_to_internal_data(Ret<Val, Errors>& v) noexcept {
	return v.any;
}

template <class Val, class Errors>
const Any<Val, Errors>& unsafe_access_to_internal_data(const Ret<Val, Errors>& v) noexcept {
	return v.any;
}

template <class Val>
Val& unsafe_access_to_internal_data(Ret<Val, Set<>>& v) noexcept {
	return v.v;
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* UNSAFE_ACCESS_TO_INTERNAL_DATA_HPP_ */
