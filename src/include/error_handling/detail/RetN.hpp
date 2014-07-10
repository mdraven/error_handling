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
#include <error_handling/detail/Set.hpp>

namespace error_handling {

namespace detail {

template <>
class Ret<N, Set<>> final {
public:
	Ret() noexcept = default;

	Ret(const Ret<N, Set<>>&) noexcept = default;
	Ret(Ret<N, Set<>>&&) noexcept = default;

	Ret<N, Set<>>& operator=(const Ret<N, Set<>>&) noexcept = default;
	Ret<N, Set<>>& operator=(Ret<N, Set<>>&&) noexcept = default;

	~Ret() noexcept = default;
};

} /* namespace detail */

} /* namespace error_handling */


#endif /* RETN_HPP_ */
