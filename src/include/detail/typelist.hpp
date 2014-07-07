/*
 * typelist.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef TYPELIST_HPP_
#define TYPELIST_HPP_

#include <loki/Typelist.h>
#include <type_traits>

namespace error_handling {

namespace typelist {

namespace l = Loki::TL;

template <class Head, class... Elems>
struct BuildTypelist {
	using type = Loki::Typelist<Head, typename BuildTypelist<Elems...>::type>;
};

template <class Head>
struct BuildTypelist<Head> {
	using type = Loki::Typelist<Head, Loki::NullType>;
};

template<class... Elems>
using Typelist = typename BuildTypelist<Elems...>::type;

template <class Typelist, template <class...> class Builder, class... Acc>
class TypelistToVariadic {
	using head = typename Typelist::Head;
	using tail = typename Typelist::Tail;
	static const bool is_tail_empty = l::Length<tail>::value == 0;
public:
	using type = typename std::conditional<!is_tail_empty,
			TypelistToVariadic<tail, Builder, Acc..., head>,
			Builder<Acc..., head>>::type::type;
};

} /* namespace typelist */

} /* namespace error_handling */

#endif /* TYPELIST_HPP_ */
