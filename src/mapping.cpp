/*
 * for.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: mdraven
 */

#include <error_handling/error_handling.hpp>

#include <cstdio>
#include <cerrno>
#include <cstring>

namespace e = error_handling;

using e::R;
using e::V;
using e::repack;
using e::if_err;

struct ErrA {};
struct ErrB {};
struct ErrC {};
struct ErrD {};
struct ErrE {};
struct ErrF {};

extern R<int, ErrA, ErrB, ErrC, ErrD, ErrE, ErrF> get();

R<int, ErrF, ErrA, ErrB, ErrC, ErrD, ErrE> mapping() {
	return get();
}

