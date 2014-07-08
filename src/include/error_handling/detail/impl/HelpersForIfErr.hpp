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
#include <error_handling/detail/impl/BuildRet.hpp>
#include <error_handling/detail/IfErr.hpp>

// TODO: delete
#include <type_traits>

namespace error_handling {

namespace detail {

namespace helpers_for_if_err {

template <class Err, class Val, class... Errors>
class RetTypeFor_ValErrors {
	using WithoutErr = typename Remove<Set<Errors...>, Err>::type;
public:
	using type = typename BuildRet<Ret, Val, WithoutErr>::type;
};

template <class Err, class UnOp, class Val, class... Errors>
class ConstraintsFor_ValErrors {
	static_assert(IsContains<Set<Errors...>, Err>::value, "`Err` isn't contains in `Errors...`");
};

class AssignHelper {
	template <class Err, class UnOp, class Val, class... Errors>
	friend
	typename RetTypeFor_ValErrors<Err, Val, Errors...>::type
	error_handling::detail::if_err(Ret<Val, Errors...>&& v, UnOp op);

	template <class Val, class OVal, class... OErrors>
	static void assign(Ret<Val>& v, Ret<OVal, OErrors...>&& ov) {
		unsafe_access_to_internal_data(v) = std::move(unsafe_any_cast<Val>(unsafe_access_to_internal_data(ov)));
	}

	template <class Val, class Err, class... Errors, class OVal, class... OErrors>
	static void assign(Ret<Val, Err, Errors...>& v, Ret<OVal, OErrors...>&& ov) {
		unsafe_access_to_internal_data(v) = std::move(unsafe_access_to_internal_data(ov));
	}
};

class CallHandler {
	template <class Err, class UnOp, class Val, class... Errors>
	friend
	typename RetTypeFor_ValErrors<Err, Val, Errors...>::type
	error_handling::detail::if_err(Ret<Val, Errors...>&& v, UnOp op);

	template <class Val, class... Errors, class Err, class UnOp,
	class = typename std::enable_if<std::is_void<typename std::result_of<UnOp(Err)>::type>::value>::type> //decltype(UnOp(std::declval<Err>()))>::value
	static Ret<Val, Errors...> call(UnOp op, Err&& err) {
		op(std::move(err));
		return Val();
	}
};

} /* namespace helpers_for_if_err */

} /* namespace detail */

} /* namespace error_handling */


#endif /* HELPERSFORIFERR_IMPL_HPP_ */
