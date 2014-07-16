/*
 * config.hpp
 *
 *  Created on: Jul 11, 2014
 *      Author: mdraven
 */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#ifndef ERROR_HANDLING_CRITICAL_ERROR
#include <stdexcept>

#define ERROR_HANDLING_CRITICAL_ERROR(MSG) \
	throw std::runtime_error((MSG));

#endif

#ifndef NDEBUG
  #include <iostream>
  #define ERROR_HANDLING_DEBUG_MSG(TYPE, MSG) \
	  std::cerr << (MSG) << typeid TYPE .name() << std::endl;
#else
  #define ERROR_HANDLING_DEBUG_MSG(TYPE, MSG)
#endif


#define ERROR_HANDLING_CHECK_EMPTY_RET

#define ERROR_HANDLING_UUID_SUPPORT

#endif /* CONFIG_HPP_ */
