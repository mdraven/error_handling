/*
 * Ops.hpp
 *
 *  Created on: Jul 15, 2014
 *      Author: mdraven
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


} /* namespace detail */

} /* namespace error_handling */

#endif /* OPS_HPP_ */
