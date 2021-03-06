/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef OPS_HPP_
#define OPS_HPP_

#include <error_handling/detail/Set/Set.hpp>

namespace error_handling {

namespace detail {

template <class Errors>
struct ErOp {};

template <class Val>
struct ValOp {};

template <class Val, class Errors, class OErrors>
Ret<Val, typename Union<Errors, OErrors>::type>
operator+(const Ret<Val, Errors>, const ErOp<OErrors>);

template <class Val, class Errors, class OErrors>
Ret<Val, typename Difference<Errors, OErrors>::type>
operator-(const Ret<Val, Errors>, const ErOp<OErrors>);

template <class Val, class Errors, class NewVal>
Ret<NewVal, Errors>
operator<<(const Ret<Val, Errors>, const ValOp<NewVal>);

template <class Val, class Errors, class OErrors>
constexpr bool
operator>=(const Ret<Val, Errors>, const ErOp<OErrors>) {
	return IsDifferenceEmpty<OErrors, Errors>::value;
}

template <class Val, class Errors, class OVal>
constexpr bool
operator>=(const Ret<Val, Errors>, const ValOp<OVal>) {
	return std::is_convertible<OVal, Val>::value || std::is_same<OVal, Val>::value;
}

template <class Val, class Errors>
ErOp<Errors> getErOp(const Ret<Val, Errors>) {
	return ErOp<Errors>();
}

template <class Val, class Errors>
ValOp<Val> getValOp(const Ret<Val, Errors>) {
	return ValOp<Errors>();
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* OPS_HPP_ */
