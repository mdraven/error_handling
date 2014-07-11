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

	Ret(const Val& v) : v(v) {}
	Ret(Val&& v) : v(std::move(v)) {}

	Ret(const Ret<Val, Set<>>& v) = default;
	Ret(Ret<Val, Set<>>&& v) noexcept(std::is_nothrow_move_constructible<Val>::value) :
			v(std::move(v.v)) {}

	Ret<Val, Set<>>& operator=(const Val& v) = delete;
	Ret<Val, Set<>>& operator=(Val&& v) = delete;

	Ret<Val, Set<>>& operator=(const Ret<Val, Set<>>& v) = delete;
	Ret<Val, Set<>>& operator=(Ret<Val, Set<>>&& v) = delete;

	explicit operator Val&() noexcept {
		return v;
	}

	explicit operator const Val&() const noexcept {
		return v;
	}

	Val& data() noexcept {
		return v;
	}

	const Val& data() const noexcept {
		return v;
	}

	/* тут набор операторов сравнения, если они есть у Val.
      Все сравнения только с Val и Ret<Val>, никаких Ret<...,Val> */

	~Ret() = default;
};

} /* namespace detail */

} /* namespace error_handling */


#endif /* RETVAL_HPP_ */
