/*
 * repack.hpp
 *
 *  Created on: Jul 13, 2014
 *      Author: mdraven
 */

#ifndef REPACK_HPP_
#define REPACK_HPP_

#include <error_handling/detail/Set/Set.hpp>
#include <error_handling/detail/FSet/FSet.hpp>
#include <error_handling/detail/Ret/RetTraits.hpp>
#include <error_handling/detail/UnOp.hpp>
#include <error_handling/detail/AssignHelper.hpp>
#include <error_handling/detail/config.hpp>

namespace error_handling {

namespace detail {

template <class OVal, class UnOp, class Val, class Errors>
Ret<OVal, Errors> repack(Ret<Val, Errors>&& v, UnOp func) {
	AutoClearAny<Val, Errors> any(unsafe_access_to_internal_data(v));
//    if(any.data().type() == typeid(Val)) {
//    	return func(unsafe_cast<Val>(any.data()));
//    }

	Ret<OVal, Errors> ret;
	AssignHelper::assign(ret, std::move(v), AssignHelperSeal());
	return ret;
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* REPACK_HPP_ */
