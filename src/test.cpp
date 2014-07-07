/*
 * test.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#include "include/error_handling.hpp"

#include <string>

// TODO: delete
#include <boost/mpl/reverse.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/erase_key.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/unique.hpp>
#include <boost/mpl/sort.hpp>

struct ErrA {};
struct ErrB {};
struct ErrC {};

struct Base {};
struct Derived : public Base {};

int main(int argc, char **argv) {
	using error_handling::Ret;
	using error_handling::if_err;
	using error_handling::T;

	std::string str("hello");
	Ret<std::string> ret(std::move(str));

	ErrA erra;
	ErrB errb;

	Ret<std::string, ErrA> ret2(erra);
//	Ret<std::string, ErrA> ret3(errb); // ERR

	Ret<std::string, ErrA> ret4(std::move(erra));
//	Ret<std::string, ErrA> ret5(std::move(errb)); // ERR

	Ret<std::string, ErrA, ErrB> ret6{Ret<std::string, ErrA>()};
//	Ret<std::string, ErrA, ErrB> ret7{Ret<std::string, ErrA, ErrC>()}; // ERR

	ret6 = Ret<std::string, ErrB>();
//	ret6 = Ret<std::string, ErrC>(); // ERR

//	Ret<N> ret8{Ret<std::string>()}; // ERR
	Ret<T> ret9{Ret<std::string>()};

	Ret<Base, ErrA> ret10{Ret<Derived, ErrA>()};
//	Ret<Derived, ErrA> ret11{Ret<Base, ErrA>()}; // ERR

	ret10 = Ret<Derived, ErrA>();
/*	Ret<Derived, ErrA> ret12;
	ret12 = Ret<Bas, ErrA>(); */ //ERR

//	auto x = if_err<ErrB>(std::move(ret6), [](){});
//	auto x = typename error_handling::helpers::BuildRet<Ret, int, error_handling::h::set<int, float>>::type();
//	typename boost::mpl::reverse_remove<boost::mpl::set<int, float>, int, boost::mpl::back_inserter<boost::mpl::set0<> >>::type x;
	using seq = boost::mpl::set<ErrA>;
	using xxx = boost::mpl::remove_if<seq, boost::is_same<ErrB, boost::mpl::_>>;
//	typename xxx::type x;
//	auto dcsf = x;
	typedef typename error_handling::h::difference<seq, boost::mpl::set<ErrA, ErrB>>::type yyy;
	yyy y;
	auto dsfakl = y;

	typedef boost::mpl::set<ErrA,ErrA,ErrB>::type types;
	typedef boost::mpl::erase_key< types,ErrA >::type result;
	typedef boost::mpl::erase_key< result,ErrC >::type result2;

	typedef boost::mpl::set<ErrA,ErrA,ErrB>::type types2;

	printf("%d\n", /*std::is_same<boost::mpl::set<float, float, int>, types>::value*/ boost::mpl::size<result2>::value);
}

