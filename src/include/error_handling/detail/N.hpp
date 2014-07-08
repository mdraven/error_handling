/*
 * N.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef N_HPP_
#define N_HPP_

namespace error_handling {

namespace detail {

struct N {
	N() = delete;
	N(const N&) = delete;
	N(N&&) = delete;

	N& operator=(const N&) = delete;
	N& operator=(N&&) = delete;
};

} /* namespace detail */

} /* namespace error_handling */


#endif /* N_HPP_ */
