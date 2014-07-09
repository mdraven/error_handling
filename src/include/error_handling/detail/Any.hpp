/*
 * Any.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef ANY_HPP_
#define ANY_HPP_

#define ERROR_HANDLING_ANY 1 /* 1 - boost::any */

#if ERROR_HANDLING_ANY == 1
#include <boost/any.hpp>
#else

#endif

namespace error_handling {

namespace detail {

#if ERROR_HANDLING_ANY == 1

template <class... Types>
struct Any : public boost::any {
	using boost::any::any;
};

//using Any = boost::any;

template <class Val, class... Types>
Val unsafe_any_cast(Any<Types...>& v) {
	return boost::any_cast<Val>(v);
}

struct AnyAssign {
	template <class Val, class Err, class... Errors, class OVal, class... OErrors>
	static void auto_move(Any<Val, Err, Errors...>& any, Any<OVal, OErrors...>&& oany) {
		any = std::move(oany);
	}
};

#else

#endif


} /* namespace detail */

} /* namespace error_handling */


#endif /* ANY_HPP_ */
