/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef BUILDRET_HPP_
#define BUILDRET_HPP_

#include <error_handling/detail/Ret.hpp>
#include <error_handling/detail/Set.hpp>

namespace error_handling {

namespace detail {

template <template <class...> class Ret, class Val, class Errors>
class BuildRet {
	template <class... OErrors>
	struct Builder {
		using type = Ret<Val, OErrors...>;
	};
public:
	using type = typename SeqToVariadicType<Errors, Builder>::type::type;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* BUILDRET_HPP_ */
