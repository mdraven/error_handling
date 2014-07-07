/*
 * test.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#include "include/error_handling.hpp"

#include <string>

struct ErrA {};
struct ErrB {};
struct ErrC {};

struct Base {};
struct Derived : public Base {};

int main(int argc, char **argv) {
	using error_handling::Ret;
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
}

