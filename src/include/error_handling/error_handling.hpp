/*
 * error_handling.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef ERROR_HANDLING_HPP_
#define ERROR_HANDLING_HPP_

#include <error_handling/detail/Ret.hpp>
#include <error_handling/detail/Ret/Ops.hpp>
#include <error_handling/detail/ifErr.hpp>
#include <error_handling/detail/repack.hpp>
#include <error_handling/detail/Set/Set.hpp>
#include <error_handling/detail/FSet/FSet.hpp>
#include <error_handling/detail/T.hpp>
#include <error_handling/detail/N.hpp>
#include <error_handling/detail/V.hpp>

namespace error_handling {

using error_handling::detail::T;
using error_handling::detail::N;
using error_handling::detail::V;

using error_handling::detail::repack;

using error_handling::detail::IsRet;

template <class Val, class... Errors>
using R = error_handling::detail::Ret<Val, error_handling::detail::Set<Errors...>>;

template <class... Errors>
using ErOp = error_handling::detail::ErOp<error_handling::detail::Set<Errors...>>;

template <class Val>
using ValOp = error_handling::detail::ValOp<Val>;

template <class... CErrors, class Val, class... Errors,
class... UnOps>
auto if_err(R<Val, Errors...>&& v, UnOps... ops)
->decltype(error_handling::detail::IfErr<error_handling::detail::Set<CErrors...>>::template call(std::move(v),
		error_handling::detail::FSet(std::forward<UnOps>(ops)...))) {
	return error_handling::detail::IfErr<error_handling::detail::Set<CErrors...>>::call(std::move(v),
			error_handling::detail::FSet(std::forward<UnOps>(ops)...));
}

template <class Val, class... Errors, class... UnOps>
auto if_errT(R<Val, Errors...>&& v, UnOps... ops)
->decltype(error_handling::detail::IfErr<T>::template call(std::move(v),
		error_handling::detail::FSet(std::forward<UnOps>(ops)...))) {
	return error_handling::detail::IfErr<T>::call(std::move(v),
			error_handling::detail::FSet(std::forward<UnOps>(ops)...));
}

} /* namespace error_handling */

#endif /* ERROR_HANDLING_HPP_ */
