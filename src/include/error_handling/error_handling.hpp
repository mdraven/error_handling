/*
 * error_handling.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef ERROR_HANDLING_HPP_
#define ERROR_HANDLING_HPP_

#include <error_handling/detail/Ret.hpp>
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

template <class Val, class... Errors>
using R = error_handling::detail::Ret<Val, error_handling::detail::Set<Errors...>>;

template <class... CErrors, class Val, class... Errors,
class... UnOps>
auto if_err(R<Val, Errors...>&& v, UnOps... ops)
->decltype(error_handling::detail::ifErr<error_handling::detail::Set<CErrors...>>(std::move(v),
		error_handling::detail::FSet(std::forward<UnOps>(ops)...))) {
	return error_handling::detail::ifErr<error_handling::detail::Set<CErrors...>>(std::move(v),
			error_handling::detail::FSet(std::forward<UnOps>(ops)...));
}

} /* namespace error_handling */

#endif /* ERROR_HANDLING_HPP_ */
