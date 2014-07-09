/*
 * HelpersForIfErr.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef HELPERSFORIFERR_HPP_
#define HELPERSFORIFERR_HPP_

namespace error_handling {

namespace detail {

namespace helpers_for_if_err {

template <class... Args>
struct Wrapper;

template <class... Args>
class RetTypeFor_ValErrors;

template <class... Args>
class ConstraintsFor_ValErrors;

class AssignHelper;

class CallHandler;

} /* namespace helpers_for_if_err */

} /* namespace detail */

} /* namespace error_handling */

#endif /* HELPERSFORIFERR_HPP_ */
