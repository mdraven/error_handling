/*
 * RetN.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef RETN_HPP_
#define RETN_HPP_

#include "Ret.hpp"
#include "N.hpp"

namespace error_handling {

namespace detail {

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

} /* namespace detail */

} /* namespace error_handling */


#endif /* RETN_HPP_ */
