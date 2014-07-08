/*
 * unsafe_access_to_internal_data.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef UNSAFE_ACCESS_TO_INTERNAL_DATA_HPP_
#define UNSAFE_ACCESS_TO_INTERNAL_DATA_HPP_

#include "Ret.hpp"

#include <boost/any.hpp>

namespace error_handling {

namespace detail {

template <class Val, class... Errors>
boost::any& unsafe_access_to_internal_data(Ret<Val, Errors...>& v) {
	return v.v;
}

template <class Val>
Val& unsafe_access_to_internal_data(Ret<Val>& v) {
	return v.v;
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* UNSAFE_ACCESS_TO_INTERNAL_DATA_HPP_ */
