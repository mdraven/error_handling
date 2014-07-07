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

int main(int argc, char **argv) {
	std::string str("hello");
	error_handling::Ret<std::string> ret(std::move(str));

	ErrA erra;
	ErrB errb;

	error_handling::Ret<std::string, ErrA> ret2(erra);
//	error_handling::Ret<std::string, ErrA> ret3(errb); // ERR

	error_handling::Ret<std::string, ErrA> ret4(std::move(erra));
//	error_handling::Ret<std::string, ErrA> ret5(std::move(errb)); // ERR

	error_handling::Ret<std::string, ErrA, ErrB> ret6{error_handling::Ret<std::string, ErrA>()};
//	error_handling::Ret<std::string, ErrA, ErrB> ret7{error_handling::Ret<std::string, ErrA, ErrC>()}; // ERR
}

