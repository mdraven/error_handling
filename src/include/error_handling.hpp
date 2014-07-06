/*
 * error_handling.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef ERROR_HANDLING_HPP_
#define ERROR_HANDLING_HPP_

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

namespace helper {

template <class Seq, class Elem>
struct enable_if_contains {
	using type = std::enable_if<boost::mpl::contains<Seq, Elem>::value>;
};

template <class Seq1, class Seq2>
struct difference {
	using type = typename boost::mpl::remove_if<Seq1, boost::mpl::contains<Seq2, boost::mpl::_>>::type;
};

template <class Seq1, class Seq2>
struct enable_if_difference {
	using type = typename std::enable_if<boost::mpl::empty<typename difference<Seq1, Seq2>::type>::value>::type;
};

template <class T>
struct enable_if_not_universal_ref {
	using type = typename std::enable_if<!std::is_lvalue_reference<T>::value>;
};

} /* namespace helper */

template <class Val, class... Errors>
class Ret<Val, Errors...> final {
    boost::any v;

    using errors = boost::mpl::set<Errors...>;
public:
    Ret();

    Ret(const Val& v);
    Ret(Val&& v) noexcept(std::is_nothrow_move_constructible<Val>::value);

    template <class Err, class = typename helper::enable_if_contains<errors, Err>::type::type>
    Ret(const Err& v);

    template <class Err,
    class = typename helper::enable_if_not_universal_ref<Err>::type::type,
    class = typename helper::enable_if_contains<errors, Err>::type::type>
    Ret(Err&& v) noexcept(std::is_nothrow_move_constructible<Err>::value);

    Ret(const Ret<Val, Errors...>& v) = delete;

    template <class... Args,
    class args = boost::mpl::set<Args...>,
    class = typename helper::enable_if_difference<args, errors>::type>
    Ret(Ret<Val, Args...>&& v) noexcept;

    Ret<Val, Errors...>& operator=(const Val& v);
    Ret<Val, Errors...>& operator=(Val&& v) noexcept(std::is_nothrow_move_assignable<Val>::value);

    template <class Err, class = typename helper::enable_if_contains<errors, Err>::type>
    Ret<Val, Errors...>& operator=(const Err& v);

    template <class Err, class = typename helper::enable_if_contains<errors, Err>::type>
    Ret<Val, Errors...>& operator=(Err&& v) noexcept(std::is_nothrow_move_assignable<Err>::value);

    Ret<Val, Errors...>& operator=(const Ret<Val, Errors...>& v) = delete;

    template <class... Args,
    class args = boost::mpl::set<Args...>,
    class = typename helper::enable_if_difference<args, errors>::type>
    Ret<Val, Errors...>& operator=(Ret<Val, Args...>&& v) noexcept;

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
template <class Err, class, class>
Ret<Val, Errors...>::Ret(Err&& v) noexcept(std::is_nothrow_move_constructible<Err>::value) :
	v(std::move(v)) {printf("move Err\n");}

template <class Val, class... Errors>
Ret<Val, Errors...>::~Ret() {
	if(!v.empty())
		throw "Kaboom!";
}

} /* namespace error_handling */

#endif /* ERROR_HANDLING_HPP_ */
