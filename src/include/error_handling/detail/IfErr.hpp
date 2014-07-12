/*
 * IfErr.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef IFERR_HPP_
#define IFERR_HPP_

#include <error_handling/detail/Set/Set.hpp>
#include <error_handling/detail/Ret/RetTraits.hpp>
#include <error_handling/detail/UnOp.hpp>
#include <error_handling/detail/config.hpp>

// TODO: delete
#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/front.hpp>
// ^^^^

namespace error_handling {

namespace detail {

template <class CErrors, class Val, class Errors>
class IfErrsStrongRetType {
	using WithoutErr = typename Difference<Errors, CErrors>::type;
public:
	using type = Ret<Val, WithoutErr>;
};

template <class CErrors,
class Val, class Errors,
class UnOps>
class IfErrsWeakRetType {
	template <class UnOp>
	class UnOpsPred {
		template <class CError>
		class CErrorsPred {
			using ret_type = typename std::result_of<UnOp(CError&&)>::type;
			static const bool is_ret_ret = IsRet<ret_type>::value;

			template <class T>
			struct Wrapper {
				using type = typename T::errors_type;
			};

			struct EmptySet {
				using type = Set<>;
			};
		public:
			using result = typename std::conditional<is_ret_ret, Wrapper<IsRet<ret_type>>, EmptySet>::type::type;
			static const bool value = !IsEmpty<result>::value;
		};
	public:
		using result = typename AccumulateToSetFromPred<typename UnOpArgSet<CErrors, UnOp>::type, CErrorsPred>::type;
		static const bool value = !IsEmpty<result>::value;
	};

	using ErrorsFromUnOpsRet = typename AccumulateToSetFromPred<UnOps, UnOpsPred>::type;
public:
	using type = Ret<Val, ErrorsFromUnOpsRet>;
};

template <class CErrors,
class Val, class Errors,
class UnOps>
class IfErrsRetType {
	using StrongType = typename IfErrsStrongRetType<CErrors, Val, Errors>::type;
	using StrongErrors = typename IsRet<StrongType>::errors_type;
	using WeakType = typename IfErrsWeakRetType<CErrors, Val, Errors, UnOps>::type;
	using WeakErrors = typename IsRet<WeakType>::errors_type;
public:
	using type = Ret<Val, typename Union<StrongErrors, WeakErrors>::type>;
};

template <class CErrors,
class Val, class Errors,
class UnOps>
typename IfErrsRetType<CErrors, Val, Errors, UnOps>::type
if_err(Ret<Val, Errors>&& v, UnOps ops);

class IfErrsSeal final {
	template <class CErrors,
	class Val, class Errors,
	class UnOps>
	friend
	typename IfErrsRetType<CErrors, Val, Errors, UnOps>::type
	if_err(Ret<Val, Errors>&& v, UnOps ops);

	template <class>
	friend class IfErrsImpl;

	constexpr IfErrsSeal() {}
	constexpr IfErrsSeal(const IfErrsSeal&) {}
	constexpr IfErrsSeal(IfErrsSeal&&) {}
};

template <class RetType>
class IfErrsImpl {
	struct AssignHelper {
		template <class Val, class OVal, class OErrors>
		static void assign(Ret<Val, Set<>>& v, Ret<OVal, OErrors>&& ov) {
			AutoClearAny<OVal, OErrors> oany(unsafe_access_to_internal_data(ov));
			Val& val = unsafe_access_to_internal_data(v);

			val = std::move(unsafe_cast<OVal>(oany.data()));
		}

		template <class Val, class Errors, class OVal, class OErrors>
		static void assign(Ret<Val, Errors>& v, Ret<OVal, OErrors>&& ov) {
			Any<Val, Errors>& any = unsafe_access_to_internal_data(v);
			AutoClearAny<OVal, OErrors> oany(unsafe_access_to_internal_data(ov));

			any = std::move(oany.data());
		}
	};

	template <class UnOps, bool with_unops = !boost::fusion::result_of::empty<UnOps>::value>
	class WithUnOps {
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

			template <class Arg, class UnOp>
			struct ConstraintsForReturnsRet {
				static_assert(std::is_convertible<typename std::result_of<UnOp(Arg&&)>::type,
						RetType>::value, "Cannot convert from `UnOp(Arg&&) to `RetType`: Maybe your error handler returns too common type?");
			};
		public:
			template <class Val, class Err, class UnOp,
			class = typename EnableForReturnsVoid<Err&&, UnOp>::type::type>
			static RetType call(UnOp op, Err&& err) {
				op(std::move(err));
				return Ret<Val, Set<>>();
			}

			template <class Val, class Err, class UnOp,
			class = typename EnableForReturnsRet<Err&&, UnOp>::type::type>
			static RetType call(UnOp op, Err&& err, void* fake = nullptr) {
				ConstraintsForReturnsRet<Err, UnOp>();
				return op(std::move(err));
			}

			template <class Val, class Err, class UnOp,
			class = typename EnableForReturnsConvertibleToVal<Err&&, UnOp, Val>::type::type>
			static RetType call(UnOp op, Err&& err, char* fake = nullptr) {
				return Val(op(std::move(err)));
			}
		};

		template <bool it_can_be_reused, class>
		struct ItCanBeReused {
			template <class CErrors,
			class Val, class Errors>
			static
			RetType
			call(Ret<Val, Errors>&& v, UnOps ops) {
				return IfErrsImpl<RetType>::call<CErrors, Val, Errors>(std::move(v), ops, IfErrsSeal());
			}
		};

		template <class Fake>
		struct ItCanBeReused<false, Fake> {
			template <class CErrors,
			class Val, class Errors>
			static
			RetType
			call(Ret<Val, Errors>&& v, UnOps ops) {
				return IfErrsImpl<RetType>::call<CErrors, Val, Errors>(std::move(v), boost::fusion::pop_front(ops), IfErrsSeal());
			}
		};
	public:
		template <class CErrors,
		class Val, class Errors>
		static
		RetType
		call(Ret<Val, Errors>&& v, UnOps ops) {
			using UnOp = typename boost::fusion::result_of::front<UnOps>::type;

			static_assert(!IsEmpty<typename UnOpArgSet<CErrors, UnOp>::type>::value,
					"No one from `CError` appropriate to `UnOp`: Check an error handler's argument type.");

			using CallArgs = typename UnOpArgSet<CErrors, UnOp>::type;
			using CallArg = typename Front<CallArgs>::type;
			using NewErrors = typename Remove<CErrors, CallArg>::type;

			if(unsafe_access_to_internal_data(v).type() == typeid(CallArg)) {
				AutoClearAny<Val, Errors> any(unsafe_access_to_internal_data(v));

				return CallHandler::template call<Val>(boost::fusion::front(ops),
						std::move(unsafe_cast<CallArg>(any.data())));
			}

			return ItCanBeReused<(Size<CallArgs>::value > 1), void>::template call<NewErrors, Val, Errors>(std::move(v), ops);
		}
	};

	template <class UnOps>
	class WithUnOps<UnOps, false> {
	public:
		template <class CErrors,
		class Val, class Errors>
		static
		RetType
		call(Ret<Val, Errors>&& v, UnOps) {
			static_assert(IsEmpty<CErrors>::value, "`CErrors` is not empty: Not enough error handlers.");

			RetType ret;
			AssignHelper::assign(ret, std::move(v));
		    return ret;
		}
	};

	template <class CErrors, class UnOps, class Val, class Errors>
	class Constraints {
		static_assert(IsDifferenceEmpty<CErrors, Errors>::value, "`CErrors` isn't contains in `Errors`: Check if_err template's argument.");
	};
public:
	template <class CErrors,
	class Val, class Errors,
	class UnOps>
	static
	RetType call(Ret<Val, Errors>&& v, UnOps ops, const IfErrsSeal) {
		Constraints<CErrors, UnOps, Val, Errors>();

		return WithUnOps<UnOps>::template call<CErrors>(std::move(v), ops);
	}
};

template <class CErrors,
class Val, class Errors,
class UnOps>
typename IfErrsRetType<CErrors, Val, Errors, UnOps>::type
if_err(Ret<Val, Errors>&& v, UnOps ops) {
#ifdef ERROR_HANDLING_CHECK_DOUBLE_IFERR
	if(unsafe_access_to_internal_data(v).empty()) {
		ERROR_HANDLING_CRITICAL_ERROR("Double `if_err` on `Ret`.");
	}
#endif

	using RetType = typename IfErrsRetType<CErrors, Val, Errors, UnOps>::type;
	return IfErrsImpl<RetType>::template call<CErrors>(std::move(v), ops, IfErrsSeal());
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* IFERR_HPP_ */
