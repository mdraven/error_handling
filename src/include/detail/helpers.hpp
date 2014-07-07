/*
 * helpers.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <loki/Typelist.h>
#include <type_traits>

namespace error_handling {

namespace helpers {

namespace l = Loki::TL;

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

template <class Errors, class Err>
struct Enable_Ret_ValErrors_CopyConstructorFor_Err {
	static const bool value = is_contains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_MoveConstructorFor_Err {
	static const bool value = is_not_universal_ref<Err>::value &&
			is_contains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class OVal, class OErrors, class Val, class Errors>
struct Enable_Ret_ValErrors_MoveConstructorFor_Ret_ValErrors {
	static const bool value = std::is_convertible<OVal, Val>::value &&
			is_difference_empty<OErrors, Errors>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_CopyAssignFor_Err {
	static const bool value = is_contains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_MoveAssignFor_Err {
	static const bool value = is_not_universal_ref<Err>::value &&
			is_contains<Errors, Err>::value;
	using type = std::enable_if<value>;
};

template <class OVal, class OErrors, class Val, class Errors>
struct Enable_Ret_ValErrors_MoveAssignFor_Ret_ValErrors {
	static const bool value = std::is_convertible<OVal, Val>::value &&
			is_difference_empty<OErrors, Errors>::value;
	using type = std::enable_if<value>;
};

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
			TypelistToVariadic<Errors, Builder>>::type::type;
};

} /* namespace helpers */

} /* namespace error_handling */

#endif /* HELPERS_HPP_ */
