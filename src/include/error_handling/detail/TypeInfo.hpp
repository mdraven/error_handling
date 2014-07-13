/*
 * TypeInfo.hpp
 *
 *  Created on: Jul 13, 2014
 *      Author: mdraven
 */

#ifndef TYPEINFO_HPP_
#define TYPEINFO_HPP_

#include <typeinfo>
#include <cassert>

namespace error_handling {

namespace detail {

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
	bool operator==(const TypeInfo& ti) const noexcept {
		assert(this->ti != nullptr);
		return *this->ti == *ti.ti;
	}

	bool operator!=(const TypeInfo& ti) const noexcept {
		return !(*this == ti);
	}
};

template <class Type>
class TypeInfoHolder {
	static void wrapperForDestructor(void* data) {
		static_cast<Type*>(data)->~Type();
	}

	template <class OType>
	static void wrapperForCopyConstr(const void* from, void* to) {
		new(to) OType(*static_cast<const OType*>(from));
	}

	template <class OType>
	static void wrapperForMoveConstr(void* from, void* to) {
		new(to) OType(std::move(*static_cast<OType*>(from)));
	}

	template <class OType>
	static void wrapperForCopyAssign(const void* from, void* to) {
		*static_cast<OType*>(to) = *static_cast<const OType*>(from);
	}

	template <class OType>
	static void wrapperForMoveAssign(void* from, void* to) {
		*static_cast<OType*>(to) = std::move(*static_cast<OType*>(from));
	}

	struct Helper {
		Helper(const Helper&) = default;
		Helper(Helper&&) = default;
		Helper& operator=(const Helper&) = default;
		Helper& operator=(Helper&&) = default;
	};
public:
	static const TypeInfo* getTypeInfo() {
		static const TypeInfo* instance;

		if(instance == nullptr) {
			// TODO: need race protect
			static TypeInfo ti;

			ti.ti = &typeid(Type);

			ti.destr = &wrapperForDestructor;

			static const bool is_copy_constructible = std::is_copy_constructible<Type>::value;
			using copy_constructible_type = typename std::conditional<is_copy_constructible, Type, Helper>::type;
			ti.copy_constr = is_copy_constructible ? &wrapperForCopyConstr<copy_constructible_type> : nullptr;

			static const bool is_move_constructible = std::is_move_constructible<Type>::value;
			using move_constructible_type = typename std::conditional<is_move_constructible, Type, Helper>::type;
			ti.move_constr = is_move_constructible ? &wrapperForMoveConstr<move_constructible_type> : nullptr;

			static const bool is_copy_assignable = std::is_copy_assignable<Type>::value;
			using copy_assignable_type = typename std::conditional<is_copy_assignable, Type, Helper>::type;
			ti.copy_assign = is_copy_assignable ? &wrapperForCopyAssign<copy_assignable_type> : nullptr;

			static const bool is_move_assignable = std::is_move_assignable<Type>::value;
			using move_assignable_type = typename std::conditional<is_move_assignable, Type, Helper>::type;
			ti.move_assign = is_move_assignable ? &wrapperForMoveAssign<move_assignable_type> : nullptr;

			instance = &ti;
		}

		return instance;
	}
};

} /* namespace detail */

} /* namespace error_handling */


#endif /* TYPEINFO_HPP_ */
