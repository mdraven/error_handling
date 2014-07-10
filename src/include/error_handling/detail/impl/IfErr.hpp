/*
 * IfErr.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef IFERR_IMPL_HPP_
#define IFERR_IMPL_HPP_

#include <error_handling/detail/IfErr.hpp>
#include <error_handling/detail/impl/HelpersForIfErr.hpp>
#include <error_handling/detail/Set.hpp>

namespace error_handling {

namespace detail {

namespace hfif = error_handling::detail::helpers_for_if_err;

template <class CErrors,
class Val, class Errors,
class RetType, class UnOps>
RetType
if_err_with_unops(Ret<Val, Errors>&& v, UnOps ops) {

}

template <class CErrors,
class Val, class Errors,
class RetType, class UnOps>
RetType
if_err(Ret<Val, Errors>&& v, UnOps ops) {
	hfif::ConstraintsFor_ValErrors<CErrors, UnOps, Val, Errors>();

	using UnOp = typename boost::fusion::result_of::front<UnOps>::type;

	static const bool cond = boost::fusion::result_of::empty<UnOps>::value;
	if(cond) {
		static_assert(!cond || IsEmpty<CErrors>::value, "`CErrors` is not empty");

		RetType ret;
		hfif::AssignHelper::assign(ret, std::move(v));
	    return ret;
	}
	else {
		static_assert(cond || !IsEmpty<typename UnOpArgSet<CErrors, UnOp>::type>::value,
				"No one from `CError` appropriate to `UnOp`");

		using CallArgs = typename UnOpArgSet<CErrors, UnOp>::type;
		using CallArg = typename Front<CallArgs>::type;
		using NewErrors = typename Remove<CErrors, CallArg>::type;

		if(unsafe_access_to_internal_data(v).type() == typeid(CallArg)) {
			return hfif::CallHandler::call<Val, RetType>(boost::fusion::front(ops),
					std::move(unsafe_any_cast<CallArg>(unsafe_access_to_internal_data(v))));
		}
		else {
//			if(Size<CallArgs>::value > 1)
//				return if_err<NewErrors, Val, Errors, RetType>(std::move(v), std::move(ops));
//			else
				return if_err<NewErrors, Val, Errors, RetType>(std::move(v), boost::fusion::pop_front(ops));
		}
	}
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* IFERR_IMPL_HPP_ */
