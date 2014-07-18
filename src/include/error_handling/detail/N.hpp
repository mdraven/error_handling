/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef N_HPP_
#define N_HPP_

namespace error_handling {

namespace detail {

struct N {
	N() = delete;
	N(const N&) = delete;
	N(N&&) = delete;

	N& operator=(const N&) = default;
	N& operator=(N&&) = default;
};

} /* namespace detail */

} /* namespace error_handling */


#endif /* N_HPP_ */
