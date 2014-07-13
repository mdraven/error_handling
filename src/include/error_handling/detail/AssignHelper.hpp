/*
 * AssignHelper.hpp
 *
 *  Created on: Jul 13, 2014
 *      Author: mdraven
 */

#ifndef ASSIGNHELPER_HPP_
#define ASSIGNHELPER_HPP_

namespace error_handling {

namespace detail {

class AssignHelperSeal final {
	template <class OVal, class UnOp, class Val, class Errors>
	friend
	Ret<OVal, Errors> repack(Ret<Val, Errors>&& v, UnOp func);

	template <class>
	friend class IfErrsImpl;

	constexpr AssignHelperSeal() {}
	constexpr AssignHelperSeal(const AssignHelperSeal&) {}
	constexpr AssignHelperSeal(AssignHelperSeal&&) {}
};

struct AssignHelper {
	template <class Val, class OVal, class OErrors>
	static void assign(Ret<Val, Set<>>& v, Ret<OVal, OErrors>&& ov, const AssignHelperSeal) {
		AutoClearAny<OVal, OErrors> oany(unsafe_access_to_internal_data(ov));
		Val& val = unsafe_access_to_internal_data(v);

		val = std::move(unsafe_cast<OVal>(oany.data()));
	}

	template <class Val, class Errors, class OVal, class OErrors>
	static void assign(Ret<Val, Errors>& v, Ret<OVal, OErrors>&& ov, const AssignHelperSeal) {
		Any<Val, Errors>& any = unsafe_access_to_internal_data(v);
		AutoClearAny<OVal, OErrors> oany(unsafe_access_to_internal_data(ov));

		any = std::move(oany.data());
	}
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* ASSIGNHELPER_HPP_ */
