/*
 * IfErr.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef IFERR_IMPL_HPP_
#define IFERR_IMPL_HPP_

#include <error_handling/detail/impl/HelpersForIfErr.hpp>

namespace error_handling {

namespace detail {

namespace hfif = error_handling::detail::helpers_for_if_err;

template <class CErr, class... CErrors, class UnOp, class Val, class... Errors>
typename hfif::RetTypeFor_ValErrors<hfif::Wrapper<CErr, CErrors...>, Val, Errors...>::type
if_err(Ret<Val, Errors...>&& v, UnOp op) {
	hfif::ConstraintsFor_ValErrors<hfif::Wrapper<CErr, CErrors...>, UnOp, Val, Errors...>();

    if(unsafe_access_to_internal_data(v).type() == typeid(CErr)) {
    	// TODO: если все будут передавать туда что-то похожее на hfif::RetTypeFor_ValErrors<Err, Val, Errors...>::type, то стоит переделать CallHandler::call
    	return hfif::CallHandler::call<Val,
    			typename hfif::RetTypeFor_ValErrors<hfif::Wrapper<CErr, CErrors...>, Val, Errors...>::type>
    	(op, std::move(unsafe_any_cast<CErr>(unsafe_access_to_internal_data(v))));
    }

	typename hfif::RetTypeFor_ValErrors<hfif::Wrapper<CErr, CErrors...>, Val, Errors...>::type ret;
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
