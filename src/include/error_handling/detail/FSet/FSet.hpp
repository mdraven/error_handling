/*
 * FSet.hpp
 *
 *  Created on: Jul 13, 2014
 *      Author: mdraven
 */

#ifndef FSET_HPP_
#define FSET_HPP_

#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/front.hpp>

namespace error_handling {

namespace detail {

namespace f = boost::fusion;

template <class... Args>
f::list<Args...> FSet(Args... args) {
	return f::make_list(std::forward<Args>(args)...);
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* FSET_HPP_ */
