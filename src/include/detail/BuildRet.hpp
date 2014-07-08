/*
 * BuildRet.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef BUILDRET_HPP_
#define BUILDRET_HPP_

#include "Ret.hpp"
#include "Set.hpp"

namespace error_handling {

namespace detail {

template <template <class...> class Ret, class Val, class Errors>
class BuildRet {
	template <class... OErrors>
	struct Builder {
		using type = Ret<Val, OErrors...>;
	};
public:
	using type = typename SeqToVariadic<Errors, Builder>::type::type;
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* BUILDRET_HPP_ */
