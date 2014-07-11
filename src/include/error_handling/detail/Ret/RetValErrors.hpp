/*
 * RetValErrors.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef RETVALERRORS_HPP_
#define RETVALERRORS_HPP_

#include <error_handling/detail/Ret/Ret.hpp>
#include <error_handling/detail/Set/Set.hpp>
#include <error_handling/detail/EnableIfNotUniversalRef.hpp>
#include <error_handling/detail/unsafe_access_to_internal_data.hpp>
#include <error_handling/detail/Any.hpp>
#include <error_handling/detail/config.hpp>

namespace error_handling {

namespace detail {

template <class Val, class Errors>
class Ret final {
	static_assert(IsSet<Errors>::value, "Second template argument must be `Set` type.");

	Any<Val, Errors> v;

	template <class OVal, class OErrors>
	friend Any<OVal, OErrors>& unsafe_access_to_internal_data(Ret<OVal, OErrors>&) noexcept;

public:
	Ret() : v(Val()) {}

	Ret(const Val& v) : v(v) {}

	Ret(Val&& v) : v(std::move(v)) {}

	template <class OErr>
	Ret(const OErr& v) : v(v) {
		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");

		printf("copy constr Err\n");
	}

	template <class OErr,
	class = typename EnableIfNotUniversalRef<OErr>::type::type>
	Ret(OErr&& v) : v(std::move(v)) {
		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");

		printf("move constr Err\n");
	}

	Ret(const Ret<Val, Errors>& v) = delete;

	template <class OVal, class OErrors>
	Ret(Ret<OVal, OErrors>&& v) /*noexcept TODO: тут нужен предикат, который проверяет на noexcept OVal и OErrors*/ :
			v(std::move(unsafe_access_to_internal_data(v))) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
		static_assert(is_more_weak, "Assign to more strong type.");

		printf("move constr Ret\n");

		unsafe_access_to_internal_data(v).clear();
	}

	Ret<Val, Errors>& operator=(const Val& v) = delete;

	Ret<Val, Errors>& operator=(Val&& v) = delete;

	template <class OErr>
	Ret<Val, Errors>& operator=(const OErr& v) = delete;

	template <class OErr,
	class = typename EnableIfNotUniversalRef<OErr>::type::type>
	Ret<Val, Errors>& operator=(OErr&& v) = delete;

	Ret<Val, Errors>& operator=(const Ret<Val, Errors>& v) = delete;

	template <class OVal, class OErrors>
	Ret<Val, Errors>& operator=(Ret<OVal, OErrors>&& v) = delete;

	/* операторов приведения типа(например к Val или ErrN) -- нет: если тип в v не совпал, то
      мы можем только бросить исключение, но эта библиотека не кидает >своих< исключений(возможно только в
      деструкторе этого класса) */

	/* набор операторов сравнения отсутствует, так как всё что они могут при
      v != Val -- кинуть исключение, а кидать исключения нельзя(в этом суть идеи) */

	~Ret() {
		if(!v.empty()) {
			ERROR_HANDLING_ERROR((Ret<Val, Errors>), "Unchecked Ret");
		}
	}
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* RETVALERRORS_HPP_ */
