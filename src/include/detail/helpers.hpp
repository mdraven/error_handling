/*
 * helpers.hpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#ifndef HELPERS_HPP_
#define HELPERS_HPP_

#include <boost/mpl/set.hpp>
#include <boost/mpl/remove.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>

namespace error_handling {

namespace helpers {

using boost::mpl::set;
using boost::mpl::remove;

template <class Seq, class Elem>
struct is_contains {
	static const bool value = boost::mpl::contains<Seq, Elem>::value;
};

template <class Seq1, class Seq2>
struct difference {
	using type = typename boost::mpl::remove_if<Seq1, boost::mpl::contains<Seq2, boost::mpl::_>>::type;
};

template <class Seq1, class Seq2>
struct is_difference_empty {
	static const bool value = boost::mpl::empty<typename difference<Seq1, Seq2>::type>::value;
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

template <class Seq, template <class...> class Builder, class... Acc>
class MPLSetToVariadic {
	using head = typename boost::mpl::front<Seq>::type;
	using tail = typename boost::mpl::remove<Seq, head>::type;
	static const bool is_tail_empty = boost::mpl::empty<tail>::value;
public:
	using type = typename std::conditional<!is_tail_empty,
			MPLSetToVariadic<tail, Builder, Acc..., head>,
			Builder<Acc..., head>>::type;
};

template <template <class...> class Ret, class Val, class Errors>
class BuildRet {
	template <class... OErrors>
	struct Builder {
		using type = Ret<Val, OErrors...>;
	};

	static const bool is_errors_empty = boost::mpl::empty<Errors>::value;
public:
	using type = typename std::conditional<is_errors_empty,
			Builder<>,
			MPLSetToVariadic<Errors, Builder>>::type;
};


} /* namespace helpers */

} /* namespace error_handling */

#endif /* HELPERS_HPP_ */
