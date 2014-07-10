/*
 * HelpersForIfErr.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef HELPERSFORIFERR_IMPL_HPP_
#define HELPERSFORIFERR_IMPL_HPP_

#include <error_handling/detail/Any.hpp>
#include <error_handling/detail/Set.hpp>
#include <error_handling/detail/HelpersForIfErr.hpp>
#include <error_handling/detail/impl/BuildRet.hpp>
#include <error_handling/detail/IfErr.hpp>
#include <error_handling/detail/impl/RetTraits.hpp>
#include <error_handling/detail/impl/UnOp.hpp>

// TODO: delete
#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/front.hpp>
// ^^^^

// TODO: delete
#include <type_traits>
// ^^^^

namespace error_handling {

namespace detail {

namespace helpers_for_if_err {

template <class CErrors, class Val, class Errors>
class RetTypeFor_ValErrors {
	using WithoutErr = typename Difference<Errors, CErrors>::type;
public:
	using type = Ret<Val, WithoutErr>;
};

} /* namespace helpers_for_if_err */

} /* namespace detail */

} /* namespace error_handling */


#endif /* HELPERSFORIFERR_IMPL_HPP_ */
