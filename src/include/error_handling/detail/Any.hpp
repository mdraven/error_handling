/*
 * Any.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef ANY_HPP_
#define ANY_HPP_

#define ERROR_HANDLING_ANY 1 /* 1 -- boost::any */

#if ERROR_HANDLING_ANY == 1
#include <boost/any.hpp>
#else
#endif

namespace error_handling {

namespace detail {

template <class... Types>
class Any;

template <class Val, class... Types>
Val unsafe_cast(Any<Types...>& v);

#if ERROR_HANDLING_ANY == 1

template <class... Types>
class Any {
	boost::any any;

	template <class Val, class... OTypes>
	friend Val unsafe_cast(Any<OTypes...>& v);

	template <class... OTypes>
	friend class Any;
public:
	Any() : any() {}

	template <class... OTypes>
	Any(const Any<OTypes...>& v) : any(v.any) {}

	template <class... OTypes>
	Any(Any<OTypes...>&& v) : any(std::move(v.any)) {}

	template<typename Val>
	Any(const Val& v) : any(v) {}

	template<typename Val>
	Any(Val&& v) : any(std::move(v)) {}

	template <class... OTypes>
	Any<Types...>& operator=(const Any<OTypes...>& v) {
		any = v.any;
		return *this;
	}

	template <class... OTypes>
	Any<Types...>& operator=(Any<OTypes...>&& v) noexcept {
		any = std::move(v.any);
		return *this;
	}

	template<typename Val>
	Any<Types...>& operator=(const Val& v) {
		any = v;
		return *this;
	}

	template<typename Val>
	Any<Types...> & operator=(Val&& v) noexcept {
		any = std::move(v);
		return *this;
	}

	bool empty() const noexcept {
		return any.empty();
	}

	const std::type_info& type() const {
		return any.type();
	}
};

template <class Val, class... Types>
Val unsafe_cast(Any<Types...>& v) {
	return boost::any_cast<Val>(v.any);
//		return boost::unsafe_any_cast<Val>(v.any);
}

#elif ERROR_HANDLING_ANY == 2

#endif


} /* namespace detail */

} /* namespace error_handling */


#endif /* ANY_HPP_ */
