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

template <class OVal,
class UnOp,
class Val,
class Errors>
struct RepacksRetType {
	using type = Ret<OVal, typename Union<Errors, typename UnOpsErrors<OVal, Val&&, UnOp>::type>::type>;
};

class RepacksSeal final {
	template <class OVal, class UnOp, class Val, class Errors>
	friend
	typename RepacksRetType<OVal, UnOp, Val, Errors>::type
	repack(Ret<Val, Errors>&& v, UnOp op);

	template <class>
	friend class RepacksImpl;

	constexpr RepacksSeal() {}
	constexpr RepacksSeal(const RepacksSeal&) {}
	constexpr RepacksSeal(RepacksSeal&&) {}
};

template <class RetType>
class RepacksImpl {
public:
	template <class OVal, class UnOp, class Val, class Errors>
	static RetType
	call(Ret<Val, Errors>&& v, UnOp op, const RepacksSeal) {
	    if(unsafe_access_to_internal_data(v).type() == getType<Val>()) {
	    	Val ret;
	    	AssignHelper::assign(ret, std::move(v), AssignHelperSeal());

	    	return CallHandler<RetType>::template call<OVal>(op,
	    			std::move(ret), CallHandlerSeal());
	    }

		Ret<OVal, Errors> ret;
		AssignHelper::assign(ret, std::move(v), AssignHelperSeal());
		return ret;
	}
};

template <class OVal, class UnOp, class Val, class Errors>
typename RepacksRetType<OVal, UnOp, Val, Errors>::type
repack(Ret<Val, Errors>&& v, UnOp op) {
#ifdef ERROR_HANDLING_CHECK_EMPTY_RET
	if(unsafe_access_to_internal_data(v).empty()) {
		ERROR_HANDLING_CRITICAL_ERROR("Calling `repack` on an empty `Ret`.");
	}
#endif

	using RetType = typename RepacksRetType<OVal, UnOp, Val, Errors>::type;

	return RepacksImpl<RetType>::template call<OVal>(std::move(v), op, RepacksSeal());
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* REPACK_HPP_ */
