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

namespace other {

struct Break {};

template <class FIter, class LIter, class F>
void For(FIter first, LIter last, F f) {
	for(auto it = first; it != last; ++it) {
		bool f_break = false;

		e::R<V, Break> r1 = f(*it);
		if_err<Break>(std::move(r1), [&f_break](Break&&) { f_break = true; });

		if(f_break)
			break;
	}
}

} /* namespace other */

int main() {

}

