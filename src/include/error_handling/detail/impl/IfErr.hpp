/*
 * IfErr.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef IFERR_IMPL_HPP_
#define IFERR_IMPL_HPP_

#include <error_handling/detail/IfErr.hpp>
#include <error_handling/detail/impl/HelpersForIfErr.hpp>
#include <error_handling/detail/Set.hpp>

namespace error_handling {

namespace detail {

namespace hfif = error_handling::detail::helpers_for_if_err;

template <class RetType>
class IfErrImpl {
	struct AssignHelper {
		template <class Val, class OVal, class OErrors>
		static void assign(Ret<Val, Set<>>& v, Ret<OVal, OErrors>&& ov) {printf("xxx %s\n", unsafe_access_to_internal_data(ov).type().name());
			unsafe_access_to_internal_data(v) = std::move(unsafe_any_cast<OVal>(unsafe_access_to_internal_data(ov)));
		}

		template <class Val, class Errors, class OVal, class OErrors>
		static void assign(Ret<Val, Errors>& v, Ret<OVal, OErrors>&& ov) {
			AnyAssign::auto_move(unsafe_access_to_internal_data(v), std::move(unsafe_access_to_internal_data(ov)));
		}
	};

	template <bool with_unops, class>
	class WithUnOps {
		template <bool it_can_be_reused, class>
		struct ItCanBeReused {
			template <class CErrors,
			class Val, class Errors,
			class UnOps>
			static
			RetType
			call(Ret<Val, Errors>&& v, UnOps ops) {
				return IfErrImpl<RetType>::call<CErrors, Val, Errors>(std::move(v), ops);
			}
		};

		template <class Fake>
		struct ItCanBeReused<false, Fake> {
			template <class CErrors,
			class Val, class Errors,
			class UnOps>
			static
			RetType
			call(Ret<Val, Errors>&& v, UnOps ops) {
				return IfErrImpl<RetType>::call<CErrors, Val, Errors>(std::move(v), boost::fusion::pop_front(ops));
			}
		};
	public:
		template <class CErrors,
		class Val, class Errors,
		class UnOps>
		static
		RetType
		call(Ret<Val, Errors>&& v, UnOps ops) {
			using UnOp = typename boost::fusion::result_of::front<UnOps>::type;

			static_assert(!IsEmpty<typename UnOpArgSet<CErrors, UnOp>::type>::value,
					"No one from `CError` appropriate to `UnOp`");

			using CallArgs = typename UnOpArgSet<CErrors, UnOp>::type;
			using CallArg = typename Front<CallArgs>::type;
			using NewErrors = typename Remove<CErrors, CallArg>::type;

			if(unsafe_access_to_internal_data(v).type() == typeid(CallArg)) {
				return hfif::CallHandler::call<Val, RetType>(boost::fusion::front(ops),
						std::move(unsafe_any_cast<CallArg>(unsafe_access_to_internal_data(v))));
			}

			return ItCanBeReused<(Size<CallArgs>::value > 1), void>::template call<NewErrors, Val, Errors>(std::move(v), ops);
		}
	};

	template <class Fake>
	class WithUnOps<false, Fake> {
	public:
		template <class CErrors,
		class Val, class Errors>
		static
		RetType
		call(Ret<Val, Errors>&& v, ...) {
			static_assert(IsEmpty<CErrors>::value, "`CErrors` is not empty");

			RetType ret;
			AssignHelper::assign(ret, std::move(v));
		    return ret;
		}
	};
public:
	template <class CErrors,
	class Val, class Errors,
	class UnOps>
	static
	RetType call(Ret<Val, Errors>&& v, UnOps ops) {
		hfif::ConstraintsFor_ValErrors<CErrors, UnOps, Val, Errors>();

		static const bool cond = boost::fusion::result_of::empty<UnOps>::value;
		return WithUnOps<!cond, void>::template call<CErrors>(std::move(v), ops);
	}
};



template <class CErrors,
class Val, class Errors,
class RetType, class UnOps>
RetType
if_err(Ret<Val, Errors>&& v, UnOps ops) {
	return IfErrImpl<RetType>::template call<CErrors>(std::move(v), ops);
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* IFERR_IMPL_HPP_ */
