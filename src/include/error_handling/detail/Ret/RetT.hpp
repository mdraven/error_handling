/*
 * RetT.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef RETT_HPP_
#define RETT_HPP_

#include <error_handling/detail/Ret/Ret.hpp>
#include <error_handling/detail/T.hpp>
#include <error_handling/detail/Set/Set.hpp>

namespace error_handling {

namespace detail {

template <>
class Ret<T, Set<>> final {
public:
	Ret() noexcept = default;

	Ret(const Ret<T, Set<>>&) noexcept = default;
	Ret(Ret<T, Set<>>&&) noexcept = default;

	template <class Val>
	Ret(const Val&) noexcept {}

	template <class Val>
	Ret(Val&&) noexcept {}

	template <class Val>
	Ret(const Ret<Val, Set<>>&) noexcept {}

	template <class Val>
	Ret(Ret<Val, Set<>>&&) noexcept {}

	Ret<T, Set<>>& operator=(const Ret<T, Set<>>&) = delete;
	Ret<T, Set<>>& operator=(Ret<T, Set<>>&&) = delete;

	template <class Val>
	Ret<T, Set<>>& operator=(const Val&) = delete;

	template <class Val>
	Ret<T, Set<>>& operator=(Val&&) = delete;

	template <class Val>
	Ret<T, Set<>>& operator=(const Ret<Val, Set<>>&) = delete;

	template <class Val>
	Ret<T, Set<>>& operator=(Ret<Val, Set<>>&&) = delete;

	explicit operator T() const noexcept {
		return T();
	}

	T data() const noexcept {
		return T();
	}

	~Ret() = default;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* RETT_HPP_ */
