/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef ANY_HPP_
#define ANY_HPP_

#include <error_handling/detail/EnableIfNotUniversalRef.hpp>
#include <error_handling/detail/config.hpp>

#include <limits>
#include <cassert>

namespace error_handling {

namespace detail {

template <class Val, class Errors>
class Any;

template <class Val, class OVal, class OErrors>
Val unsafe_cast(Any<OVal, OErrors>& v);

using TypeIndex = unsigned char;
static const TypeIndex empty_ti = 0;
static const TypeIndex val_ti = 1;
static const TypeIndex errs_ti_base = 2;

template <class Val, class Errors>
class GetTypeIndex {
public:
	template <class T>
	static
	typename std::enable_if<std::is_same<T, Val>::value, size_t>::type
	call() {
		return val_ti;
	}

	template <class T>
	static
	typename std::enable_if<IsContains<Errors, T>::value, size_t>::type
	call() {
		return Index<Errors, T>::value + errs_ti_base;
	}
};

template <class Val, class FromErrors, class ToErrors>
class MapTypeIndex {
	template <class Errors>
	static
	typename std::enable_if<!IsEmpty<Errors>::value, TypeIndex>::type
	errors(TypeIndex ti) {
		using Error = typename Front<Errors>::type;
		if(ti == GetTypeIndex<Val, FromErrors>::template call<Error>())
			return GetTypeIndex<Val, ToErrors>::template call<Error>();
		else
			return errors<typename Remove<Errors, Error>::type>(ti);
	}

	template <class Errors>
	static
	typename std::enable_if<IsEmpty<Errors>::value, TypeIndex>::type
	errors(TypeIndex) {
		ERROR_HANDLING_CRITICAL_ERROR("Cannot map type.");
	}

	template <class Errors>
	static
	typename std::enable_if<!IsEmpty<Errors>::value>::type
	fill(TypeIndex* map) {
		using Error = typename Front<Errors>::type;

		const TypeIndex ind_from = GetTypeIndex<Val, FromErrors>::template call<Error>();
		const TypeIndex ind_to = GetTypeIndex<Val, ToErrors>::template call<Error>();
		map[ind_from] = ind_to;

		fill<typename Remove<Errors, Error>::type>(map);
	}

	template <class Errors>
	static
	typename std::enable_if<IsEmpty<Errors>::value>::type
	fill(TypeIndex*) {}
public:
	static
	TypeIndex
	call(TypeIndex ti) {
		const auto max = std::numeric_limits<TypeIndex>::max();
		(void)max;

		using Types = typename Union<FromErrors, ToErrors>::type;
		TypeIndex map[Size<Types>::value + 2]; // errors + val + empty

#ifndef NDEBUG
		std::fill(std::begin(map), std::end(map), max);
#endif

		map[empty_ti] = empty_ti;
		map[val_ti] = val_ti;

		fill<typename Intersection<FromErrors, ToErrors>::type>(map);

#ifndef NDEBUG
		if(map[ti] == max) {
			ERROR_HANDLING_CRITICAL_ERROR("Cannot map type.");
		}
#endif

		return map[ti];
	}
};

template <class Action, class Val, class Errors>
class DoAction {
	template <class OErrors, class>
	class CheckErrors {
		using Error = typename Front<OErrors>::type;
		using TailErrors = typename Remove<OErrors, Error>::type;
	public:
		static
		void call(void* from, size_t from_ti, void* to) {
			if(from_ti == GetTypeIndex<Val, Errors>::template call<Error>())
				Action::template call<Error>(from, to);
			else
				CheckErrors<TailErrors, void>::call(from, from_ti, to);
		}

		static
		void call(const void* from, size_t from_ti, void* to) {
			if(from_ti == GetTypeIndex<Val, Errors>::template call<Error>())
				Action::template call<Error>(from, to);
			else
				CheckErrors<TailErrors, void>::call(from, from_ti, to);
		}
	};

	template <class Fake>
	class CheckErrors<Set<>, Fake> {
	public:
		static
		void call(const void*, size_t, const void*) {
			return;
		}
	};
public:
	static
	void call(void* from, size_t from_ti, void* to) {
		if(from_ti == GetTypeIndex<Val, Errors>::template call<Val>())
			Action::template call<Val>(from, to);
		else
			CheckErrors<Errors, void>::call(from, from_ti, to);
	}

