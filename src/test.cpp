/*
 * test.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#include <error_handling/error_handling.hpp>

#include <string>
#include <iostream>

struct ErrA {};
struct ErrB {};
struct ErrC {};

struct Base {};
struct Derived : public Base {};

struct Ops {
	error_handling::Ret<std::string, ErrB, ErrC>
	operator()(ErrA&&) {
		std::cout << "ErrA&&" << std::endl;
		return std::string();
	}

	error_handling::Ret<std::string, ErrA, ErrC>
	operator()(ErrB&&) {
		std::cout << "ErrB&&" << std::endl;
		return std::string();
	}

	error_handling::Ret<std::string, ErrA, ErrB>
	operator()(ErrC&&) {
		std::cout << "ErrC&&" << std::endl;
		return std::string();
	}
};


#include <vector>
template <template <class...> class X>
struct ZZ {
	void func() {
		X<int>();
	}
};

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
//	Ret<Derived, ErrA> ret12; ret12 = Ret<Base, ErrA>();  //ERR

	Ret<std::string, ErrA, ErrB> ret13{std::string("hello")};
	Ret<std::string, ErrA> ret14 = if_err<ErrB>(std::move(ret13), [](ErrB&&) { return; });
//	Ret<std::string, ErrA> ret15 = if_err<ErrB>(std::move(ret14), [](){}); // ERR
	Ret<std::string> ret16 = if_err<ErrA>(std::move(ret14), [](ErrA&&) { return; });

	std::cout << ret16.data() << std::endl;

	Ret<std::string, int> ret17{int(666)};
	Ret<std::string> ret18 = if_err<int>(std::move(ret17), [](int&& i){ std::cout << i << std::endl; return; });

	Ret<std::string, ErrA, ErrB> ret19{std::string("hello")}; // N14error_handling6detail3RetIISs4ErrA4ErrBEEE
	if_err<ErrB>(std::move(ret19), [](ErrB&&) -> Ret<std::string, ErrA> { return std::string(); }); // N14error_handling6detail3RetIISs4ErrAEEE

	Ret<std::string, ErrA, ErrB> ret20{ErrB()};
	if_err<ErrB>(std::move(ret20), [](ErrB&&) -> Ret<std::string, ErrA> { std::cout << "err_b" << std::endl; return std::string(); });

	Ret<std::string, ErrA, ErrB, ErrC> ret21{ErrB()};
//	if_err<ErrA, ErrB, ErrC>(std::move(ret21), Ops());

	ZZ<std::vector>().func();

//	std::cout << IsUnOp<ErrA, Ops>::value << std::endl;
//	std::cout << IsUnOp<ErrB, Ops>::value << std::endl;
//	std::cout << IsUnOp<ErrC, Ops>::value << std::endl;
//	std::cout << IsUnOp<Base, Ops>::value << std::endl;
//	typename error_handling::detail::UnOpArgSet<error_handling::detail::Set<ErrA, Derived, ErrB, ErrC, Base>, Ops>::type x = 10;
}
