/*
 * helpers.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include "Set.hpp"

#include <type_traits>

namespace error_handling {

namespace detail {

template <class T>
struct is_not_universal_ref {
	static const bool value = !std::is_lvalue_reference<T>::value;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* HELPERS_HPP_ */
