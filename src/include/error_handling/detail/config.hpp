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

#ifndef ERROR_HANDLING_ERROR
#include <iostream>
#define ERROR_HANDLING_ERROR(TYPE, MSG) \
	std::cerr << (MSG) << typeid TYPE .name() << std::endl;

#endif


#define ERROR_HANDLING_CHECK_DOUBLE_IFERR

#endif /* CONFIG_HPP_ */