	static
	void call(const void* from, size_t from_ti, void* to) {
		if(from_ti == GetTypeIndex<Val, Errors>::template call<Val>())
			Action::template call<Val>(from, to);
		else
			CheckErrors<Errors, void>::call(from, from_ti, to);
	}
};

template <class Val, class Errors>
class Any {
	TypeIndex ti; // 0 - empty; 1 - Val; 2... - Errors

	static const size_t max_size = MaxSize<typename Insert<Errors, Val>::type>::value;
	static const size_t max_align = MaxAlign<typename Insert<Errors, Val>::type>::value;
	using Storage = typename std::aligned_storage<max_size, max_align>::type;
	Storage storage;

	template <class RVal, class OVal, class OErrors>
	friend RVal unsafe_cast(Any<OVal, OErrors>& v);

	template <class OVal, class OErrors>
	friend class Any;

	template <class OVal>
	void valCopyConstructor(const OVal& v) {
		new(&storage) OVal(v);
		ti = GetTypeIndex<Val, Errors>::template call<OVal>();
	}

	template <class OVal,
	class = typename EnableIfNotUniversalRef<OVal>::type::type>
	void valMoveConstructor(OVal&& v) {
		new(&storage) OVal(std::move(v));
		ti = GetTypeIndex<Val, Errors>::template call<OVal>();
	}

	struct CopyConstructorAction {
		template <class T>
		static
		typename std::enable_if<std::is_copy_constructible<T>::value>::type
		call(const void* from, void* to) {
			new(to) T(*static_cast<const T*>(from));
		}

		template <class T>
		static
		typename std::enable_if<!std::is_copy_constructible<T>::value>::type
		call(const void*, void*) {
			ERROR_HANDLING_CRITICAL_ERROR("Is not copy constructible.");
		}
	};

	void callCopyConstructor(void* to) const {
		DoAction<CopyConstructorAction, Val, Errors>::call(&storage, ti, to);
	}

	class MoveConstructorAction {
	public:
		template <class T>
		static
		typename std::enable_if<std::is_move_constructible<T>::value>::type
		call(void* from, void* to) {
			new(to) T(std::move(*static_cast<T*>(from)));
		}

		template <class T>
		static
		typename std::enable_if<!std::is_move_constructible<T>::value>::type
		call(void*, void*) {
			ERROR_HANDLING_CRITICAL_ERROR("Is not move constructible.");
		}
	};

	void callMoveConstructor(void* to) {
		DoAction<MoveConstructorAction, Val, Errors>::call(&storage, ti, to);
	}

	struct CopyAssignAction {
		template <class T>
		static void call(const void* from, void* to) {
			*static_cast<T*>(to) = *static_cast<const T*>(from);
		}
	};

	void callCopyAssign(void* to) const {
		DoAction<CopyAssignAction, Val, Errors>::call(&storage, ti, to);
	}

	struct MoveAssignAction {
		template <class T>
		static void call(void* from, void* to) {
			*static_cast<T*>(to) = std::move(*static_cast<T*>(from));
		}
	};

	void callMoveAssign(void* to) {
		DoAction<MoveAssignAction, Val, Errors>::call(&storage, ti, to);
	}

	struct DestructorAction {
		template <class T>
		static void call(void* from, void*) {
			static_cast<T*>(from)->~T();
		}
	};

	void destructor() {
		DoAction<DestructorAction, Val, Errors>::call(&storage, ti, nullptr);
		ti = empty_ti;
	}

	void clear() {
		if(ti == empty_ti)
			return;
		destructor();
	}

	struct Constraints {
		template <class OVal, class OErrors>
		static void anyOValOErr(const Any<OVal, OErrors>& v) {
			static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
			assert((is_convertible_val || v.ti != val_ti));
			(void)v; (void)is_convertible_val;
		}
	};
public:
	Any() : ti(empty_ti) {}

	Any(const Any<Val, Errors>& v) : ti(empty_ti) {
		if(v.ti == empty_ti)
			clear();
		else {
			v.callCopyConstructor(&storage);
			ti = v.ti;
		}
	}

	Any(Any<Val, Errors>&& v) : ti(empty_ti) {
		if(v.ti == empty_ti)
			clear();
		else {
			v.callMoveConstructor(&storage);
			ti = v.ti;
			v.destructor();
		}
	}

