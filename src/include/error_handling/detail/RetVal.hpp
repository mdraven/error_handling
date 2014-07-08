/*
 * RetVal.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef RETVAL_HPP_
#define RETVAL_HPP_

#include "Ret.hpp"
#include "unsafe_access_to_internal_data.hpp"

#include <type_traits>
#include <utility>

namespace error_handling {

namespace detail {

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

	Val& data() noexcept;
	const Val& data() const noexcept;

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

template <class Val>
Val& Ret<Val>::data() noexcept {
	return v;
}

template <class Val>
const Val& Ret<Val>::data() const noexcept {
	return v;
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* RETVAL_HPP_ */
