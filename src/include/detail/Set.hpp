/*
 * Set.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef SET_HPP_
#define SET_HPP_

#include <boost/mpl/set.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/remove.hpp>
#include <boost/mpl/remove_if.hpp>

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
class SeqToVariadic {
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

} /* namespace detail */

} /* namespace error_handling */


#endif /* SET_HPP_ */
