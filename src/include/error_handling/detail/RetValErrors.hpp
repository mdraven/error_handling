/*
 * RetValErrors.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef RETVALERRORS_HPP_
#define RETVALERRORS_HPP_

#include "Ret.hpp"
#include "Set.hpp"
#include "EnablesForRetValErrors.hpp"
#include "EnableIfNotUniversalRef.hpp"
#include "unsafe_access_to_internal_data.hpp"
#include "Any.hpp"

namespace error_handling {

namespace detail {

template <class Val, class Errors>
class Ret final {
	Any<Val, Errors> v;

	template <class OVal, class OErrors>
	friend Any<OVal, OErrors>& unsafe_access_to_internal_data(Ret<OVal, OErrors>&);

public:
	Ret();

	Ret(const Val& v);
	Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value);

	template <class OErr>
	Ret(const OErr& v);

	template <class OErr,
	class = typename EnableIfNotUniversalRef<OErr>::type::type>
	Ret(OErr&& v) noexcept(std::is_nothrow_move_constructible<OErr>::value);

	Ret(const Ret<Val, Errors>& v) = delete;

	template <class OVal, class OErrors>
	Ret(Ret<OVal, OErrors>&& v) noexcept;

	Ret<Val, Errors>& operator=(const Val& v);
	Ret<Val, Errors>& operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value);

	template <class OErr>
	Ret<Val, Errors>& operator=(const OErr& v);

	template <class OErr,
	class = typename EnableIfNotUniversalRef<OErr>::type::type>
	Ret<Val, Errors>& operator=(OErr&& v) noexcept(std::is_nothrow_move_assignable<OErr>::value);

	Ret<Val, Errors>& operator=(const Ret<Val, Errors>& v) = delete;

	template <class OVal, class OErrors>
	Ret<Val, Errors>& operator=(Ret<OVal, OErrors>&& v) noexcept;

	/* операторов приведения типа(например к Val или ErrN) -- нет: если тип в v не совпал, то
      мы можем только бросить исключение, но эта библиотека не кидает >своих< исключений(возможно только в
      деструкторе этого класса) */

	/* набор операторов сравнения отсутствует, так как всё что они могут при
      v != Val -- кинуть исключение, а кидать исключения нельзя(в этом суть идеи) */

	~Ret();
};

template <class Val, class Errors>
Ret<Val, Errors>::Ret() : v(Val()) {}

template <class Val, class Errors>
Ret<Val, Errors>::Ret(const Val& v) : v(v) {}

template <class Val, class Errors>
Ret<Val, Errors>::Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value) :
	v(std::move(v)) {}

template <class Val, class Errors>
template <class OErr>
Ret<Val, Errors>::Ret(const OErr& v) : v(v) {
	printf("copy constr Err\n");

	static const bool is_known_error = IsContains<Errors, OErr>::value;
	static_assert(is_known_error, "Unknown error type");
}

template <class Val, class Errors>
template <class OErr, class>
Ret<Val, Errors>::Ret(OErr&& v) noexcept(std::is_nothrow_move_constructible<OErr>::value) :
	v(std::move(v)) {
	printf("move constr Err\n");

	static const bool is_known_error = IsContains<Errors, OErr>::value;
	static_assert(is_known_error, "Unknown error type");
}

template <class Val, class Errors>
template <class OVal, class OErrors>
Ret<Val, Errors>::Ret(Ret<OVal, OErrors>&& v) noexcept :
	v(std::move(unsafe_access_to_internal_data(v))) {
	printf("move constr Ret\n");

	static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
	static_assert(is_convertible_val, "Cannot convert `Val` type.");

	static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
	static_assert(is_more_weak, "Assign to more strong type.");
}

template <class Val, class Errors>
Ret<Val, Errors>&
Ret<Val, Errors>::operator=(const Val& v) {
	this->v = v.v;
	return *this;
}

template <class Val, class Errors>
Ret<Val, Errors>&
Ret<Val, Errors>::operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value) {
	this->v = std::move(v);
	return *this;
}

template <class Val, class Errors>
template <class OErr>
Ret<Val, Errors>&
Ret<Val, Errors>::operator=(const OErr& v) {

	static const bool is_known_error = IsContains<Errors, OErr>::value;
	static_assert(is_known_error, "Unknown error type");

	this->v = v.v;
	return *this;
}

template <class Val, class Errors>
template <class OErr, class>
Ret<Val, Errors>&
Ret<Val, Errors>::operator=(OErr&& v) noexcept(std::is_nothrow_move_assignable<OErr>::value) {

	static const bool is_known_error = IsContains<Errors, OErr>::value;
	static_assert(is_known_error, "Unknown error type");

	this->v = std::move(v);
	return *this;
}

template <class Val, class Errors>
template <class OVal, class OErrors>
Ret<Val, Errors>&
Ret<Val, Errors>::operator=(Ret<OVal, OErrors>&& v) noexcept {
	static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
	static_assert(is_convertible_val, "Cannot convert `Val` type.");

	static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
	static_assert(is_more_weak, "Assign to more strong type.");


	printf("move assign Ret\n");
	this->v = std::move(unsafe_access_to_internal_data(v));
	return *this;
}

template <class Val, class Errors>
Ret<Val, Errors>::~Ret() {
	if(!v.empty())
		printf("Unchecked Ret: %s\n", typeid(Ret<Val, Errors>).name());
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* RETVALERRORS_HPP_ */
