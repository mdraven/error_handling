/*
 * UnOp.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef UNOP_HPP_
#define UNOP_HPP_

#include <error_handling/detail/Set/Set.hpp>

namespace error_handling {

namespace detail {

template <class Arg, class UnOp>
class IsUnOp {
	template <class OArg, class OUnOp, class = typename std::result_of<OUnOp(OArg)>::type>
	static std::true_type helper(const OArg&);

	template <class OArg, class OUnOp>
	static std::false_type helper(...);

public:
	static const bool value = decltype(helper<Arg, UnOp>(std::declval<Arg>()))::value;
};

template <class Args, class UnOp>
class UnOpArgSet {
	template <class Arg>
	using Pred = IsUnOp<Arg, UnOp>;
public:
	using type = typename AccumulateToSet<Args, Pred>::type;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* UNOP_HPP_ */
