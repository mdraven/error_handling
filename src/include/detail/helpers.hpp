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

namespace error_handling {

namespace helpers {

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

template <class Args, class Errors>
struct Enable_Ret_ValErrors_MoveAssignFor_Ret_ValErrors {
	static const bool value = is_difference_empty<Args, Errors>::value;
	using type = std::enable_if<value>;
};

} /* namespace helpers */

} /* namespace error_handling */

#endif /* HELPERS_HPP_ */
