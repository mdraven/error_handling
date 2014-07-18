/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef ENABLEIFNOTUNIVERSALREF_HPP_
#define ENABLEIFNOTUNIVERSALREF_HPP_

#include <type_traits>

namespace error_handling {

namespace detail {

template <class T>
struct EnableIfNotUniversalRef {
	static const bool value = !std::is_lvalue_reference<T>::value;
	using type = std::enable_if<value>;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* ENABLEIFNOTUNIVERSALREF_HPP_ */
