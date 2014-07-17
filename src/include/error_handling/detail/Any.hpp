/*
 * Any.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef ANY_HPP_
#define ANY_HPP_

#include <error_handling/detail/EnableIfNotUniversalRef.hpp>
#include <error_handling/detail/config.hpp>

#include <cassert>

namespace error_handling {

namespace detail {

template <class Val, class Errors>
class Any;

template <class Val, class OVal, class OErrors>
Val unsafe_cast(Any<OVal, OErrors>& v);

template <class Val, class Errors>
class GetTypeIndex {
public:
	template <class T>
	static
	typename std::enable_if<std::is_same<T, Val>::value, size_t>::type
	call() {
		return 1;
	}

	template <class T>
	static
	typename std::enable_if<IsContains<Errors, T>::value, size_t>::type
	call() {
		return Index<Errors, T>::value + 2;
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
	size_t ti; // 0 - empty; 1 - Val; 2... - Errors

	static const size_t max_size = MaxSize<typename Insert<Errors, Val>::type>::value;
	using Storage = typename std::aligned_storage<max_size>::type;
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

	template <class OVal>
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
		ti = 0;
	}

	void clear() {
		if(ti == 0)
			return;
		destructor();
	}

public:
	Any() : ti(0) {}

	Any(const Any<Val, Errors>& v) : ti(0) {
		if(v.ti == 0)
			clear();
		else {
			v.callCopyConstructor(&storage);
			ti = v.ti;
		}
	}

	Any(Any<Val, Errors>&& v) : ti(0) {
		if(v.ti == 0)
			clear();
		else {
			v.callMoveConstructor(&storage);
			ti = v.ti;
			v.destructor();
		}
	}

	template <class OVal, class OErrors>
	Any(const Any<OVal, OErrors>& v) : ti(0) {
		if(v.ti == 0)
			clear();
		else {
			v.callCopyConstructor(&storage);
			ti = v.ti;
		}
	}

	template <class OVal, class OErrors>
	Any(Any<OVal, OErrors>&& v) : ti(0) {
		if(v.ti == 0)
			clear();
		else {
			v.callMoveConstructor(&storage);
			ti = v.ti;
			v.destructor();
		}
	}

	template <class OVal>
	Any(const OVal& v) : ti(0) {
		valCopyConstructor(v);
	}

	template <class OVal>
	Any(OVal&& v) : ti(0) {
		valMoveConstructor(std::move(v));
	}

	Any<Val, Errors>& operator=(const Any<Val, Errors>& v) {
		if(this == &v)
			return *this;

		if(v.ti == 0)
			clear();
		else {
			if(ti == 0)
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
		if(v.ti == 0)
			clear();
		else {
			if(ti == 0)
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

	Any<Val, Errors>& operator=(Any<Val, Errors>&& v) noexcept {
		if(this == &v)
			return *this;

		if(v.ti == 0)
			clear();
		else {
			if(ti == 0)
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
		if(v.ti == 0)
			clear();
		else {
			if(ti == 0)
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

	template <class OVal>
	Any<Val, Errors>& operator=(const OVal& v) {
		if(ti == 0)
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
		if(ti == 0)
			valMoveConstructor(v);
		else if(ti == GetTypeIndex<Val, Errors>::template call<OVal>()) {
			static_cast<OVal*>(&storage)->operator=(v);
		}
		else {
			destructor();
			valMoveConstructor(v);
		}

		return *this;
	}

	bool empty() const noexcept {
		return ti == 0;
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
