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

namespace error_handling {

namespace detail {

template <class Val, class Errors>
class Ret final {
	static_assert(IsSet<Errors>::value, "Second template argument must be `Set` type.");

	Any<Val, Errors> v;

	template <class OVal, class OErrors>
	friend Any<OVal, OErrors>& unsafe_access_to_internal_data(Ret<OVal, OErrors>&);

public:
	Ret() : v(Val()) {}

	Ret(const Val& v) : v(v) {}

	Ret(Val&& v) : v(std::move(v)) {}

	template <class OErr>
	Ret(const OErr& v) : v(v) {
		printf("copy constr Err\n");

		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");
	}

	template <class OErr,
	class = typename EnableIfNotUniversalRef<OErr>::type::type>
	Ret(OErr&& v) : v(std::move(v)) {
		printf("move constr Err\n");

		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");
	}

	Ret(const Ret<Val, Errors>& v) = delete;

	template <class OVal, class OErrors>
	Ret(Ret<OVal, OErrors>&& v) noexcept : v(std::move(unsafe_access_to_internal_data(v))) {
		printf("move constr Ret\n");

		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
		static_assert(is_more_weak, "Assign to more strong type.");
	}

	Ret<Val, Errors>& operator=(const Val& v) {
		this->v = v;
		return *this;
	}

	Ret<Val, Errors>& operator=(Val&& v) {
		this->v = std::move(v);
		return *this;
	}

	template <class OErr>
	Ret<Val, Errors>& operator=(const OErr& v) {
		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");

		this->v = v;
		return *this;
	}

	template <class OErr,
	class = typename EnableIfNotUniversalRef<OErr>::type::type>
	Ret<Val, Errors>& operator=(OErr&& v) {
		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");

		this->v = std::move(v);
		return *this;
	}

	Ret<Val, Errors>& operator=(const Ret<Val, Errors>& v) = delete;

	template <class OVal, class OErrors>
	Ret<Val, Errors>& operator=(Ret<OVal, OErrors>&& v) noexcept {
		printf("move assign Ret\n");

		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
		static_assert(is_more_weak, "Assign to more strong type.");

		this->v = std::move(unsafe_access_to_internal_data(v));
		return *this;
	}

	/* операторов приведения типа(например к Val или ErrN) -- нет: если тип в v не совпал, то
      мы можем только бросить исключение, но эта библиотека не кидает >своих< исключений(возможно только в
      деструкторе этого класса) */

	/* набор операторов сравнения отсутствует, так как всё что они могут при
      v != Val -- кинуть исключение, а кидать исключения нельзя(в этом суть идеи) */

	~Ret() {
		if(!v.empty())
			printf("Unchecked Ret: %s\n", typeid(Ret<Val, Errors>).name());
	}
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* RETVALERRORS_HPP_ */
