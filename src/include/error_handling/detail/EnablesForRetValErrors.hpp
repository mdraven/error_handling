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

namespace enables_for_ret_valerrors {

template <class Errors, class Err>
struct EnableCopyConstructorFor_Err {
	static const bool value = IsContains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct EnableMoveConstructorFor_Err {
	static const bool value = is_not_universal_ref<Err>::value &&
			IsContains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct EnableCopyAssignFor_Err {
	static const bool value = IsContains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct EnableMoveAssignFor_Err {
	static const bool value = is_not_universal_ref<Err>::value &&
			IsContains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class OVal, class OErrors, class Val, class Errors>
struct EnableMoveAssignFor_Ret_ValErrors {
	static const bool value = std::is_convertible<OVal, Val>::value &&
			IsDifferenceEmpty<OErrors, Errors>::value;
	using type = std::enable_if<value>;
};

} /* namespace enables_for_ret_valerrors */

} /* namespace detail */

} /* namespace error_handling */

#endif /* ENABLESFORRETVALERRORS_HPP_ */
