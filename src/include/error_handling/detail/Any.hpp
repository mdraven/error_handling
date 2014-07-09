/*
 * Any.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef ANY_HPP_
#define ANY_HPP_

#ifdef ERROR_HANDLING_BOOST_ANY
#include <boost/any.hpp>
#else
#include <boost/variant.hpp>
#endif

namespace error_handling {

namespace detail {

#ifdef ERROR_HANDLING_BOOST_ANY

template <class... Types>
using Any = boost::any;

template <class Val, class... Types>
Val unsafe_any_cast(Any<Types...>& v) {
	return boost::any_cast<Val>(v);
}

#else

template <class... Types>
using Any = boost::variant<Types...>;

template <class Val, class... Types>
Val unsafe_any_cast(Any<Types...>& v) {
	return boost::get<Val>(v);
}

struct AnyAssign {
	template <class... Args>
	class Wrapper {};

	template <class Type, class... Types, class Any, class OAny>
	static void helper(Any& any, OAny& oany, Wrapper<Type, Types...>) {
		if(oany.type() == typeid(Type))
			any = std::move(boost::get<Type>(oany));
		else
			helper(any, oany, Wrapper<Types...>());
	}

	template <class Any, class OAny>
	static void helper(Any&, OAny&, Wrapper<>) {}
public:
	template <class Val, class Err, class... Errors, class OVal, class... OErrors>
	static void auto_move(Any<Val, Err, Errors...>& any, Any<OVal, OErrors...>& oany) {
		helper(any, oany, Wrapper<OVal, Err, Errors...>());
	}
};

#endif


} /* namespace detail */

} /* namespace error_handling */


#endif /* ANY_HPP_ */
