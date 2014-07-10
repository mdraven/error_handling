/*
 * RetTraits.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef RETTRAITS_HPP_
#define RETTRAITS_HPP_

#include <error_handling/detail/Ret.hpp>
#include <error_handling/detail/Set.hpp>

namespace error_handling {

namespace detail {

template <class... Args>
struct RetTraits;

template <class Val, class... Errors>
struct RetTraits<Ret<Val, Errors...>> {
	using val_type = Val;
	using errors_type = Set<Errors...>;
};

template <class T>
class IsRet {
	template <class... OArgs>
	struct Helper {
		static const bool value = false;
	};

	template <class Val, class... Errors>
	struct Helper<Ret<Val, Errors...>> {
		static const bool value = true;
	};

public:
	static const bool value = Helper<T>::value;
};


} /* namespace detail */

} /* namespace error_handling */


#endif /* RETTRAITS_HPP_ */
