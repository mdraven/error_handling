/*
 * BuildRet.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef BUILDRET_HPP_
#define BUILDRET_HPP_

#include "Ret.hpp"
#include "Set.hpp"

namespace error_handling {

namespace detail {

template <template <class...> class Ret, class Val, class Errors>
class BuildRet;

} /* namespace detail */

} /* namespace error_handling */

#endif /* BUILDRET_HPP_ */
