/*
 * Copyright (C) 2014 Ramil Iljasov
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 */

#ifndef ASSIGNHELPER_HPP_
#define ASSIGNHELPER_HPP_

namespace error_handling {

namespace detail {

class AssignHelperSeal final {
	template <class>
	friend class RepacksImpl;

	template <class>
	friend class IfErrsImpl;

	constexpr AssignHelperSeal() {}
	constexpr AssignHelperSeal(const AssignHelperSeal&) {}
	constexpr AssignHelperSeal(AssignHelperSeal&&) {}
};

struct AssignHelper {
	template <class Val, class OVal, class OErrors>
	static void assign(Ret<Val, Set<>>& v, Ret<OVal, OErrors>&& ov, const AssignHelperSeal) {
		Any<OVal, OErrors>& oany = unsafe_access_to_internal_data(ov);
		Val& val = unsafe_access_to_internal_data(v);

		val = std::move(unsafe_cast<OVal>(oany));
	}

	template <class Val, class Errors, class OVal, class OErrors>
	static void assign(Ret<Val, Errors>& v, Ret<OVal, OErrors>&& ov, const AssignHelperSeal) {
		Any<Val, Errors>& any = unsafe_access_to_internal_data(v);
		Any<OVal, OErrors>& oany = unsafe_access_to_internal_data(ov);

		any = std::move(oany);
	}

	template <class T, class OVal, class OErrors>
	static T assign(Ret<OVal, OErrors>&& ov, const AssignHelperSeal) {
		Any<OVal, OErrors>& oany = unsafe_access_to_internal_data(ov);

		return unsafe_cast<T>(oany);
	}
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* ASSIGNHELPER_HPP_ */
