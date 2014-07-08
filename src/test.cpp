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
#include <boost/mpl/set.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/erase_key.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/unique.hpp>
#include <boost/mpl/sort.hpp>
// delete ^

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
	ret12 = Ret<Base, ErrA>(); */ //ERR

	Ret<std::string, ErrA> ret13 = if_err<ErrB>(std::move(ret6), [](){});
	Ret<std::string, ErrA> ret14 = if_err<ErrB>(std::move(ret13), [](){}); // TODO: должен не компилироваться, а он компилируется
	Ret<std::string> ret15 = if_err<ErrA>(std::move(ret13), [](){});


	//	typename error_handling::helpers::BuildRet<Ret, int, error_handling::h::Typelist<int, float, char, ErrA, ErrB>>::type x = 10;
//	using namespace boost::mpl;
//	typedef set<ErrA, ErrB, ErrC, ErrB> t1;
//	typedef begin<t1>::type t2;
//	typedef advance<t2, int_<3>>::type::type t3;
//	t3 x = 10;
}

