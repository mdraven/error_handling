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

namespace error_handling {

namespace detail {

namespace hfif = error_handling::detail::helpers_for_if_err;

//template <class... UnOps,
//class Val, class... Errors,
//class RetType, class... Args>
//struct Builder {
//	using type = if_err<Args..., UnOps..., Val, Errors..., RetType>;
//};


template <class CErr, class... CErrors,
class UnOp, class... UnOps,
class Val, class... Errors,
class RetType>
RetType
if_err(Ret<Val, Errors...>&& v, UnOp op, UnOps... ops) {
	hfif::ConstraintsFor_ValErrors<hfif::Wrapper<CErr, CErrors...>, hfif::Wrapper<UnOp, UnOps...>, Val, Errors...>();

	using CallArgs = typename UnOpArgSet<Set<CErr, CErrors...>, UnOp>::type;
	using CallArg = typename Front<CallArgs>::type;
	using NewErrors = typename Remove<Set<CErr, CErrors...>, CallArgs>::type;

    if(unsafe_access_to_internal_data(v).type() == typeid(CallArg)) {
    	return hfif::CallHandler::call<Val, RetType>
    					(op, std::move(unsafe_any_cast<CallArg>(unsafe_access_to_internal_data(v))));
    }
    else {

    	if(Size<CallArgs>::value > 1)
    		return if_err<CErrors..., UnOp, UnOps..., Val, Errors..., RetType>(std::move(v), op, ops...);
    	else
    		return if_err<CErrors..., UnOps..., Val, Errors..., RetType>(std::move(v), ops...);
//    	return SeqToVariadicCall<NewErrors, Builder>::call(std::move(v), ops...);
    }
}

template <class Val, class... Errors,
class RetType>
RetType
if_err(Ret<Val, Errors...>&& v) {
	RetType ret;
	hfif::AssignHelper::assign(ret, std::move(v));
    return ret;
}

template <class Val>
Ret<Val> if_err(Ret<Val>&& val) {
    return val;
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* IFERR_IMPL_HPP_ */
