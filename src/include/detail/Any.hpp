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

using Any = boost::any;

template <class Val>
Val unsafe_any_cast(Any& v) {
	return boost::any_cast<Val>(v);
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* ANY_HPP_ */
