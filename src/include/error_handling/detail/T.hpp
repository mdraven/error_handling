/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef T_HPP_
#define T_HPP_

namespace error_handling {

namespace detail {

struct T {
	T() noexcept = default;
	T(const T&) noexcept = default;
	T(T&&) noexcept = default;

	T& operator=(T&) noexcept = default;
	T& operator=(T&&) noexcept = default;

	template <class Val>
	T(const Val&) noexcept {}

	template <class Val>
	T(Val&&) noexcept {}

	template <class Val>
	T& operator=(Val&) noexcept {
		return *this;
	}

	template <class Val>
	T& operator=(Val&&) noexcept {
		return *this;
	}

	~T() = default;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* T_HPP_ */
