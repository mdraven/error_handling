/*
 * helpers.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include "typelist.hpp"

#include <loki/Typelist.h>
#include <type_traits>

namespace error_handling {

namespace helpers {

namespace l = Loki::TL;
namespace t = error_handling::typelist;

using l::EraseAll;

template <class Typelist, class Elem>
struct is_contains {
	static const bool value = l::IndexOf<Typelist, Elem>::value != -1;
};

template <class Typelist1, class Typelist2>
struct difference {
	template <bool is_end, class T1, class T2>
	struct dispatcher {
		using type = typename l::EraseAll<T1, typename T2::Head>::Result;
	};

	template <class T1, class T2>
	struct dispatcher<false, T1, T2> {
		using type = typename difference<typename l::EraseAll<Typelist1, typename Typelist2::Head>::Result, typename Typelist2::Tail>::type;
	};

	static const bool is_end = l::Length<Typelist2>::value == 1;
	using type = typename dispatcher<is_end, Typelist1, Typelist2>::type;
};

template <class Seq1, class Seq2>
struct is_difference_empty {
	static const bool value = l::Length<typename difference<Seq1, Seq2>::type>::value == 0;
};

template <class T>
struct is_not_universal_ref {
	static const bool value = !std::is_lvalue_reference<T>::value;
};

template <template <class...> class Ret, class Val, class Errors>
class BuildRet {
	template <class... OErrors>
	struct Builder {
		using type = Ret<Val, OErrors...>;
	};

	static const bool is_errors_empty = l::Length<Errors>::value == 0;
public:
	using type = typename std::conditional<is_errors_empty,
			Builder<>,
			t::TypelistToVariadic<Errors, Builder>>::type::type;
};

} /* namespace helpers */

} /* namespace error_handling */

#endif /* HELPERS_HPP_ */
