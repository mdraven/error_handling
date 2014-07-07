/*
 * error_handling.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef ERROR_HANDLING_HPP_
#define ERROR_HANDLING_HPP_

#include "detail/helpers.hpp"

#include <boost/any.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/remove.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/empty.hpp>

#include <utility>
#include <type_traits>

namespace error_handling {

template <class... Args>
class Ret;

template <class Val>
class Ret<Val> final {
    Val v{};
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

template <class Val, class... Errors>
class Ret<Val, Errors...> final {
    boost::any v;

    template <class Val_, class... Errors_>
    friend boost::any& unsafe_access_to_internal_data(Ret<Val_, Errors_...>&);

    using errors = boost::mpl::set<Errors...>;
public:
    Ret();

    Ret(const Val& v);
    Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value);

    template <class Err,
    class = typename helpers::Enable_Ret_ValErrors_CopyConstructorFor_Err<errors, Err>::type::type>
    Ret(const Err& v);


    template <class Err,
    class = typename helpers::Enable_Ret_ValErrors_MoveConstructorFor_Err<errors, Err>::type::type>
    Ret(Err&& v) noexcept(std::is_nothrow_move_constructible<Err>::value);

    Ret(const Ret<Val, Errors...>& v) = delete;

    template <class... Args,
    class = typename helpers::Enable_Ret_ValErrors_MoveConstructorFor_Ret_ValErrors<boost::mpl::set<Args...>, errors>::type::type>
    Ret(Ret<Val, Args...>&& v) noexcept;

    Ret<Val, Errors...>& operator=(const Val& v);
    Ret<Val, Errors...>& operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value);

//    template <class Err, class = typename helpers::enable_if_contains<errors, Err>::type>
//    Ret<Val, Errors...>& operator=(const Err& v);

//    template <class Err, class = typename helpers::enable_if_contains<errors, Err>::type>
//    Ret<Val, Errors...>& operator=(Err&& v) noexcept(std::is_nothrow_move_assignable<Err>::value);

    Ret<Val, Errors...>& operator=(const Ret<Val, Errors...>& v) = delete;

//    template <class... Args,
//    class args = boost::mpl::set<Args...>,
//    class = typename helpers::enable_if_difference<args, errors>::type>
//    Ret<Val, Errors...>& operator=(Ret<Val, Args...>&& v) noexcept;

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
Ret<Val, Errors...>::Ret(const Err& v) : v(v) {printf("copy Err\n");}

template <class Val, class... Errors>
template <class Err, class>
Ret<Val, Errors...>::Ret(Err&& v) noexcept(std::is_nothrow_move_constructible<Err>::value) :
	v(std::move(v)) {printf("move Err\n");}

template <class Val, class... Errors>
template <class... Args, class>
Ret<Val, Errors...>::Ret(Ret<Val, Args...>&& v) noexcept :
	v(std::move(unsafe_access_to_internal_data(v))) {printf("move Ret\n");}

template <class Val, class... Errors>
Ret<Val, Errors...>::~Ret() {
	if(!v.empty())
		throw "Kaboom!";
}

template <class Val, class... Errors>
boost::any& unsafe_access_to_internal_data(Ret<Val, Errors...>& v) {
	return v.v;
}

} /* namespace error_handling */

#endif /* ERROR_HANDLING_HPP_ */
