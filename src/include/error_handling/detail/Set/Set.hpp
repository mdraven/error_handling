/*
 * Set.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef SET_HPP_
#define SET_HPP_

#include <boost/mpl/set.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/remove.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/is_sequence.hpp>

namespace error_handling {

namespace detail {

namespace m = boost::mpl;

template <class... Args>
using Set = m::set<Args...>;

struct SetInserter {
	using state = m::set<>;
	using operation =  m::insert<m::_1, m::_2>;
};

template <class Set, class Elem>
struct Remove {
	using type = typename m::remove<Set, Elem, SetInserter>::type;
};

template <class Seq>
struct IsEmpty {
	static const bool value = m::empty<Seq>::value;
};

template <class Seq>
struct Size {
	static const size_t value = m::size<Seq>::value;
};

template <class Seq, template <class> class Pred>
struct AccumulateToSet {
	using type = typename m::fold<Seq, m::set<>, m::if_<Pred<m::_2>, m::insert<m::_1, m::_2>, m::_1>>::type;
};

template <class Seq>
struct Front {
	using type = typename m::front<Seq>::type;
};

template <class Seq, class Elem>
struct IsContains {
	static const bool value = m::contains<Seq, Elem>::value;
};

template <class Seq1, class Seq2>
struct Difference {
	using type = typename m::remove_if<Seq1, m::contains<Seq2, m::_>, SetInserter>::type;
};

template <class Seq1, class Seq2>
struct IsDifferenceEmpty {
	static const bool value = m::empty<typename Difference<Seq1, Seq2>::type>::value;
};

template <class Seq, template <class...> class Builder>
class SeqToVariadicType {
	template <size_t i = m::size<Seq>::value, class... Acc>
	struct Helper {
		using Elem = typename m::advance<typename m::begin<Seq>::type, m::int_<i-1>>::type::type;
		using type = typename Helper<i-1, Acc..., Elem>::type;
	};

	template <class... Acc>
	struct Helper<0, Acc...> {
		using type = Builder<Acc...>;
	};
public:
	using type = typename Helper<m::size<Seq>::value>::type;
};

template <class T>
class IsSet {
public:
	static const bool value = m::is_sequence<T>::value;
};


} /* namespace detail */

} /* namespace error_handling */


#endif /* SET_HPP_ */
