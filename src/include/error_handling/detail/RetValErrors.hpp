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

template <class Val, class Err, class... Errors>
class Ret<Val, Err, Errors...> final {
	Any v;

	template <class OVal, class OErr, class... OErrors>
	friend Any& unsafe_access_to_internal_data(Ret<OVal, OErr, OErrors...>&);

	using errors = Set<Err, Errors...>;
public:
	Ret();

	Ret(const Val& v);
	Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value);

	template <class OErr,
	class = typename erve::EnableCopyConstructorFor_Err<errors, OErr>::type::type>
	Ret(const OErr& v);

	template <class OErr,
	class = typename erve::EnableMoveConstructorFor_Err<errors, OErr>::type::type>
	Ret(OErr&& v) noexcept(std::is_nothrow_move_constructible<OErr>::value);

	Ret(const Ret<Val, Err, Errors...>& v) = delete;

	template <class OVal, class OErr, class... OErrors,
	class = typename erve::EnableMoveConstructorFor_Ret_ValErrors<OVal, Set<OErr, OErrors...>, Val, errors>::type::type>
	Ret(Ret<OVal, OErr, OErrors...>&& v) noexcept;

	Ret<Val, Err, Errors...>& operator=(const Val& v);
	Ret<Val, Err, Errors...>& operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value);

	template <class OErr,
	class = typename erve::EnableCopyAssignFor_Err<errors, OErr>::type::type>
	Ret<Val, Err, Errors...>& operator=(const OErr& v);

	template <class OErr,
	class = typename erve::EnableMoveAssignFor_Err<errors, OErr>::type::type>
	Ret<Val, Err, Errors...>& operator=(OErr&& v) noexcept(std::is_nothrow_move_assignable<OErr>::value);

	Ret<Val, Err, Errors...>& operator=(const Ret<Val, Err, Errors...>& v) = delete;

	template <class OVal, class OErr, class... OErrors,
	class = typename erve::EnableMoveAssignFor_Ret_ValErrors<OVal, Set<OErr, OErrors...>, Val, errors>::type::type>
	Ret<Val, Err, Errors...>& operator=(Ret<OVal, OErr, OErrors...>&& v) noexcept;

	/* операторов приведения типа(например к Val или ErrN) -- нет: если тип в v не совпал, то
      мы можем только бросить исключение, но эта библиотека не кидает >своих< исключений(возможно только в
      деструкторе этого класса) */

	/* набор операторов сравнения отсутствует, так как всё что они могут при
      v != Val -- кинуть исключение, а кидать исключения нельзя(в этом суть идеи) */

	~Ret();
};

template <class Val, class Err, class... Errors>
Ret<Val, Err, Errors...>::Ret() : v(Val()) {}

template <class Val, class Err, class... Errors>
Ret<Val, Err, Errors...>::Ret(const Val& v) : v(v) {}

template <class Val, class Err, class... Errors>
Ret<Val, Err, Errors...>::Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value) :
	v(std::move(v)) {}

template <class Val, class Err, class... Errors>
template <class OErr, class>
Ret<Val, Err, Errors...>::Ret(const OErr& v) : v(v) {printf("copy constr Err\n");}

template <class Val, class Err, class... Errors>
template <class OErr, class>
Ret<Val, Err, Errors...>::Ret(OErr&& v) noexcept(std::is_nothrow_move_constructible<OErr>::value) :
	v(std::move(v)) {printf("move constr Err\n");}

template <class Val, class Err, class... Errors>
template <class OVal, class OErr, class... OErrors, class>
Ret<Val, Err, Errors...>::Ret(Ret<OVal, OErr, OErrors...>&& v) noexcept :
	v(std::move(unsafe_access_to_internal_data(v))) {printf("move constr Ret\n");}

template <class Val, class Err, class... Errors>
Ret<Val, Err, Errors...>&
Ret<Val, Err, Errors...>::operator=(const Val& v) {
	this->v = v.v;
	return *this;
}

template <class Val, class Err, class... Errors>
Ret<Val, Err, Errors...>&
Ret<Val, Err, Errors...>::operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value) {
	this->v = std::move(v);
	return *this;
}

template <class Val, class Err, class... Errors>
template <class OErr, class>
Ret<Val, Err, Errors...>&
Ret<Val, Err, Errors...>::operator=(const OErr& v) {
	this->v = v.v;
	return *this;
}

template <class Val, class Err, class... Errors>
template <class OErr, class>
Ret<Val, Err, Errors...>&
Ret<Val, Err, Errors...>::operator=(OErr&& v) noexcept(std::is_nothrow_move_assignable<OErr>::value) {
	this->v = std::move(v);
	return *this;
}

template <class Val, class Err, class... Errors>
template <class OVal, class OErr, class... OErrors, class>
Ret<Val, Err, Errors...>&
Ret<Val, Err, Errors...>::operator=(Ret<OVal, OErr, OErrors...>&& v) noexcept {
	printf("move assign Ret\n");
	this->v = std::move(unsafe_access_to_internal_data(v));
	return *this;
}

template <class Val, class Err, class... Errors>
Ret<Val, Err, Errors...>::~Ret() {
	if(!v.empty())
		printf("Unchecked Ret: %s\n", typeid(Ret<Val, Err, Errors...>).name());
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* RETVALERRORS_HPP_ */
