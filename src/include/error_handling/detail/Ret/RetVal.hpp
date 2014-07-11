/*
 * RetVal.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef RETVAL_HPP_
#define RETVAL_HPP_

#include <error_handling/detail/Ret/Ret.hpp>
#include <error_handling/detail/unsafe_access_to_internal_data.hpp>
#include <error_handling/detail/Set/Set.hpp>

#include <type_traits>
#include <utility>

namespace error_handling {

namespace detail {

template <class Val>
class Ret<Val, Set<>> final {
	Val v{};

	template <class OVal>
	friend OVal& unsafe_access_to_internal_data(Ret<OVal, Set<>>&);
public:
	Ret() = default;

	Ret(const Val& v);
	Ret(Val&& v);

	Ret(const Ret<Val, Set<>>& v) = default;
	Ret(Ret<Val, Set<>>&& v) noexcept(std::is_nothrow_move_constructible<Val>::value) :
			v(std::move(v.v)) {}

	Ret<Val, Set<>>& operator=(const Val& v) = delete;
	Ret<Val, Set<>>& operator=(Val&& v) = delete;

	Ret<Val, Set<>>& operator=(const Ret<Val, Set<>>& v) = delete;
	Ret<Val, Set<>>& operator=(Ret<Val, Set<>>&& v) = delete;

	explicit operator Val&() noexcept;
	explicit operator const Val&() const noexcept;

	Val& data() noexcept;
	const Val& data() const noexcept;

	/* тут набор операторов сравнения, если они есть у Val.
      Все сравнения только с Val и Ret<Val>, никаких Ret<...,Val> */

	~Ret() = default;
};

template <class Val>
Ret<Val, Set<>>::Ret(const Val& v) : v(v) {}

template <class Val>
Ret<Val, Set<>>::Ret(Val&& v) :
	v(std::move(v)) {}

template <class Val>
Ret<Val, Set<>>::operator Val&() noexcept {
	return v;
}

template <class Val>
Ret<Val, Set<>>::operator const Val&() const noexcept {
	return v;
}

template <class Val>
Val& Ret<Val, Set<>>::data() noexcept {
	return v;
}

template <class Val>
const Val& Ret<Val, Set<>>::data() const noexcept {
	return v;
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* RETVAL_HPP_ */
