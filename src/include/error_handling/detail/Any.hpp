/*
 * Any.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef ANY_HPP_
#define ANY_HPP_

#include <boost/any.hpp>

namespace error_handling {

namespace detail {

template <class... Types>
using Any = boost::any;

template <class Val, class... Types>
Val unsafe_any_cast(Any<Types...>& v) {
	return boost::any_cast<Val>(v);
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* ANY_HPP_ */
