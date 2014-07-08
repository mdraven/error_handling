/*
 * RetT.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef RETT_HPP_
#define RETT_HPP_

#include "Ret.hpp"
#include "T.hpp"

namespace error_handling {

namespace detail {

template <>
class Ret<T> final {
public:
	Ret() noexcept = default;

	Ret(const Ret<T>&) noexcept = default;
	Ret(Ret<T>&&) noexcept = default;

	Ret<T>& operator=(const Ret<T>&) noexcept = default;
	Ret<T>& operator=(Ret<T>&&) noexcept = default;

	template <class Val>
	Ret(const Val&) noexcept {}

	template <class Val>
	Ret(Val&&) noexcept {}

	template <class Val>
	Ret(const Ret<Val>&) noexcept {}

	template <class Val>
	Ret(Ret<Val>&&) noexcept {}

	template <class Val>
	Ret<T>& operator=(const Val&) noexcept {
		return *this;
	}

	template <class Val>
	Ret<T>& operator=(Val&&) noexcept {
		return *this;
	}

	template <class Val>
	Ret<T>& operator=(const Ret<Val>&) noexcept {
		return *this;
	}

	template <class Val>
	Ret<T>& operator=(Ret<Val>&&) noexcept {
		return *this;
	}

	~Ret() = default;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* RETT_HPP_ */
