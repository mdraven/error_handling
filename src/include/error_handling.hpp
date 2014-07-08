/*
 * error_handling.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef ERROR_HANDLING_HPP_
#define ERROR_HANDLING_HPP_

#include "detail/helpers.hpp"
#include "detail/class_enables.hpp"
#include "detail/typelist.hpp"

#include <boost/any.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/remove.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/empty.hpp>

#include <utility>
#include <type_traits>

namespace error_handling {

namespace h = error_handling::helpers;
namespace c = error_handling::class_enables;
namespace t = error_handling::typelist;

struct V {};
struct T {};

struct N {
	N() = delete;
	N(const N&) = delete;
	N(N&&) = delete;

	N& operator=(const N&) = delete;
	N& operator=(N&&) = delete;
};

template <class... Args>
class Ret;

template <class Val>
class Ret<Val> final {
	Val v{};

	template <class OVal>
	friend OVal& unsafe_access_to_internal_data(Ret<OVal>&);

public:
	Ret() = default;

	Ret(const Val& v);
	Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value);

	Ret(const Ret<Val>& v) = default;
	Ret(Ret<Val>&& v) noexcept(std::is_nothrow_move_constructible<Val>::value) = default;

	Ret<Val>& operator=(const Val& v);
	Ret<Val>& operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value);

	Ret<Val>& operator=(const Ret<Val>& v) = default;
	Ret<Val>& operator=(Ret<Val>&& v) noexcept(std::is_nothrow_move_assignable<Val>::value) = default;

	explicit operator Val&() noexcept;
	explicit operator const Val&() const noexcept;

	/* тут набор операторов сравнения, если они есть у Val.
      Все сравнения только с Val и Ret<Val>, никаких Ret<...,Val> */

	~Ret() = default;
};

template <class Val>
Ret<Val>::Ret(const Val& v) : v(v) {}

template <class Val>
Ret<Val>::Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value) :
	v(std::move(v)) {}

template <class Val>
Ret<Val>&
Ret<Val>::operator=(const Val& v) {
	this->v = v;
	return *this;
}

template <class Val>
Ret<Val>&
Ret<Val>::operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value) {
	this->v = std::move(v);
	return *this;
}

template <class Val>
Ret<Val>::operator Val&() noexcept {
	return v;
}

template <class Val>
Ret<Val>::operator const Val&() const noexcept {
	return v;
}

template <>
class Ret<N> final {
public:
	Ret() noexcept = default;

	Ret(const Ret<N>&) noexcept = default;
	Ret(Ret<N>&&) noexcept = default;

	Ret<N>& operator=(const Ret<N>&) noexcept = default;
	Ret<N>& operator=(Ret<N>&&) noexcept = default;

	~Ret() noexcept = default;
};

template <>
class Ret<T> final {
public:
	Ret() noexcept = default;

	Ret(const Ret<T>&) noexcept = default;
	Ret(Ret<T>&&) noexcept = default;

	Ret<T>& operator=(const Ret<T>&) noexcept = default;
	Ret<T>& operator=(Ret<T>&&) noexcept = default;

	template <class Val>
	Ret(const Val& v) noexcept {}

	template <class Val>
	Ret(Val&& v) noexcept {}

	template <class Val>
	Ret(const Ret<Val>& v) noexcept {}

	template <class Val>
	Ret(Ret<Val>&& v) noexcept {}

	template <class Val>
	Ret<T>& operator=(const Val& v) noexcept {
		return *this;
	}

	template <class Val>
	Ret<T>& operator=(Val&& v) noexcept {
		return *this;
	}

	template <class Val>
	Ret<T>& operator=(const Ret<Val>& v) noexcept {
		return *this;
	}

	template <class Val>
	Ret<T>& operator=(Ret<Val>&& v) noexcept {
		return *this;
	}

	~Ret() = default;
};

template <class Val, class... Errors>
class Ret<Val, Errors...> final {
	boost::any v;

	template <class OVal, class... OErrors>
	friend boost::any& unsafe_access_to_internal_data(Ret<OVal, OErrors...>&);

	using errors = t::Typelist<Errors...>;
public:
	Ret();

	Ret(const Val& v);
	Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value);

	template <class Err,
	class = typename c::Enable_Ret_ValErrors_CopyConstructorFor_Err<errors, Err>::type::type>
	Ret(const Err& v);

	template <class Err,
	class = typename c::Enable_Ret_ValErrors_MoveConstructorFor_Err<errors, Err>::type::type>
	Ret(Err&& v) noexcept(std::is_nothrow_move_constructible<Err>::value);

	Ret(const Ret<Val, Errors...>& v) = delete;

	template <class OVal, class... OErrors,
	class = typename c::Enable_Ret_ValErrors_MoveConstructorFor_Ret_ValErrors<OVal, t::Typelist<OErrors...>, Val, errors>::type::type>
	Ret(Ret<OVal, OErrors...>&& v) noexcept;

	Ret<Val, Errors...>& operator=(const Val& v);
	Ret<Val, Errors...>& operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value);

	template <class Err,
	class = typename c::Enable_Ret_ValErrors_CopyAssignFor_Err<errors, Err>::type::type>
	Ret<Val, Errors...>& operator=(const Err& v);

	template <class Err,
	class = typename c::Enable_Ret_ValErrors_MoveAssignFor_Err<errors, Err>::type::type>
	Ret<Val, Errors...>& operator=(Err&& v) noexcept(std::is_nothrow_move_assignable<Err>::value);

	Ret<Val, Errors...>& operator=(const Ret<Val, Errors...>& v) = delete;

	template <class OVal, class... OErrors,
	class = typename c::Enable_Ret_ValErrors_MoveAssignFor_Ret_ValErrors<OVal, t::Typelist<OErrors...>, Val, errors>::type::type>
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
		throw "Kaboom!";
}

template <class Val, class... Errors>
boost::any& unsafe_access_to_internal_data(Ret<Val, Errors...>& v) {
	return v.v;
}

template <class Val>
Val& unsafe_access_to_internal_data(Ret<Val>& v) {
	return v.v;
}

template <class Err, class UnOp, class Val, class... Errors>
typename h::BuildRet<Ret, Val, typename h::EraseAll<t::Typelist<Errors...>, Err>::Result>::type
if_err(Ret<Val, Errors...>&& v, UnOp op) {
    if(unsafe_access_to_internal_data(v).type() == typeid(Err)) {
        // если func возвращает Ret<...>, то: return func(std::move(err.v));
        // если func возвращает что-то другое, то ошибка компиляции(зарезервирую bool для себя ^_^)
        /* если func возвращает void: func(std::move(err.v)); return Ret<Val>();
           (такое поведения для типов Val != Void нужно потому, что из лямбды могут кидать
           исключение или делать exit(). И если разрешить не писать типы только для Val == Void,
           то тип придётся писать и в случаях когда мы не выходим из лямбды нормально, а это неудобно) */
    }

    typename h::BuildRet<Ret, Val, typename h::EraseAll<t::Typelist<Errors...>, Err>::Result>::type ret;
    unsafe_access_to_internal_data(ret) = std::move(unsafe_access_to_internal_data(v));
    return ret;
}

} /* namespace error_handling */

#endif /* ERROR_HANDLING_HPP_ */
