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

#include <boost/any.hpp>

namespace error_handling {

namespace detail {

template <class Val, class... Errors>
class Ret<Val, Errors...> final {
	boost::any v;

	template <class OVal, class... OErrors>
	friend boost::any& unsafe_access_to_internal_data(Ret<OVal, OErrors...>&);

	using errors = Set<Errors...>;
public:
	Ret();

	Ret(const Val& v);
	Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value);

	template <class Err,
	class = typename Enable_Ret_ValErrors_CopyConstructorFor_Err<errors, Err>::type::type>
	Ret(const Err& v);

	template <class Err,
	class = typename Enable_Ret_ValErrors_MoveConstructorFor_Err<errors, Err>::type::type>
	Ret(Err&& v) noexcept(std::is_nothrow_move_constructible<Err>::value);

	Ret(const Ret<Val, Errors...>& v) = delete;

	template <class OVal, class... OErrors,
	class = typename Enable_Ret_ValErrors_MoveConstructorFor_Ret_ValErrors<OVal, Set<OErrors...>, Val, errors>::type::type>
	Ret(Ret<OVal, OErrors...>&& v) noexcept;

	Ret<Val, Errors...>& operator=(const Val& v);
	Ret<Val, Errors...>& operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value);

	template <class Err,
	class = typename Enable_Ret_ValErrors_CopyAssignFor_Err<errors, Err>::type::type>
	Ret<Val, Errors...>& operator=(const Err& v);

	template <class Err,
	class = typename Enable_Ret_ValErrors_MoveAssignFor_Err<errors, Err>::type::type>
	Ret<Val, Errors...>& operator=(Err&& v) noexcept(std::is_nothrow_move_assignable<Err>::value);

	Ret<Val, Errors...>& operator=(const Ret<Val, Errors...>& v) = delete;

	template <class OVal, class... OErrors,
	class = typename Enable_Ret_ValErrors_MoveAssignFor_Ret_ValErrors<OVal, Set<OErrors...>, Val, errors>::type::type>
	Ret<Val, Errors...>& operator=(Ret<OVal, OErrors...>&& v) noexcept;

	/* операторов приведения типа(например к Val или ErrN) -- нет: если тип в v не совпал, то
      мы можем только бросить исключение, но эта библиотека не кидает >своих< исключений(возможно только в
      деструкторе этого класса) */

	/* набор операторов сравнения отсутствует, так как всё что они могут при
      v != Val -- кинуть исключение, а кидать исключения нельзя(в этом суть идеи) */

	~Ret();
};

template <class Val, class... Errors>
Ret<Val, Errors...>::Ret() : v(Val()) {}

template <class Val, class... Errors>
Ret<Val, Errors...>::Ret(const Val& v) : v(v) {}

template <class Val, class... Errors>
Ret<Val, Errors...>::Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value) :
	v(std::move(v)) {}

template <class Val, class... Errors>
template <class Err, class>
Ret<Val, Errors...>::Ret(const Err& v) : v(v) {printf("copy constr Err\n");}

template <class Val, class... Errors>
template <class Err, class>
Ret<Val, Errors...>::Ret(Err&& v) noexcept(std::is_nothrow_move_constructible<Err>::value) :
	v(std::move(v)) {printf("move constr Err\n");}

template <class Val, class... Errors>
template <class OVal, class... OErrors, class>
Ret<Val, Errors...>::Ret(Ret<OVal, OErrors...>&& v) noexcept :
	v(std::move(unsafe_access_to_internal_data(v))) {printf("move constr Ret\n");}

template <class Val, class... Errors>
Ret<Val, Errors...>&
Ret<Val, Errors...>::operator=(const Val& v) {
	this->v = v.v;
	return *this;
}

template <class Val, class... Errors>
Ret<Val, Errors...>&
Ret<Val, Errors...>::operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value) {
	this->v = std::move(v);
	return *this;
}

template <class Val, class... Errors>
template <class Err, class>
Ret<Val, Errors...>&
Ret<Val, Errors...>::operator=(const Err& v) {
	this->v = v.v;
	return *this;
}

template <class Val, class... Errors>
template <class Err, class>
Ret<Val, Errors...>&
Ret<Val, Errors...>::operator=(Err&& v) noexcept(std::is_nothrow_move_assignable<Err>::value) {
	this->v = std::move(v);
	return *this;
}

template <class Val, class... Errors>
template <class OVal, class... OErrors, class>
Ret<Val, Errors...>&
Ret<Val, Errors...>::operator=(Ret<OVal, OErrors...>&& v) noexcept {
	printf("move assign Ret\n");
	this->v = std::move(unsafe_access_to_internal_data(v));
	return *this;
}

template <class Val, class... Errors>
Ret<Val, Errors...>::~Ret() {
	if(!v.empty())
		printf("Boom\n");
}

template <class Val, class... Errors>
boost::any& unsafe_access_to_internal_data(Ret<Val, Errors...>& v) {
	return v.v;
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* RETVALERRORS_HPP_ */
