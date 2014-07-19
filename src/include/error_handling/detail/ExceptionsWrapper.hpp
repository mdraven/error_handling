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

template <class CErrors, class F, class Args>
class ExceptionsWrapperRetType {
	template <class... T>
	struct Builder {
		using type = typename std::result_of<F(T...)>::type;
	};
	using FRet = typename SeqToVariadicType<Args, Builder>::type::type;
	using ORet = typename std::conditional<std::is_same<FRet, void>::value, V, FRet>::type;
public:
	using type = Ret<ORet, CErrors>;
};

template <class RetType>
class ExceptionsWrapperImpl {
	template <class CErrors, bool with_cerrors = !IsEmpty<CErrors>::value>
	struct WithCErrors {
		template <class F, class Args>
		static void
		call(RetType& ret, F f, const Args& args) {
			using Error = typename Front<CErrors>::type;
			using TailErrors = typename Remove<CErrors, Error>::type;

			try {
				ExceptionsWrapperImpl<RetType>::template call<TailErrors>(ret, f, args);
			}
			catch(const Error& e) {
				ret = e;
			}
		}
	};

	template <class CErrors>
	struct WithCErrors<CErrors, false> {
		template <class F, class Args, class Val, class Errors>
		static void
		call(Ret<Val, Errors>& ret, F f, const Args& args) {
			ret = invoke(f, args);
		}

		template <class F, class Args, class Errors>
		static void
		call(Ret<V, Errors>& ret, F f, const Args& args) {
			ret = V();
			invoke(f, args);
		}
	};
public:
	template <class CErrors, class F, class Args>
	static void
	call(RetType& ret, F f, const Args& args) {
		WithCErrors<CErrors>:: template call(ret, f, args);
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
