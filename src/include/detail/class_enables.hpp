/*
 * class_enables.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef CLASS_ENABLES_HPP_
#define CLASS_ENABLES_HPP_

#include "helpers.hpp"

namespace error_handling {

namespace class_enables {

namespace h = error_handling::helpers;

template <class Errors, class Err>
struct Enable_Ret_ValErrors_CopyConstructorFor_Err {
	static const bool value = h::is_contains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_MoveConstructorFor_Err {
	static const bool value = h::is_not_universal_ref<Err>::value &&
			h::is_contains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class OVal, class OErrors, class Val, class Errors>
struct Enable_Ret_ValErrors_MoveConstructorFor_Ret_ValErrors {
	static const bool value = std::is_convertible<OVal, Val>::value &&
			h::is_difference_empty<OErrors, Errors>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_CopyAssignFor_Err {
	static const bool value = h::is_contains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_MoveAssignFor_Err {
	static const bool value = h::is_not_universal_ref<Err>::value &&
			h::is_contains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class OVal, class OErrors, class Val, class Errors>
struct Enable_Ret_ValErrors_MoveAssignFor_Ret_ValErrors {
	static const bool value = std::is_convertible<OVal, Val>::value &&
			h::is_difference_empty<OErrors, Errors>::value;
	using type = std::enable_if<value>;
};

} /* namespace class_enables */

} /* namespace error_handling */


#endif /* CLASS_ENABLES_HPP_ */
