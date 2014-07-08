/*
 * helpers.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include "set.hpp"

#include <type_traits>

namespace error_handling {

namespace helpers {

namespace s = error_handling::set;

template <class Seq, class Elem>
struct is_contains {
	static const bool value = s::contains<Seq, Elem>::value;
};

template <class Seq1, class Seq2>
struct difference {
	using type = typename s::remove_if<Seq1, s::contains<Seq2, s::_>, s::SetInserter>::type;
};

template <class Seq1, class Seq2>
struct is_difference_empty {
	static const bool value = s::empty<typename difference<Seq1, Seq2>::type>::value;
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

	static const bool is_errors_empty = s::empty<Errors>::value;
public:
	using type = typename s::SeqToVariadic<Errors, Builder>::type::type;/*typename std::conditional<is_errors_empty,
			Builder<>,
			s::SeqToVariadic<Errors, Builder>>::type::type;*/
};

} /* namespace helpers */

} /* namespace error_handling */

#endif /* HELPERS_HPP_ */
