/*
 * Type.hpp
 *
 *  Created on: Jul 16, 2014
 *      Author: mdraven
 */

#ifndef TYPE_HPP_
#define TYPE_HPP_

#include <typeinfo>

namespace error_handling {

namespace detail {

class Type {
	const std::type_info* ti;

	template <class T>
	friend Type getType();
public:
	Type() : ti(nullptr) {}
	Type(std::nullptr_t) : ti(nullptr) {}

	Type(const Type&) = default;

	Type& operator=(const Type&) = default;

	Type& operator=(std::nullptr_t) {
		ti = nullptr;
		return *this;
	}

	bool operator==(const Type& t) const {
		return *t.ti == *ti;
	}

	bool operator==(std::nullptr_t) const {
		return ti == nullptr;
	}

	bool operator!=(const Type& t) const {
		return !(*this == t);
	}

	bool operator!=(std::nullptr_t) const {
		return !(*this == nullptr);
	}
};

template <class T>
Type getType() {
	Type t;
	t.ti = &typeid(T);
	return t;
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* TYPE_HPP_ */
