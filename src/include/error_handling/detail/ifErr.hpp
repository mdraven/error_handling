/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
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

template <class Errors,
class UnOps>
class IfErrsHandledErrors {
	template <class UnOp>
	struct UnOpsPred {
		using result = typename UnOpArgSet<Errors, UnOp>::type;
		static const bool value = true;
	};

	using HandledErrors = typename AccumulateToSetFromPred<UnOps, UnOpsPred>::type;
public:
	using type = HandledErrors;
};

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
	template <class>
	friend class IfErr;

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
			void
			call(RetType& ret, Ret<Val, Errors>&& v, UnOps ops) {
				return IfErrsImpl<RetType>::callHandlers<CErrors, Val, Errors>(ret, std::move(v), ops);
			}
		};

		template <class Fake>
		struct ItCanBeReused<false, Fake> {
			template <class CErrors,
			class Val, class Errors>
			static
			void
			call(RetType& ret, Ret<Val, Errors>&& v, UnOps ops) {
				return IfErrsImpl<RetType>::callHandlers<CErrors, Val, Errors>(ret, std::move(v), popFront(ops));
			}
		};
	public:
		template <class CErrors,
		class Val, class Errors>
		static
		void
		call(RetType& ret, Ret<Val, Errors>&& v, UnOps ops) {
			using UnOp = typename Front<UnOps>::type;

			static_assert(!IsEmpty<typename UnOpArgSet<CErrors, UnOp>::type>::value,
					"No one from `CError` appropriate to `UnOp`: Check an error handler's argument type.");

			using CallArgs = typename UnOpArgSet<CErrors, UnOp>::type;
			using CallArg = typename Front<CallArgs>::type;
			using NewErrors = typename Remove<CErrors, CallArg>::type;

			if(unsafe_access_to_internal_data(v).template isType<CallArg>()) {
				CallArg arg = AssignHelper::assign<CallArg>(std::move(v), AssignHelperSeal());

		    	CallHandler<RetType>::template call<Val>(ret, getFront(ops),
		    			std::move(arg), CallHandlerSeal());
			}
			else
				ItCanBeReused<(Size<CallArgs>::value > 1), void>::template call<NewErrors, Val, Errors>(ret, std::move(v), ops);
		}
	};

	template <class UnOps>
	class WithUnOps<UnOps, false> {
	public:
		template <class CErrors,
		class Val, class Errors>
		static
		void
		call(RetType& ret, Ret<Val, Errors>&& v, UnOps) {
			static_assert(IsEmpty<CErrors>::value, "`CErrors` is not empty: Not enough error handlers.");

			AssignHelper::assign(ret, std::move(v), AssignHelperSeal());
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
	void callHandlers(RetType& ret, Ret<Val, Errors>&& v, UnOps ops) {
		Constraints<CErrors, UnOps, Val, Errors>();

		WithUnOps<UnOps>::template call<CErrors>(ret, std::move(v), ops);
	}
public:
	template <class CErrors,
	class Val, class Errors,
	class UnOps>
	static
	void call(RetType& ret, Ret<Val, Errors>&& v, UnOps ops, const IfErrsSeal) {
		if(unsafe_access_to_internal_data(v).template isType<Val>()) {
			AssignHelper::assign(ret, std::move(v), AssignHelperSeal());
		}
		else
			IfErrsImpl<RetType>::callHandlers<CErrors, Val, Errors>(ret, std::move(v), ops);
	}
};

template <class CErrors>
class IfErr {
	static const bool is_cerrors_t = std::is_same<CErrors, T>::value;

	template <class T>
	using EnableIfCErrorsIsT = std::enable_if<is_cerrors_t, T>;

	template <class T>
	using EnableIfCErrorsIsNotT = std::enable_if<!is_cerrors_t, T>;
public:
	template <class Val, class Errors,
	class UnOps>
	static
	typename EnableIfCErrorsIsNotT<IfErrsRetType<CErrors, Val, Errors, UnOps>>::type::type
	call(Ret<Val, Errors>&& v, UnOps ops) {
	#ifdef ERROR_HANDLING_CHECK_EMPTY_RET
		if(unsafe_access_to_internal_data(v).empty()) {
			ERROR_HANDLING_CRITICAL_ERROR("Calling `if_err` on an empty `Ret`.");
		}
	#endif

		using RetType = typename IfErrsRetType<CErrors, Val, Errors, UnOps>::type;
		RetType ret;
		IfErrsImpl<RetType>::template call<CErrors>(ret, std::move(v), ops, IfErrsSeal());
		return ret;
	}

	template <class Val, class Errors,
	class UnOps>
	static
	typename EnableIfCErrorsIsT<IfErrsRetType<typename IfErrsHandledErrors<Errors, UnOps>::type, Val, Errors, UnOps>>::type::type
	call(Ret<Val, Errors>&& v, UnOps ops) {
		using NewCErrors = typename IfErrsHandledErrors<Errors, UnOps>::type;
		return IfErr<NewCErrors>::call(std::move(v), ops);
	}
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* IFERR_HPP_ */
