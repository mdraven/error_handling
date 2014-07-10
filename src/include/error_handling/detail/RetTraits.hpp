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

template <class T>
class IsRet {
	template <class... OArgs>
	struct Helper {
		using val_type = void;
		using errors_type = void;

		static const bool value = false;
	};

	template <class Val, class... Errors>
	struct Helper<Ret<Val, Errors...>> {
		using val_type = Val;
		using errors_type = Set<Errors...>;

		static const bool value = true;
	};

public:
	using val_type = typename Helper<T>::val_type;
	using errors_type = typename Helper<T>::errors_type;

	static const bool value = Helper<T>::value;
};


} /* namespace detail */

} /* namespace error_handling */


#endif /* RETTRAITS_HPP_ */
