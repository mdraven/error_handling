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
#include "unsafe_access_to_internal_data.hpp"
#include "Any.hpp"

namespace error_handling {

namespace detail {

namespace erve = error_handling::detail::enables_for_ret_valerrors;

template <class Val, class Errors>
class Ret final {
	Any<Val, Errors> v;

	template <class OVal, class OErrors>
	friend Any<OVal, OErrors>& unsafe_access_to_internal_data(Ret<OVal, OErrors>&);
public:
	Ret();

	Ret(const Val& v);
	Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value);

	template <class Err,
	class = typename erve::EnableCopyConstructorFor_Err<Errors, Err>::type::type>
	Ret(const Err& v);

	template <class Err,
	class = typename erve::EnableMoveConstructorFor_Err<Errors, Err>::type::type>
	Ret(Err&& v) noexcept(std::is_nothrow_move_constructible<Err>::value);

	Ret(const Ret<Val, Errors>& v) = delete;

	template <class OVal, class OErrors,
	class = typename erve::EnableMoveConstructorFor_Ret_ValErrors<OVal, OErrors, Val, Errors>::type::type>
	Ret(Ret<OVal, OErrors>&& v) noexcept;

	Ret<Val, Errors>& operator=(const Val& v);
	Ret<Val, Errors>& operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value);

	template <class OErr,
	class = typename erve::EnableCopyAssignFor_Err<Errors, OErr>::type::type>
	Ret<Val, Errors>& operator=(const OErr& v);

	template <class OErr,
	class = typename erve::EnableMoveAssignFor_Err<Errors, OErr>::type::type>
	Ret<Val, Errors>& operator=(OErr&& v) noexcept(std::is_nothrow_move_assignable<OErr>::value);

	Ret<Val, Errors>& operator=(const Ret<Val, Errors>& v) = delete;

	template <class OVal, class OErrors,
	class = typename erve::EnableMoveAssignFor_Ret_ValErrors<OVal, OErrors, Val, Errors>::type::type>
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
template <class OErr, class>
Ret<Val, Errors>::Ret(const OErr& v) : v(v) {printf("copy constr Err\n");}

template <class Val, class Errors>
template <class OErr, class>
Ret<Val, Errors>::Ret(OErr&& v) noexcept(std::is_nothrow_move_constructible<OErr>::value) :
	v(std::move(v)) {printf("move constr Err\n");}

template <class Val, class Errors>
template <class OVal, class OErrors, class>
Ret<Val, Errors>::Ret(Ret<OVal, OErrors>&& v) noexcept :
	v(std::move(unsafe_access_to_internal_data(v))) {printf("move constr Ret\n");}

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
template <class OErr, class>
Ret<Val, Errors>&
Ret<Val, Errors>::operator=(const OErr& v) {
	this->v = v.v;
	return *this;
}

template <class Val, class Errors>
template <class OErr, class>
Ret<Val, Errors>&
Ret<Val, Errors>::operator=(OErr&& v) noexcept(std::is_nothrow_move_assignable<OErr>::value) {
	this->v = std::move(v);
	return *this;
}

template <class Val, class Errors>
template <class OVal, class OErrors, class>
Ret<Val, Errors>&
Ret<Val, Errors>::operator=(Ret<OVal, OErrors>&& v) noexcept {
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
