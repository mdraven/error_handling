/*
 * EnablesForRetValErrors.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef ENABLESFORRETVALERRORS_HPP_
#define ENABLESFORRETVALERRORS_HPP_

#include "helpers.hpp"

namespace error_handling {

namespace detail {

template <class Errors, class Err>
struct Enable_Ret_ValErrors_CopyConstructorFor_Err {
	static const bool value = IsContains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_MoveConstructorFor_Err {
	static const bool value = is_not_universal_ref<Err>::value &&
			IsContains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class OVal, class OErrors, class Val, class Errors>
struct Enable_Ret_ValErrors_MoveConstructorFor_Ret_ValErrors {
	static const bool value = std::is_convertible<OVal, Val>::value &&
			IsDifferenceEmpty<OErrors, Errors>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_CopyAssignFor_Err {
	static const bool value = IsContains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_MoveAssignFor_Err {
	static const bool value = is_not_universal_ref<Err>::value &&
			IsContains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class OVal, class OErrors, class Val, class Errors>
struct Enable_Ret_ValErrors_MoveAssignFor_Ret_ValErrors {
	static const bool value = std::is_convertible<OVal, Val>::value &&
			IsDifferenceEmpty<OErrors, Errors>::value;
	using type = std::enable_if<value>;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* ENABLESFORRETVALERRORS_HPP_ */
