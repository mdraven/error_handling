/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef EXCEPTIONSWRAPPER_HPP_
#define EXCEPTIONSWRAPPER_HPP_

#include <error_handling/detail/V.hpp>

namespace error_handling {

namespace detail {

template <class F, class Args>
class ExceptionsWrapperFRetType {
	template <class... T>
	struct Builder {
		using type = typename std::result_of<F(T...)>::type;
	};
public:
	using type = typename SeqToVariadicType<Args, Builder>::type::type;
};

template <class CErrors, class F, class Args>
class ExceptionsWrapperRetType {
	template <class FRet,
	bool is_void = std::is_same<FRet, void>::value,
	bool is_ret = IsRet<FRet>::value,
	bool is_error = IsContains<CErrors, FRet>::value>
	struct Dispatcher {};

	template <class FRet> // is_void
	struct Dispatcher<FRet, true, false, false> {
		using OVal = V;
		using OErrors = CErrors;
	};

	template <class FRet> // is_ret
	struct Dispatcher<FRet, false, true, false> {
		using OVal = typename IsRet<FRet>::val_type::type;
		using OErrors = typename Union<typename IsRet<FRet>::errors_type::type, CErrors>::type;
	};

	template <class FRet> // is_error
	struct Dispatcher<FRet, false, false, true> {
		using OVal = V;
		using OErrors = CErrors;
	};

	template <class FRet> // is_val
	struct Dispatcher<FRet, false, false, false> {
		using OVal = FRet;
		using OErrors = CErrors;
	};

	using FRet = typename ExceptionsWrapperFRetType<F, Args>::type;
	using Disp = Dispatcher<FRet>;
public:
	using type = Ret<typename Disp::OVal, typename Disp::OErrors>;
};

template <class RetType>
class ExceptionsWrapperImpl {
	template <class CErrors, bool with_cerrors = !IsEmpty<CErrors>::value>
	class WithCErrors {
	public:
		template <class F, class Args>
		static void
		call(RetType& ret, F f, const Args& args) {
			using Error = typename Front<CErrors>::type;
			using TailErrors = typename Remove<CErrors, Error>::type;

			try {
				WithCErrors<TailErrors>::call(ret, f, args);
			}
			catch(const Error& e) {
				ret = e;
			}
		}
	};

	template <class CErrors>
	class WithCErrors<CErrors, false> {
		template <class Args, class F>
		class EnableForReturnsVoid {
			using FRet = typename ExceptionsWrapperFRetType<F, Args>::type;
			static const bool is_ret_void = std::is_same<FRet, void>::value;
		public:
			using type = std::enable_if<is_ret_void>;
		};

		template <class Args, class F>
		class EnableForReturnsRet {
			using FRet = typename ExceptionsWrapperFRetType<F, Args>::type;
			static const bool is_ret_ret = IsRet<FRet>::value;
		public:
			using type = std::enable_if<is_ret_ret>;
		};

		template <class Args, class F>
		class EnableForReturnsError {
			using FRet = typename ExceptionsWrapperFRetType<F, Args>::type;
			using errors = typename IsRet<RetType>::errors_type::type;
			static const bool is_ret_error = IsContains<errors, FRet>::value;
		public:
			using type = std::enable_if<is_ret_error>;
		};

		template <class Args, class F>
		class EnableForReturnsVal {
			using FRet = typename ExceptionsWrapperFRetType<F, Args>::type;
			static const bool is_ret_void = std::is_same<FRet, void>::value;
			static const bool is_ret_ret = IsRet<FRet>::value;
			using errors = typename IsRet<RetType>::errors_type::type;
			static const bool is_ret_error = IsContains<errors, FRet>::value;
			static const bool is_ret_val = !(is_ret_void || is_ret_ret || is_ret_error);
		public:
			using type = std::enable_if<is_ret_val>;
		};

		template <int>
		struct Fake;
	public:
		template <class F, class Args,
		class = typename EnableForReturnsVal<Args, F>::type::type>
		static void
		call(RetType& ret, F f, const Args& args, Fake<0>* = nullptr) {
			ret = invoke(f, args);
		}

		template <class F, class Args,
		class = typename EnableForReturnsVoid<Args, F>::type::type>
		static void
		call(RetType& ret, F f, const Args& args, Fake<1>* = nullptr) {
			ret = V();
			invoke(f, args);
		}

		template <class F, class Args,
		class = typename EnableForReturnsRet<Args, F>::type::type>
		static void
		call(RetType& ret, F f, const Args& args, Fake<2>* = nullptr) {
			ret = invoke(f, args);
		}

		template <class F, class Args,
		class = typename EnableForReturnsError<Args, F>::type::type>
		static void
		call(RetType& ret, F f, const Args& args, Fake<3>* = nullptr) {
			ret = invoke(f, args);
		}
	};
public:
	template <class CErrors, class F, class Args>
	static void
	call(RetType& ret, F f, const Args& args) {
		WithCErrors<CErrors>::template call(ret, f, args);
	}
};

template <class CErrors, class F, class Args>
typename ExceptionsWrapperRetType<CErrors, F, Args>::type
exceptionsWrapper(F f, const Args& args) {

	using RetType = typename ExceptionsWrapperRetType<CErrors, F, Args>::type;
	RetType ret;
	ExceptionsWrapperImpl<RetType>::template call<CErrors>(ret, f, args);
	return ret;
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* EXCEPTIONSWRAPPER_HPP_ */
