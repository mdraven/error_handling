/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
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
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/not.hpp>

namespace error_handling {

namespace detail {

namespace m = boost::mpl;

template <class Seq, class Elem>
struct IsContains {
	static const bool value = m::contains<Seq, Elem>::value;
};

template <class Set>
struct RemoveDuplicates {
	using type = typename m::fold<Set, m::set<>, m::if_<IsContains<m::_1, m::_2>, m::_1, m::insert<m::_1, m::_2>>>::type;
};

template <class... Args>
using Set = typename RemoveDuplicates<m::set<Args...>>::type;

struct SetInserter {
	using state = m::set<>;
	using operation =  m::insert<m::_1, m::_2>;
};

template <class Set, class Elem>
struct Insert {
	using type = typename m::if_<IsContains<Set, Elem>, Set, typename m::insert<Set, Elem>::type>::type;
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

template <class Seq>
struct Front {
	using type = typename m::front<Seq>::type;
};

template <class Seq1, class Seq2>
class Union {
	using WithDupl = typename m::fold<Seq2, Seq1, m::insert<m::_1, m::_2>>::type;
public:
	using type = typename RemoveDuplicates<WithDupl>::type;
};

template <class Seq1, class Seq2>
struct Difference {
	using type = typename m::remove_if<Seq1, m::contains<Seq2, m::_>, SetInserter>::type;
};

template <class Seq1, class Seq2>
struct IsDifferenceEmpty {
	static const bool value = m::empty<typename Difference<Seq1, Seq2>::type>::value;
};

template <class Set1, class Set2>
struct Intersection {
	using type = typename m::remove_if<Set1, m::not_<m::contains<Set2, m::_>>, SetInserter>::type;
};

template <class Set>
class MaxSize {
	using First = typename m::front<Set>::type;
	using type = typename m::fold<Set, m::size_t<sizeof(First)>,
			m::if_<m::less<m::_1, m::size_t<sizeof(m::_2)>>, m::size_t<sizeof(m::_2)>, m::_1>>::type;
public:
	static const std::size_t value = type::value;
};

template <class Set>
class MaxAlign {
	using First = typename m::front<Set>::type;
	using type = typename m::fold<Set, m::size_t<alignof(First)>,
			m::if_<m::less<m::_1, m::size_t<alignof(m::_2)>>, m::size_t<alignof(m::_2)>, m::_1>>::type;
public:
	static const std::size_t value = type::value;
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
struct IsSet {
	static const bool value = m::is_sequence<T>::value;
};

template <class Seq, template <class> class Pred>
struct AccumulateToSet {
	using type = typename m::fold<Seq, m::set<>, m::if_<Pred<m::_2>, m::insert<m::_1, m::_2>, m::_1>>::type;
};

template <class Seq, template <class> class Pred>
class AccumulateToSetFromPred {
	struct Helper {
		template <class T1, class T2>
		struct apply {
			using type = typename Union<T1, typename Pred<T2>::result>::type;
		};
	};
public:
	using type = typename m::fold<Seq, m::set<>, m::if_<Pred<m::_2>, m::bind<Helper, m::_1, m::_2>, m::_1>>::type;
};

template <class Set, class Elem>
class Index {
	using Iter = typename m::find<Set, Elem>::type;
public:
	static const std::size_t value = m::distance<typename m::begin<Set>::type, Iter>::value;
};

} /* namespace detail */

} /* namespace error_handling */


#endif /* SET_HPP_ */
