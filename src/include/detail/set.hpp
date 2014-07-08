/*
 * set.hpp
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

namespace set {

using boost::mpl::set;
using boost::mpl::insert;
using boost::mpl::contains;
using boost::mpl::remove;
using boost::mpl::remove_if;
using boost::mpl::empty;
using boost::mpl::size;
using boost::mpl::advance;
using boost::mpl::begin;
using boost::mpl::int_;
using boost::mpl::_;
using boost::mpl::_1;
using boost::mpl::_2;

struct SetInserter {
	typedef set<> state;
	typedef insert<_1, _2> operation;
};

template <class Seq, template <class...> class Builder>
class SeqToVariadic {
	template <size_t i = size<Seq>::value, class... Acc>
	struct Helper {
		using Elem = typename advance<typename begin<Seq>::type, int_<i-1>>::type::type;
		using type = typename Helper<i-1, Acc..., Elem>::type;
	};

	template <class... Acc>
	struct Helper<0, Acc...> {
		using type = Builder<Acc...>;
	};
public:
	using type = typename Helper<size<Seq>::value>::type;
};

} /* namespace set */

} /* namespace error_handling */


#endif /* SET_HPP_ */
