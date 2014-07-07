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
	using type = typename std::enable_if<is_contains<Errors, Err>::value>;
};

template <class Errors, class Err>
struct Enable_Ret_ValErrors_MoveConstructorFor_Err {
	using type = typename std::enable_if<is_not_universal_ref<Err>::value &&
			is_contains<Errors, Err>::value>;
};

template <class Args, class Errors>
struct Enable_Ret_ValErrors_MoveConstructorFor_Ret_ValErrors {
	using type = typename std::enable_if<is_difference_empty<Args, Errors>::value>;
};

} /* namespace helpers */

} /* namespace error_handling */

#endif /* HELPERS_HPP_ */
