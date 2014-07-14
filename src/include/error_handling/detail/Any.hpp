/*
 * Any.hpp
 *
 *  Created on: Jul 9, 2014
 *      Author: mdraven
 */

#ifndef ANY_HPP_
#define ANY_HPP_

#include <error_handling/detail/EnableIfNotUniversalRef.hpp>

#define ERROR_HANDLING_ANY 2 /* 1 -- boost::any */

#if ERROR_HANDLING_ANY == 1
#include <boost/any.hpp>
#else
#include <error_handling/detail/TypeInfo.hpp>

#include <cassert>
#endif

namespace error_handling {

namespace detail {

template <class Val, class Errors>
class Any;

template <class Val, class OVal, class OErrors>
Val unsafe_cast(Any<OVal, OErrors>& v);

template <class Val, class Errors>
class AutoClearAny {
	Any<Val, Errors>& any;
public:
	AutoClearAny() = delete;
	AutoClearAny(const AutoClearAny<Val, Errors>&) = delete;
	AutoClearAny(AutoClearAny<Val, Errors>&&) = delete;

	AutoClearAny(Any<Val, Errors>& any) noexcept : any(any) {}

	operator Any<Val, Errors>&() {
		return any;
	}

	operator const Any<Val, Errors>&() const {
		return any;
	}

	Any<Val, Errors>& data() {
		return any;
	}

	const Any<Val, Errors>& data() const {
		return any;
	}

	~AutoClearAny() {
		any.clear();
	}
};


#if ERROR_HANDLING_ANY == 1

template <class... Types>
class Any {
	boost::any any;

	template <class Val, class... OTypes>
	friend Val unsafe_cast(Any<OTypes...>& v);

	template <class... OTypes>
	friend class Any;
public:
	Any() : any() {}

	template <class... OTypes>
	Any(const Any<OTypes...>& v) : any(v.any) {}

	template <class... OTypes>
	Any(Any<OTypes...>&& v) : any(std::move(v.any)) {}

	template <class Val>
	Any(const Val& v) : any(v) {}

	template <class Val,
	class = typename EnableIfNotUniversalRef<Val>::type::type>
	Any(Val&& v) : any(std::move(v)) {}

	template <class... OTypes>
	Any<Types...>& operator=(const Any<OTypes...>& v) {
		any = v.any;
		return *this;
	}

	template <class... OTypes>
	Any<Types...>& operator=(Any<OTypes...>&& v) noexcept {
		any = std::move(v.any);
		return *this;
	}

	template <class Val>
	Any<Types...>& operator=(const Val& v) {
		any = v;
		return *this;
	}

	template <class Val,
	class = typename EnableIfNotUniversalRef<Val>::type::type>
	Any<Types...>& operator=(Val&& v) noexcept {
		any = std::move(v);
		return *this;
	}

	bool empty() const noexcept {
		return any.empty();
	}

	const std::type_info& type() const {
		return any.type();
	}
};

template <class Val, class... Types>
Val unsafe_cast(Any<Types...>& v) {
	return boost::any_cast<Val>(v.any);
//		return boost::unsafe_any_cast<Val>(v.any);
}

#elif ERROR_HANDLING_ANY == 2

#if 1

template <template <class> class Action>
class DoAction {
	template <class Errors, class>
	class CheckErrors {
		using Error = typename Front<Errors>::type;
		using TailErrors = typename Remove<Errors, Error>::type;
	public:
		static
		void call(void* from, const std::type_info& from_ti, void* to) {
			if(from_ti == typeid(Error))
				Action<Error>::call(from, to);
			else
				CheckErrors<TailErrors, void>::call(from, from_ti, to);
		}

		static
		void call(const void* from, const std::type_info& from_ti, void* to) {
			if(from_ti == typeid(Error))
				Action<Error>::call(from, to);
			else
				CheckErrors<TailErrors, void>::call(from, from_ti, to);
		}
	};

	template <class Fake>
	class CheckErrors<Set<>, Fake> {
	public:
		static
		void call(const void* from, const std::type_info& from_ti, const void* to) {
			return;
		}
	};
public:
	template <class Val, class Errors>
	static
	void call(void* from, const std::type_info& from_ti, void* to) {
		if(from_ti == typeid(Val))
			Action<Val>::call(from, to);
		else
			CheckErrors<Errors, void>::call(from, from_ti, to);
	}

