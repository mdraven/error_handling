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

template <class CErrors, class Val, class Errors>
class RetTypeFor_ValErrors;

template <class CErrors, class UnOps, class Val, class Errors>
class ConstraintsFor_ValErrors;

class AssignHelper;

class CallHandler;

} /* namespace helpers_for_if_err */

} /* namespace detail */

} /* namespace error_handling */

#endif /* HELPERSFORIFERR_HPP_ */
