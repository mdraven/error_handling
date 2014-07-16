/*
 * Type.hpp
 *
 *  Created on: Jul 16, 2014
 *      Author: mdraven
 */

#ifndef TYPE_HPP_
#define TYPE_HPP_

#include <error_handling/detail/config.hpp>

#include <typeinfo>
#include <algorithm>

namespace error_handling {

namespace detail {

using UUID = std::array<std::uint32_t, 4>;

template <class T>
struct TypeUUID {
	/*
	static
	constexpr const UUID id{{0, 0, 0, 0}}; */
};

template <class T>
class HasTypeUUID {
	template <class W>
	struct Wrapper {};

	template <class W, const UUID* = &TypeUUID<W>::id>
	static
	std::true_type check(const Wrapper<W>&);

	static
	std::false_type check(...);
public:
	static const bool value = decltype(check(Wrapper<T>()))::value;
};

template <class T>
class GetUUIDPointer {
	template <bool has_uuid, class>
	struct H {
		static
		constexpr const UUID* value = &TypeUUID<T>::id;
	};

	template <class Fake>
	struct H<false, Fake> {
		static
		constexpr const UUID* value = nullptr;
	};
public:
	static
	constexpr const UUID* value = H<HasTypeUUID<T>::value, void>::value;
};

class Type {
	const std::type_info* ti;
#ifdef ERROR_HANDLING_UUID_SUPPORT
	const UUID* uuid;
#endif

	template <class T>
	friend Type getType();

	static
	bool cmp(const UUID* a, const UUID* b) {printf("xxx\n");
		return std::equal(std::begin(*a), std::end(*a), std::begin(*b));
	}
public:
	Type() :
		ti(nullptr),
		uuid(nullptr) {}

	Type(std::nullptr_t) :
		ti(nullptr),
		uuid(nullptr) {}

	Type(const Type&) = default;

	Type& operator=(const Type&) = default;

	Type& operator=(std::nullptr_t) {
		ti = nullptr;
		uuid = nullptr;
		return *this;
	}

	template <class T>
	typename std::enable_if<HasTypeUUID<T>::value, bool>::type
	equal() const {
		if(uuid == nullptr)
			return false;
		else
			return cmp(uuid, &TypeUUID<T>::id);
	}

	template <class T>
	typename std::enable_if<!HasTypeUUID<T>::value, bool>::type
	equal() const {
		if(uuid != nullptr)
			return false;
		else
			return *ti == typeid(T);
	}

	bool operator==(const Type& t) const {
		if(uuid == nullptr && t.uuid == nullptr)
			return *t.ti == *ti;
		else if(uuid != nullptr && t.uuid != nullptr)
			return cmp(uuid, t.uuid);
		else
			return false;
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
Type
getType() {
	Type t;
	t.ti = &typeid(T);
	t.uuid = GetUUIDPointer<T>::value;
	return t;
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* TYPE_HPP_ */
