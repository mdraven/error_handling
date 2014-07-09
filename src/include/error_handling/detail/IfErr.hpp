/*
 * IfErr.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef IFERR_HPP_
#define IFERR_HPP_

#include <error_handling/detail/HelpersForIfErr.hpp>

namespace error_handling {

namespace detail {

namespace hfif = error_handling::detail::helpers_for_if_err;

template <class CErr, class... CErrors,
class UnOp, class... UnOps,
class Val, class... Errors,
class RetType = typename hfif::RetTypeFor_ValErrors<hfif::Wrapper<CErr, CErrors...>, Val, Errors...>::type>
RetType
if_err(Ret<Val, Errors...>&& v, UnOp op, UnOps... ops);

template <class Val, class... Errors,
class RetType = typename hfif::RetTypeFor_ValErrors<hfif::Wrapper<>, Val, Errors...>::type>
RetType
if_err(Ret<Val, Errors...>&& v);

} /* namespace detail */

} /* namespace error_handling */

#endif /* IFERR_HPP_ */