	template <class OVal, class OErrors>
	Any(const Any<OVal, OErrors>& v) : ti(empty_ti) {
		Constraints::anyOValOErr(v);

		if(v.ti == empty_ti)
			clear();
		else {
			v.callCopyConstructor(&storage);
			ti = MapTypeIndex<Val, OErrors, Errors>::call(v.ti);
		}
	}

	template <class OVal, class OErrors>
	Any(Any<OVal, OErrors>&& v) : ti(empty_ti) {
		Constraints::anyOValOErr(v);

		if(v.ti == empty_ti)
			clear();
		else {
			v.callMoveConstructor(&storage);
			ti = MapTypeIndex<Val, OErrors, Errors>::call(v.ti);
			v.destructor();
		}
	}

	template <class OVal>
	Any(const OVal& v) : ti(empty_ti) {
		valCopyConstructor(v);
	}

	template <class OVal>
	Any(OVal&& v) : ti(empty_ti) {
		valMoveConstructor(std::move(v));
	}

	Any<Val, Errors>& operator=(const Any<Val, Errors>& v) {
		if(this == &v)
			return *this;

		if(v.ti == empty_ti)
			clear();
		else {
			if(ti == empty_ti)
				v.callCopyConstructor(&storage);
			else if(ti == v.ti)
				v.callCopyAssign(&storage);
			else {
				destructor();
				v.callCopyConstructor(&storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	template <class OVal, class OErrors>
	Any<Val, Errors>& operator=(const Any<OVal, OErrors>& v) {
		Constraints::anyOValOErr(v);

		if(v.ti == empty_ti)
			clear();
		else {
			if(ti == empty_ti)
				v.callCopyConstructor(&storage);
			else if(ti == v.ti)
				v.callCopyAssign(&storage);
			else {
				destructor();
				v.callCopyConstructor(&storage);
			}

			ti = MapTypeIndex<Val, OErrors, Errors>::call(v.ti);
		}

		return *this;
	}

	Any<Val, Errors>& operator=(Any<Val, Errors>&& v) noexcept {
		Constraints::anyOValOErr(v);

		if(this == &v)
			return *this;

		if(v.ti == empty_ti)
			clear();
		else {
			if(ti == empty_ti)
				v.callMoveConstructor(&storage);
			else if(ti == v.ti)
				v.callMoveAssign(&storage);
			else {
				destructor();
				v.callMoveConstructor(&storage);
			}

			ti = v.ti;
			v.destructor();
		}

		return *this;
	}

	template <class OVal, class OErrors>
	Any<Val, Errors>& operator=(Any<OVal, OErrors>&& v) noexcept {
		Constraints::anyOValOErr(v);

		if(v.ti == empty_ti)
			clear();
		else {
			if(ti == empty_ti)
				v.callMoveConstructor(&storage);
			else if(ti == v.ti)
				v.callMoveAssign(&storage);
			else {
				destructor();
				v.callMoveConstructor(&storage);
			}

			ti = MapTypeIndex<Val, OErrors, Errors>::call(v.ti);
			v.destructor();
		}

		return *this;
	}

	template <class OVal>
	Any<Val, Errors>& operator=(const OVal& v) {
		if(ti == empty_ti)
			valCopyConstructor(v);
		else if(ti == GetTypeIndex<Val, Errors>::template call<OVal>()) {
			static_cast<OVal*>(&storage)->operator=(v);
		}
		else {
			destructor();
			valCopyConstructor(v);
		}

		return *this;
	}

	template <class OVal>
	Any<Val, Errors>& operator=(OVal&& v) noexcept {
		if(ti == empty_ti)
			valMoveConstructor(std::move(v));
		else if(ti == GetTypeIndex<Val, Errors>::template call<OVal>()) {
			*static_cast<OVal*>(static_cast<void*>(&storage)) = std::move(v);
		}
		else {
			destructor();
			valMoveConstructor(std::move(v));
		}

		return *this;
	}

	bool empty() const noexcept {
		return ti == empty_ti;
	}

	template <class T>
	size_t isType() const {
		return ti == GetTypeIndex<Val, Errors>::template call<T>();
	}

	~Any() {
		clear();
	}
};

template <class Val, class OVal, class OErrors>
Val unsafe_cast(Any<OVal, OErrors>& v) {
	assert((v.template isType<Val>()));

	Val ret = std::move(*static_cast<Val*>(static_cast<void*>(&v.storage)));
	v.destructor();

	return ret;
}

} /* namespace detail */

} /* namespace error_handling */


#endif /* ANY_HPP_ */
