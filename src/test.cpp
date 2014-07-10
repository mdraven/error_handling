/*
 * test.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#include <error_handling/error_handling.hpp>

#include <string>
#include <iostream>

// TODO: delete
#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/front.hpp>
// ^^^^

struct ErrA {};
struct ErrB {};
struct ErrC {};

struct Base {};
struct Derived : public Base {};

struct Ops {
	error_handling::Ret<std::string, error_handling::Set<>>
	operator()(ErrA&&) {
		std::cout << "ErrA&&" << std::endl;
		return std::string();
	}

	error_handling::Ret<std::string, error_handling::Set<>>
	operator()(ErrB&&) {
		std::cout << "ErrB&&" << std::endl;
		return std::string();
	}

	error_handling::Ret<std::string, error_handling::Set<>>
	operator()(ErrC&&) {
		std::cout << "ErrC&&" << std::endl;
		return std::string();
	}
};

int main() {
	using error_handling::Ret;
	using error_handling::if_err;
	using error_handling::T;
	using error_handling::Set;

#if 0
	std::string str("hello");
	Ret<std::string, Set<>> ret(std::move(str));

	ErrA erra;
	ErrB errb;

	Ret<std::string, Set<ErrA>> ret2(erra);
//	Ret<std::string, ErrA> ret3(errb); // ERR

	Ret<std::string, Set<ErrA>> ret4(std::move(erra));
//	Ret<std::string, ErrA> ret5(std::move(errb)); // ERR

	Ret<std::string, Set<ErrA, ErrB>> ret6{Ret<std::string, Set<ErrA>>()};
//	Ret<std::string, ErrA, ErrB> ret7{Ret<std::string, ErrA, ErrC>()}; // ERR

	ret6 = Ret<std::string, Set<ErrB>>();
//	ret6 = Ret<std::string, ErrC>(); // ERR

//	Ret<N> ret8{Ret<std::string>()}; // ERR
	Ret<T, Set<>> ret9{Ret<std::string, Set<>>()};

	Ret<Base, Set<ErrA>> ret10{Ret<Derived, Set<ErrA>>()};
//	Ret<Derived, ErrA> ret11{Ret<Base, ErrA>()}; // ERR

	ret10 = Ret<Derived, Set<ErrA>>();
//	Ret<Derived, ErrA> ret12; ret12 = Ret<Base, ErrA>();  //ERR
#endif

	Ret<std::string, Set<ErrA, ErrB>> ret13{std::string("hello")};
	Ret<std::string, Set<ErrA>> ret14 = if_err<Set<ErrB>>(std::move(ret13), boost::fusion::make_list([](ErrB&&) { return; }));
//	Ret<std::string, ErrA> ret15 = if_err<ErrB>(std::move(ret14), [](){}); // ERR
	Ret<std::string, Set<>> ret16 = if_err<Set<ErrA>>(std::move(ret14), boost::fusion::make_list([](ErrA&&) { return; }));

#if 0
//	std::cout << ret16.data() << std::endl;

	Ret<std::string, Set<int>> ret17{int(666)};
	Ret<std::string, Set<>> ret18 = if_err<Set<int>>(std::move(ret17), boost::fusion::make_list([](int&& i) { std::cout << i << std::endl; return; }));

	Ret<std::string, Set<ErrA, ErrB>> ret19{std::string("hello")}; // N14error_handling6detail3RetIISs4ErrA4ErrBEEE
	if_err<Set<ErrB>>(std::move(ret19), boost::fusion::make_list([](ErrB&&) -> Ret<std::string, Set<ErrA>> { return std::string(); })); // N14error_handling6detail3RetIISs4ErrAEEE

	Ret<std::string, Set<ErrA, ErrB>> ret20{ErrB()};
	if_err<Set<ErrB>>(std::move(ret20), boost::fusion::make_list([](ErrB&&) -> Ret<std::string, Set<ErrA>> { std::cout << "err_b" << std::endl; return std::string(); }));

	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret21{ErrB()};
	if_err<Set<ErrA, ErrB, ErrC>>(std::move(ret21), boost::fusion::make_list(Ops()));

	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret22{ErrB()};
//	Ret<std::string, Set<ErrC>> ret23 = if_err<Set<ErrA>>(std::move(ret22), boost::fusion::make_list([](ErrA&&) { return; })); // ERR

	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret24{ErrB()};
	if_err<Set<ErrA, ErrB, ErrC>>(std::move(ret24), boost::fusion::make_list([](ErrB&&) {return;},
			[](ErrC&&) {return;}, [](ErrA&&) {return;}));

//	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret25{ErrB()};
//	if_err<Set<ErrA, ErrB, ErrC>>(std::move(ret25), boost::fusion::make_list([](ErrB&&) {return;},
//			[](ErrC&&) {return;})); // ERR
#endif

//	std::cout << IsUnOp<ErrA, Ops>::value << std::endl;
//	std::cout << IsUnOp<ErrB, Ops>::value << std::endl;
//	std::cout << IsUnOp<ErrC, Ops>::value << std::endl;
//	std::cout << IsUnOp<Base, Ops>::value << std::endl;
//	typename error_handling::detail::UnOpArgSet<error_handling::detail::Set<ErrA, Derived, ErrB, ErrC, Base>, Ops>::type x = 10;

//	error_handling::detail::EnableIfNotUniversalRef<ErrA&&>::value;  // OK
//	typename error_handling::detail::EnableIfNotUniversalRef<ErrA&&>::type xxx1;  // OK
//	typename error_handling::detail::EnableIfNotUniversalRef<ErrA&&>::type::type xxx2; // compile-time ERR
}