	template <class Val, class Errors>
	static
	void call(const void* from, const std::type_info& from_ti, void* to) {
		if(from_ti == typeid(Val))
			Action<Val>::call(from, to);
		else
			CheckErrors<Errors, void>::call(from, from_ti, to);
	}
};

template <class Val, class Errors>
class Any {
	char storage[MaxSize<typename Insert<Errors, Val>::type>::value];
	const TypeInfo* ti;

	template <class RVal, class OVal, class OErrors>
	friend RVal unsafe_cast(Any<OVal, OErrors>& v);

	template <class OVal, class OErrors>
	friend class Any;

	template <class OVal>
	void valCopyConstructor(const OVal& v) {
		new(storage) OVal(v);
		ti = TypeInfoHolder<OVal>::getTypeInfo();
	}

	template <class OVal>
	void valMoveConstructor(OVal&& v) {
		new(storage) OVal(std::move(v));
		ti = TypeInfoHolder<OVal>::getTypeInfo();
	}

	template <class T>
	struct CopyConstructorAction {
		static void call(const void* from, void* to) {
			new(to) T(*static_cast<const T*>(from));
		}
	};

	void callCopyConstructor(void* to) const {
		DoAction<CopyConstructorAction>::template call<Val, Errors>(storage, *ti->ti, to);
	}

	template <class T>
	struct MoveConstructorAction {
		static void call(void* from, void* to) {
			new(to) T(std::move(*static_cast<T*>(from)));
		}
	};

	void callMoveConstructor(void* to) {
		DoAction<MoveConstructorAction>::template call<Val, Errors>(storage, *ti->ti, to);
	}

	template <class T>
	struct CopyAssignAction {
		static void call(const void* from, void* to) {
			*static_cast<T*>(to) = *static_cast<const T*>(from);
		}
	};

	void callCopyAssign(void* to) const {
		DoAction<CopyAssignAction>::template call<Val, Errors>(storage, *ti->ti, to);
	}

	template <class T>
	struct MoveAssignAction {
		static void call(void* from, void* to) {
			*static_cast<T*>(to) = std::move(*static_cast<T*>(from));
		}
	};

	void callMoveAssign(void* to) {
		DoAction<MoveAssignAction>::template call<Val, Errors>(storage, *ti->ti, to);
	}

	template <class T>
	struct DestructorAction {
		static void call(void* from, void*) {
			static_cast<T*>(from)->~T();
		}
	};

	void destructor() {
		DoAction<DestructorAction>::template call<Val, Errors>(storage, *ti->ti, nullptr);
		ti = nullptr;
	}
public:
	Any() : ti(nullptr) {}

	Any(const Any<Val, Errors>& v) : ti(nullptr) {
		if(v.ti == nullptr)
			clear();
		else {
			v.callCopyConstructor(storage);
			ti = v.ti;
		}
	}

	Any(Any<Val, Errors>&& v) : ti(nullptr) {
		if(v.ti == nullptr)
			clear();
		else {
			v.callMoveConstructor(storage);
			ti = v.ti;
		}
	}

	template <class OVal, class OErrors>
	Any(const Any<OVal, OErrors>& v) : ti(nullptr) {
		if(v.ti == nullptr)
			clear();
		else {
			v.callCopyConstructor(storage);
			ti = v.ti;
		}
	}

	template <class OVal, class OErrors>
	Any(Any<OVal, OErrors>&& v) : ti(nullptr) {
		if(v.ti == nullptr)
			clear();
		else {
			v.callMoveConstructor(storage);
			ti = v.ti;
		}
	}

	template <class OVal>
	Any(const OVal& v) : ti(nullptr) {
		valCopyConstructor(v);
	}

	template <class OVal,
	class = typename EnableIfNotUniversalRef<OVal>::type::type>
	Any(OVal&& v) : ti(nullptr) {
		valMoveConstructor(std::move(v));
	}

