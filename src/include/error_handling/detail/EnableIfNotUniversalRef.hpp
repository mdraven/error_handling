/*
 * EnableIfNotUniversalRef.hpp
 *
 *  Created on: Jul 11, 2014
 *      Author: mdraven
 */

#ifndef ENABLEIFNOTUNIVERSALREF_HPP_
#define ENABLEIFNOTUNIVERSALREF_HPP_

#include <type_traits>

namespace error_handling {

namespace detail {

template <class T>
struct EnableIfNotUniversalRef {
	static const bool value = !std::is_lvalue_reference<T>::value;
	using type = std::enable_if<value>;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* ENABLEIFNOTUNIVERSALREF_HPP_ */
