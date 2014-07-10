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
#include <error_handling/detail/HelpersForIfErr.hpp>
#include <error_handling/detail/impl/BuildRet.hpp>
#include <error_handling/detail/IfErr.hpp>
#include <error_handling/detail/impl/RetTraits.hpp>
#include <error_handling/detail/impl/UnOp.hpp>

// TODO: delete
#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/front.hpp>
// ^^^^

// TODO: delete
#include <type_traits>
// ^^^^

namespace error_handling {

namespace detail {

namespace helpers_for_if_err {

template <class CErrors, class Val, class Errors>
class RetTypeFor_ValErrors {
	using WithoutErr = typename Difference<Errors, CErrors>::type;
public:
	using type = Ret<Val, WithoutErr>;
};

template <class CErrors, class UnOps, class Val, class Errors>
class ConstraintsFor_ValErrors {
	static_assert(IsDifferenceEmpty<CErrors, Errors>::value, "`CErrors` isn't contains in `Errors`");
};

class AssignHelper {
	template <class CErrors,
	class Val, class Errors,
	class RetType, class UnOps>
	friend
	RetType
	error_handling::detail::if_err(Ret<Val, Errors>&& v, UnOps ops);

	template <class Val, class OVal, class OErrors>
	static void assign(Ret<Val, Set<>>& v, Ret<OVal, OErrors>&& ov) {printf("xxx %s\n", unsafe_access_to_internal_data(ov).type().name());
		unsafe_access_to_internal_data(v) = std::move(unsafe_any_cast<OVal>(unsafe_access_to_internal_data(ov)));
	}

	template <class Val, class Errors, class OVal, class OErrors>
	static void assign(Ret<Val, Errors>& v, Ret<OVal, OErrors>&& ov) {
		AnyAssign::auto_move(unsafe_access_to_internal_data(v), std::move(unsafe_access_to_internal_data(ov)));
	}
};

class CallHandler {
	template <class CErrors,
	class Val, class Errors,
	class RetType, class UnOps>
	friend
	RetType
	error_handling::detail::if_err(Ret<Val, Errors>&& v, UnOps ops);

	template <class Arg, class UnOp>
	struct EnableForReturnsVoid {
		using type = std::enable_if<std::is_void<typename std::result_of<UnOp(Arg)>::type>::value>;
	};

	template <class Arg, class UnOp>
	class EnableForReturnsRet {
		using ret_type = typename std::result_of<UnOp(Arg)>::type;
	public:
		static const bool value = IsRet<ret_type>::value;
		using type = std::enable_if<value>;
	};

	template <class Val, class Ret, class Err, class UnOp,
	class = typename EnableForReturnsVoid<Err&&, UnOp>::type::type>
	static Ret call(UnOp op, Err&& err) {
		op(std::move(err));
		return Val();
	}

	template <class Val, class Ret, class Err, class UnOp,
	class = typename EnableForReturnsRet<Err&&, UnOp>::type::type>
	static Ret call(UnOp op, Err&& err, void* fake = nullptr) {
		return op(std::move(err));
	}
};

} /* namespace helpers_for_if_err */

} /* namespace detail */

} /* namespace error_handling */


#endif /* HELPERSFORIFERR_IMPL_HPP_ */
