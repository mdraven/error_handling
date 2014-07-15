/*
 * ifErr.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef IFERR_HPP_
#define IFERR_HPP_

#include <error_handling/detail/Set/Set.hpp>
#include <error_handling/detail/FSet/FSet.hpp>
#include <error_handling/detail/Ret/RetTraits.hpp>
#include <error_handling/detail/UnOp.hpp>
#include <error_handling/detail/AssignHelper.hpp>
#include <error_handling/detail/CallHandler.hpp>
#include <error_handling/detail/config.hpp>

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
		struct CErrorsPred {
			using result = typename UnOpsErrors<Val, CError&&, UnOp>::type;
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
	using StrongErrors = typename IsRet<StrongType>::errors_type::type;
	using WeakType = typename IfErrsWeakRetType<CErrors, Val, Errors, UnOps>::type;
	using WeakErrors = typename IsRet<WeakType>::errors_type::type;
public:
	using type = Ret<Val, typename Union<StrongErrors, WeakErrors>::type>;
};

class IfErrsSeal final {
	template <class CErrors,
	class Val, class Errors,
	class UnOps>
	friend
	typename IfErrsRetType<CErrors, Val, Errors, UnOps>::type
	ifErr(Ret<Val, Errors>&& v, UnOps ops);

	template <class>
	friend class IfErrsImpl;

	constexpr IfErrsSeal() {}
	constexpr IfErrsSeal(const IfErrsSeal&) {}
	constexpr IfErrsSeal(IfErrsSeal&&) {}
};

template <class RetType>
class IfErrsImpl {
	template <class UnOps, bool with_unops = !IsEmpty<UnOps>::value>
	class WithUnOps {
		template <bool it_can_be_reused, class>
		struct ItCanBeReused {
			template <class CErrors,
			class Val, class Errors>
			static
			RetType
			call(Ret<Val, Errors>&& v, UnOps ops) {
				return IfErrsImpl<RetType>::callHandlers<CErrors, Val, Errors>(std::move(v), ops);
			}
		};

		template <class Fake>
		struct ItCanBeReused<false, Fake> {
			template <class CErrors,
			class Val, class Errors>
			static
			RetType
			call(Ret<Val, Errors>&& v, UnOps ops) {
				return IfErrsImpl<RetType>::callHandlers<CErrors, Val, Errors>(std::move(v), popFront(ops));
			}
		};
	public:
		template <class CErrors,
		class Val, class Errors>
		static
		RetType
		call(Ret<Val, Errors>&& v, UnOps ops) {
			using UnOp = typename Front<UnOps>::type;

			static_assert(!IsEmpty<typename UnOpArgSet<CErrors, UnOp>::type>::value,
					"No one from `CError` appropriate to `UnOp`: Check an error handler's argument type.");

			using CallArgs = typename UnOpArgSet<CErrors, UnOp>::type;
			using CallArg = typename Front<CallArgs>::type;
			using NewErrors = typename Remove<CErrors, CallArg>::type;

			if(unsafe_access_to_internal_data(v).type() == typeid(CallArg)) {
				CallArg ret;
		    	AssignHelper::assign(ret, std::move(v), AssignHelperSeal());

		    	return CallHandler<RetType>::template call<Val>(getFront(ops),
		    			std::move(ret), CallHandlerSeal());
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
			AssignHelper::assign(ret, std::move(v), AssignHelperSeal());
		    return ret;
		}
	};

	template <class CErrors, class UnOps, class Val, class Errors>
	class Constraints {
		static_assert(IsDifferenceEmpty<CErrors, Errors>::value, "`CErrors` isn't contains in `Errors`: Check if_err template's argument.");
	};

	template <class CErrors,
	class Val, class Errors,
	class UnOps>
	static
	RetType callHandlers(Ret<Val, Errors>&& v, UnOps ops) {
		Constraints<CErrors, UnOps, Val, Errors>();

		return WithUnOps<UnOps>::template call<CErrors>(std::move(v), ops);
	}
public:
	template <class CErrors,
	class Val, class Errors,
	class UnOps>
	static
	RetType call(Ret<Val, Errors>&& v, UnOps ops, const IfErrsSeal) {
		if(unsafe_access_to_internal_data(v).type() == typeid(Val)) {
			RetType ret;
			AssignHelper::assign(ret, std::move(v), AssignHelperSeal());
		    return ret;
		}

		return IfErrsImpl<RetType>::callHandlers<CErrors, Val, Errors>(std::move(v), ops);
	}
};

template <class CErrors,
class Val, class Errors,
class UnOps>
typename IfErrsRetType<CErrors, Val, Errors, UnOps>::type
ifErr(Ret<Val, Errors>&& v, UnOps ops) {
#ifdef ERROR_HANDLING_CHECK_EMPTY_RET
	if(unsafe_access_to_internal_data(v).empty()) {
		ERROR_HANDLING_CRITICAL_ERROR("Calling `if_err` on an empty `Ret`.");
	}
#endif

	using RetType = typename IfErrsRetType<CErrors, Val, Errors, UnOps>::type;
	return IfErrsImpl<RetType>::template call<CErrors>(std::move(v), ops, IfErrsSeal());
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* IFERR_HPP_ */
