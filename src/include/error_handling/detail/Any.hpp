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
#include <typeinfo>
#endif

namespace error_handling {

namespace detail {

template <class Val, class Errors>
class Any;

template <class Val, class Errors>
Val unsafe_cast(Any<Val, Errors>& v);

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

struct TypeInfo {
	using Destructor = void(*)(void*);
	using CopyConstr = void(*)(const void*, void*);
	using MoveConstr = void(*)(void*, void*);
	using CopyAssign = void(*)(const void*, void*);
	using MoveAssign = void(*)(void*, void*);

	const std::type_info* ti;
	Destructor destr;

	CopyConstr copy_constr;
	MoveConstr move_constr;

	CopyAssign copy_assign;
	MoveAssign move_assign;

//	EqOper* eq;
};

template <class Type>
class TypeInfoHolder {
	static const TypeInfo* instance;

	static void wrapperForDestructor(void* data) {
		static_cast<Type*>(data)->~Type();
	}

	static void wrapperForCopyConstr(const void* from, void* to) {
		new(to) Type(*static_cast<const Type*>(from));
	}

	static void wrapperForMoveConstr(void* from, void* to) {
		new(to) Type(std::move(*static_cast<Type*>(from)));
	}

	static void wrapperForCopyAssign(const void* from, void* to) {
		static_cast<Type*>(to)->operator=(*static_cast<const Type*>(from));
	}

	static void wrapperForMoveAssign(void* from, void* to) {
		static_cast<Type*>(to)->operator=(std::move(*static_cast<const Type*>(from)));
	}

public:
	static const TypeInfo* getTypeInfo() {
		if(instance == nullptr) {
			// TODO: need race protect
			static TypeInfo ti;

			ti.ti = &typeid(Type);

			ti.destr = &wrapperForDestructor;
			ti.copy_constr = &wrapperForCopyConstr;
			ti.move_constr = &wrapperForMoveConstr;

			ti.copy_assign = &wrapperForCopyAssign;
			ti.move_assign = &wrapperForMoveAssign;

			instance = &ti;
		}
		return instance;
	}
};


template <class Val, class Errors>
class Any {
	char storage[MaxSize<typename Insert<Errors, Val>::type>::value];
	const TypeInfo* ti;

	template <class OVal, class OErrors>
	friend OVal unsafe_cast(Any<OVal, OErrors>& v);

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

	void callCopyConstructor(void* to) {
		ti->copy_constr(storage, to);
	}

	void callMoveConstructor(void* to) {
		ti->move_constr(storage, to);
	}

	void callCopyAssign(void* to) {
		ti->copy_assign(storage, to);
	}

	void callMoveAssign(void* to) {
		ti->move_assign(storage, to);
	}

	void destructor() {
		ti->destr(storage);
		ti = nullptr;
	}
public:
	Any() : ti(nullptr) {}

	template <class OVal, class OErrors>
	Any(const Any<OVal, OErrors>& v) : ti(nullptr) {
		v.callCopyConstructor(storage);
		ti = v.ti;
	}

	template <class OVal, class OErrors>
	Any(Any<OVal, OErrors>&& v) : ti(nullptr) {
		v.callMoveConstructor(storage);
		ti = v.ti;
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

	template <class OVal, class OErrors>
	Any<Val, Errors>& operator=(const Any<OVal, OErrors>& v) {
		if(this == &v)
			return *this;

		if(ti == nullptr)
			v.callCopyConstructor(storage);
		else if(ti == v.ti)
			v.callCopyAssign(storage);
		else {
			destructor();
			v.callCopyConstructor(storage);
		}

		ti = v.ti;

		return *this;
	}

	template <class OVal, class OErrors>
	Any<Val, Errors>& operator=(Any<OVal, OErrors>&& v) noexcept {
		if(this == &v)
			return *this;

		if(ti == nullptr)
			v.callMoveConstructor(storage);
		else if(ti == v.ti)
			v.callMoveAssign(storage);
		else {
			destructor();
			v.callMoveConstructor(storage);
		}

		ti = v.ti;

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

template <class Val, class Errors>
Val unsafe_cast(Any<Val, Errors>& v) {
	return *static_cast<Val*>(v.storage);
}

#endif


} /* namespace detail */

} /* namespace error_handling */


#endif /* ANY_HPP_ */