	Any<Val, Errors>& operator=(const Any<Val, Errors>& v) {
		if(this == &v)
			return *this;

		if(v.ti == nullptr)
			clear();
		else {
			if(ti == nullptr)
				v.callCopyConstructor(storage);
			else if(ti == v.ti)
				v.callCopyAssign(storage);
			else {
				destructor();
				v.callCopyConstructor(storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	template <class OVal, class OErrors>
	Any<Val, Errors>& operator=(const Any<OVal, OErrors>& v) {
		if(v.ti == nullptr)
			clear();
		else {
			if(ti == nullptr)
				v.callCopyConstructor(storage);
			else if(ti == v.ti)
				v.callCopyAssign(storage);
			else {
				destructor();
				v.callCopyConstructor(storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	Any<Val, Errors>& operator=(Any<Val, Errors>&& v) noexcept {
		if(this == &v)
			return *this;

		if(v.ti == nullptr)
			clear();
		else {
			if(ti == nullptr)
				v.callMoveConstructor(storage);
			else if(ti == v.ti)
				v.callMoveAssign(storage);
			else {
				destructor();
				v.callMoveConstructor(storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	template <class OVal, class OErrors>
	Any<Val, Errors>& operator=(Any<OVal, OErrors>&& v) noexcept {
		if(v.ti == nullptr)
			clear();
		else {
			if(ti == nullptr)
				v.callMoveConstructor(storage);
			else if(ti == v.ti)
				v.callMoveAssign(storage);
			else {
				destructor();
				v.callMoveConstructor(storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	template <class OVal>
	Any<Val, Errors>& operator=(const OVal& v) {
		if(ti == nullptr)
			valCopyConstructor(v);
		else if(*ti->ti == typeid(OVal)) {
			static_cast<OVal*>(storage)->operator=(v);
		}
		else {
			destructor();
			valCopyConstructor(v);
		}

		return *this;
	}

	template <class OVal,
	class = typename EnableIfNotUniversalRef<OVal>::type::type>
	Any<Val, Errors>& operator=(OVal&& v) noexcept {
		if(ti == nullptr)
			valMoveConstructor(v);
		else if(*ti->ti == typeid(OVal)) {
			static_cast<OVal*>(storage)->operator=(v);
		}
		else {
			destructor();
			valMoveConstructor(v);
		}

		return *this;
	}

	bool empty() const noexcept {
		return ti == nullptr;
	}

	const std::type_info& type() const {
		return (ti == nullptr) ? typeid(void) : *ti->ti;
	}

	void clear() {
		if(ti == nullptr)
			return;
		destructor();
	}

	~Any() {
		clear();
	}
};

#else

template <class Val, class Errors>
class Any {
	char storage[MaxSize<typename Insert<Errors, Val>::type>::value];
	const TypeInfo* ti;

	template <class RVal, class OVal, class OErrors>
	friend RVal unsafe_cast(Any<OVal, OErrors>& v);

	template <class OVal, class OErrors>
	friend class Any;

	template <class OVal>
	void valCopyConstructor(const OVal& v) {
		new(storage) OVal(v);
		ti = TypeInfoHolder<OVal>::getTypeInfo();
	}

	template <class OVal>
	void valMoveConstructor(OVal&& v) {
		new(storage) OVal(std::move(v));
		ti = TypeInfoHolder<OVal>::getTypeInfo();
	}

	void callCopyConstructor(void* to) const {
		assert(ti->copy_constr);
		ti->copy_constr(storage, to);
	}

	void callMoveConstructor(void* to) {
		assert(ti->move_constr);
		ti->move_constr(storage, to);
	}

	void callCopyAssign(void* to) const {
		assert(ti->copy_assign);
		ti->copy_assign(storage, to);
	}

	void callMoveAssign(void* to) {
		assert(ti->move_assign);
		ti->move_assign(storage, to);
	}

	void destructor() {
		ti->destr(storage);
		ti = nullptr;
	}
public:
	Any() : ti(nullptr) {}

	Any(const Any<Val, Errors>& v) : ti(nullptr) {
		if(v.ti == nullptr)
			clear();
		else {
			v.callCopyConstructor(storage);
			ti = v.ti;
		}
	}

	Any(Any<Val, Errors>&& v) : ti(nullptr) {
		if(v.ti == nullptr)
			clear();
		else {
			v.callMoveConstructor(storage);
			ti = v.ti;
		}
	}

	template <class OVal, class OErrors>
	Any(const Any<OVal, OErrors>& v) : ti(nullptr) {
		if(v.ti == nullptr)
			clear();
		else {
			v.callCopyConstructor(storage);
			ti = v.ti;
		}
	}

	template <class OVal, class OErrors>
	Any(Any<OVal, OErrors>&& v) : ti(nullptr) {
		if(v.ti == nullptr)
			clear();
		else {
			v.callMoveConstructor(storage);
			ti = v.ti;
		}
	}

	template <class OVal>
	Any(const OVal& v) : ti(nullptr) {
		valCopyConstructor(v);
	}

	template <class OVal,
	class = typename EnableIfNotUniversalRef<OVal>::type::type>
	Any(OVal&& v) : ti(nullptr) {
		valMoveConstructor(std::move(v));
	}

	Any<Val, Errors>& operator=(const Any<Val, Errors>& v) {
		if(this == &v)
			return *this;

		if(v.ti == nullptr)
			clear();
		else {
			if(ti == nullptr)
				v.callCopyConstructor(storage);
			else if(ti == v.ti)
				v.callCopyAssign(storage);
			else {
				destructor();
				v.callCopyConstructor(storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	template <class OVal, class OErrors>
	Any<Val, Errors>& operator=(const Any<OVal, OErrors>& v) {
		if(v.ti == nullptr)
			clear();
		else {
			if(ti == nullptr)
				v.callCopyConstructor(storage);
			else if(ti == v.ti)
				v.callCopyAssign(storage);
			else {
				destructor();
				v.callCopyConstructor(storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	Any<Val, Errors>& operator=(Any<Val, Errors>&& v) noexcept {
		if(this == &v)
			return *this;

		if(v.ti == nullptr)
			clear();
		else {
			if(ti == nullptr)
				v.callMoveConstructor(storage);
			else if(ti == v.ti)
				v.callMoveAssign(storage);
			else {
				destructor();
				v.callMoveConstructor(storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	template <class OVal, class OErrors>
	Any<Val, Errors>& operator=(Any<OVal, OErrors>&& v) noexcept {
		if(v.ti == nullptr)
			clear();
		else {
			if(ti == nullptr)
				v.callMoveConstructor(storage);
			else if(ti == v.ti)
				v.callMoveAssign(storage);
			else {
				destructor();
				v.callMoveConstructor(storage);
			}

			ti = v.ti;
		}

		return *this;
	}

	template <class OVal>
	Any<Val, Errors>& operator=(const OVal& v) {
		if(ti == nullptr)
			valCopyConstructor(v);
		else if(*ti->ti == typeid(OVal)) {
			static_cast<OVal*>(storage)->operator=(v);
		}
		else {
			destructor();
			valCopyConstructor(v);
		}

		return *this;
	}

	template <class OVal,
	class = typename EnableIfNotUniversalRef<OVal>::type::type>
	Any<Val, Errors>& operator=(OVal&& v) noexcept {
		if(ti == nullptr)
			valMoveConstructor(v);
		else if(*ti->ti == typeid(OVal)) {
			static_cast<OVal*>(storage)->operator=(v);
		}
		else {
			destructor();
			valMoveConstructor(v);
		}

		return *this;
	}

	bool empty() const noexcept {
		return ti == nullptr;
	}

	const std::type_info& type() const {
		return (ti == nullptr) ? typeid(void) : *ti->ti;
	}

	void clear() {
		if(ti == nullptr)
			return;
		destructor();
	}

	~Any() {
		clear();
	}
};

#endif

template <class Val, class OVal, class OErrors>
Val unsafe_cast(Any<OVal, OErrors>& v) {
	assert(v.type() == typeid(Val));
	return *static_cast<Val*>(static_cast<void*>(v.storage));
}

#endif


} /* namespace detail */

} /* namespace error_handling */


#endif /* ANY_HPP_ */
