/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef RETTRAITS_HPP_
#define RETTRAITS_HPP_

#include <error_handling/detail/Ret.hpp>
#include <error_handling/detail/Set/Set.hpp>

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

	template <class Val, class Errors>
	struct Helper<Ret<Val, Errors>> {
		using val_type = Val;
		using errors_type = Errors;

		static const bool value = true;
	};

public:
	struct val_type {
		using type = typename Helper<T>::val_type;
	};

	struct errors_type {
		using type = typename Helper<T>::errors_type;
	};

	static const bool value = Helper<T>::value;
};


} /* namespace detail */

} /* namespace error_handling */


#endif /* RETTRAITS_HPP_ */
