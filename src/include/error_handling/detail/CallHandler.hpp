/*
 * CallHandler.hpp
 *
 *  Created on: Jul 13, 2014
 *      Author: mdraven
 */

#ifndef CALLHANDLER_HPP_
#define CALLHANDLER_HPP_

namespace error_handling {

namespace detail {

class CallHandlerSeal final {
	template <class>
	friend class RepacksImpl;

	template <class>
	friend class IfErrsImpl;

	constexpr CallHandlerSeal() {}
	constexpr CallHandlerSeal(const CallHandlerSeal&) {}
	constexpr CallHandlerSeal(CallHandlerSeal&&) {}
};

template <class Val, class Arg, class UnOp>
class UnOpsErrors {
	using ret_type = typename std::result_of<UnOp(Arg)>::type;
	static const bool is_ret_void = std::is_void<ret_type>::value;
	static const bool is_ret_ret = IsRet<ret_type>::value;
	static const bool is_ret_convertible_to_val = std::is_convertible<ret_type, Val>::value;
	static const bool is_ret_error = !(is_ret_void || is_ret_ret || is_ret_convertible_to_val);

	using ret = typename std::conditional<is_ret_ret, typename IsRet<ret_type>::errors_type::type, Set<>>::type;
	using error = typename std::conditional<is_ret_error, Set<ret_type>, Set<>>::type;
public:
	using type = typename Union<ret, error>::type;
};

template <class RetType>
class CallHandler {
	template <class Arg, class UnOp>
	class EnableForReturnsVoid {
		static const bool is_ret_void = std::is_void<typename std::result_of<UnOp(Arg)>::type>::value;
	public:
		using type = std::enable_if<is_ret_void>;
	};

	template <class Arg, class UnOp>
	class EnableForReturnsRet {
		using ret_type = typename std::result_of<UnOp(Arg)>::type;
		static const bool is_ret_ret = IsRet<ret_type>::value;
	public:
		using type = std::enable_if<is_ret_ret>;
	};

	template <class Arg, class UnOp, class Val>
	class EnableForReturnsConvertibleToVal {
		using ret_type = typename std::result_of<UnOp(Arg)>::type;
		static const bool is_ret_convertible_to_val = std::is_convertible<ret_type, Val>::value;
	public:
		using type = std::enable_if<is_ret_convertible_to_val>;
	};

	template <class Arg, class UnOp, class Val>
	class EnableForReturnsError {
		using ret_type = typename std::result_of<UnOp(Arg)>::type;
		using errors = typename IsRet<RetType>::errors_type::type;
		static const bool is_ret_error = IsContains<errors, ret_type>::value;
	public:
		using type = std::enable_if<is_ret_error>;
	};

	template <class Arg, class UnOp>
	struct ConstraintsForReturnsRet {
		static_assert(std::is_convertible<typename std::result_of<UnOp(Arg&&)>::type,
				RetType>::value, "Cannot convert from `UnOp(Arg&&) to `RetType`: Maybe your error handler returns too common type?");
	};

	template <int>
	struct Fake;
public:
	template <class Val, class Err, class UnOp,
	class = typename EnableForReturnsVoid<Err&&, UnOp>::type::type>
	static RetType call(UnOp op, Err&& err, const CallHandlerSeal) {
		op(std::move(err));
		return Ret<Val, Set<>>();
	}

	template <class Val, class Err, class UnOp,
	class = typename EnableForReturnsRet<Err&&, UnOp>::type::type>
	static RetType call(UnOp op, Err&& err, const CallHandlerSeal, Fake<0>* = nullptr) {
		ConstraintsForReturnsRet<Err, UnOp>();
		return op(std::move(err));
	}

	template <class Val, class Err, class UnOp,
	class = typename EnableForReturnsConvertibleToVal<Err&&, UnOp, Val>::type::type>
	static RetType call(UnOp op, Err&& err, const CallHandlerSeal, Fake<1>* = nullptr) {
		return Val(op(std::move(err)));
	}

	template <class Val, class Err, class UnOp,
	class = typename EnableForReturnsError<Err&&, UnOp, Val>::type::type>
	static RetType call(UnOp op, Err&& err, const CallHandlerSeal, Fake<2>* = nullptr) {
		return op(std::move(err));
	}
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* CALLHANDLER_HPP_ */
