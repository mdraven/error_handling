/*
 * error_handling.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef ERROR_HANDLING_HPP_
#define ERROR_HANDLING_HPP_

#include "detail/RetN.hpp"
#include "detail/RetT.hpp"
#include "detail/RetVal.hpp"
#include "detail/RetValErrors.hpp"
#include "detail/if_err.hpp"
#include "detail/T.hpp"
#include "detail/N.hpp"
#include "detail/V.hpp"

namespace error_handling {

using error_handling::detail::Ret;

using error_handling::detail::if_err;

using error_handling::detail::T;
using error_handling::detail::N;
using error_handling::detail::V;

} /* namespace error_handling */

#endif /* ERROR_HANDLING_HPP_ */
