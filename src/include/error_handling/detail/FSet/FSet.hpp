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

template <class FSet>
auto fpop_front(FSet fset) -> decltype(f::pop_front(std::forward<FSet>(fset))) {
	return f::pop_front(std::forward<FSet>(fset));
}

template <class FSet>
auto ffront(FSet fset) -> decltype(f::front(std::forward<FSet>(fset))) {
	return f::front(std::forward<FSet>(fset));
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* FSET_HPP_ */
