/*
 * IfErr.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef IFERR_HPP_
#define IFERR_HPP_

#include <error_handling/detail/HelpersForIfErr.hpp>

// TODO: delete
#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/front.hpp>
// ^^^^

namespace error_handling {

namespace detail {

namespace hfif = error_handling::detail::helpers_for_if_err;

template <class CErrors,
class Val, class Errors,
class RetType = typename hfif::RetTypeFor_ValErrors<CErrors, Val, Errors>::type,
class UnOps>
RetType
if_err(Ret<Val, Errors>&& v, UnOps ops);

} /* namespace detail */

} /* namespace error_handling */

#endif /* IFERR_HPP_ */
