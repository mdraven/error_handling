/*
 * RetValErrors.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef RETVALERRORS_HPP_
#define RETVALERRORS_HPP_

#include <error_handling/detail/Ret/Ret.hpp>
#include <error_handling/detail/Set/Set.hpp>
#include <error_handling/detail/EnableIfNotUniversalRef.hpp>
#include <error_handling/detail/unsafe_access_to_internal_data.hpp>
#include <error_handling/detail/Any.hpp>
#include <error_handling/detail/config.hpp>

namespace error_handling {

namespace detail {

template <class Val, class Errors>
class Ret final {
	static_assert(IsSet<Errors>::value, "Second template argument must be `Set` type.");

	Any<Val, Errors> any;

	template <class OVal, class OErrors>
	friend Any<OVal, OErrors>& unsafe_access_to_internal_data(Ret<OVal, OErrors>&) noexcept;

	template <class OVal, class OErrors>
	friend const Any<OVal, OErrors>& unsafe_access_to_internal_data(const Ret<OVal, OErrors>& v) noexcept;

public:
	Ret() noexcept(noexcept(Any<Val, Errors>())) :
		any() {}

	Ret(const Val& v) noexcept(noexcept(Any<Val, Errors>(v))) :
		any(v) {}

	Ret(Val&& v) noexcept(noexcept(Any<Val, Errors>(std::move(v)))) :
		any(std::move(v)) {}

	template <class OErr>
	Ret(const OErr& v) noexcept(noexcept(Any<Val, Errors>(v))) :
		any(v) {
		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "copy constructor Err");
	}

	template <class OErr,
	class = typename EnableIfNotUniversalRef<OErr>::type::type>
	Ret(OErr&& v) noexcept(noexcept(Any<Val, Errors>(std::move(v)))) :
		any(std::move(v)) {
		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "move constructor Err");
	}

	template <class OVal, class OErrors>
	Ret(const Ret<OVal, OErrors>& v) noexcept(noexcept(Any<Val, Errors>(unsafe_access_to_internal_data(v)))) :
		any(unsafe_access_to_internal_data(v)) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
		static_assert(is_more_weak, "Assign to more strong type.");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "copy constructor Ret");

#ifdef ERROR_HANDLING_CHECK_EMPTY_RET
		if(unsafe_access_to_internal_data(v).empty()) {
			ERROR_HANDLING_CRITICAL_ERROR("Copying an empty `Ret`.");
		}
#endif
	}

	template <class OVal>
	Ret(const Ret<OVal, Set<>>& v) noexcept(noexcept(Any<Val, Errors>(unsafe_access_to_internal_data(v)))) :
		any(unsafe_access_to_internal_data(v)) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "copy constructor Ret");
	}

	template <class OVal, class OErrors>
	Ret(Ret<OVal, OErrors>&& v) noexcept(noexcept(Any<Val, Errors>(std::move(unsafe_access_to_internal_data(v))))) :
			any(std::move(unsafe_access_to_internal_data(v))) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
		static_assert(is_more_weak, "Assign to more strong type.");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "move constructor Ret");

#ifdef ERROR_HANDLING_CHECK_EMPTY_RET
		if(unsafe_access_to_internal_data(v).empty()) {
			ERROR_HANDLING_CRITICAL_ERROR("Moving an empty `Ret`.");
		}
#endif

		unsafe_access_to_internal_data(v).clear();
	}

	template <class OVal>
	Ret(Ret<OVal, Set<>>&& v) noexcept(noexcept(Any<Val, Errors>(std::move(unsafe_access_to_internal_data(v))))) :
			any(std::move(unsafe_access_to_internal_data(v))) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "move constructor Ret");
	}

	Ret<Val, Errors>& operator=(const Val& v) noexcept(noexcept(any = v)) {
		this->any = v;
		return *this;
	}

	Ret<Val, Errors>& operator=(Val&& v) noexcept(noexcept(any = std::move(v))) {
		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "move assign Val");

		this->any = std::move(v);
		return *this;
	}

	template <class OErr>
	Ret<Val, Errors>& operator=(const OErr& v) noexcept(noexcept(any = v)) {
		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");

		this->any = v;
		return *this;
	}

	template <class OErr,
	class = typename EnableIfNotUniversalRef<OErr>::type::type>
	Ret<Val, Errors>& operator=(OErr&& v) noexcept(noexcept(any = std::move(v))) {
		static const bool is_known_error = IsContains<Errors, OErr>::value;
		static_assert(is_known_error, "Unknown error type");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "move assign Err");

		this->any = std::move(v);
		return *this;
	}

	template <class OVal, class OErrors>
	Ret<Val, Errors>& operator=(const Ret<OVal, OErrors>& v) noexcept(noexcept(any = unsafe_access_to_internal_data(v))) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
		static_assert(is_more_weak, "Assign to more strong type.");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "copy assign Ret");

		if(unsafe_access_to_internal_data(v).empty()) {
			ERROR_HANDLING_CRITICAL_ERROR("Copying an empty `Ret`.");
		}

		this->any = unsafe_access_to_internal_data(v);

		return *this;
	}

	template <class OVal>
	Ret<Val, Errors>& operator=(const Ret<OVal, Set<>>& v) noexcept(noexcept(any = unsafe_access_to_internal_data(v))) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "copy assign Ret");

		this->any = unsafe_access_to_internal_data(v);

		return *this;
	}

	template <class OVal, class OErrors>
	Ret<Val, Errors>& operator=(Ret<OVal, OErrors>&& v) noexcept(noexcept(any = std::move(unsafe_access_to_internal_data(v)))) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		static const bool is_more_weak = IsDifferenceEmpty<OErrors, Errors>::value;
		static_assert(is_more_weak, "Assign to more strong type.");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "move assign Ret");

		if(unsafe_access_to_internal_data(v).empty()) {
			ERROR_HANDLING_CRITICAL_ERROR("Moving an empty `Ret`.");
		}

		this->any = std::move(unsafe_access_to_internal_data(v));

		unsafe_access_to_internal_data(v).clear();

		return *this;
	}

	template <class OVal>
	Ret<Val, Errors>& operator=(Ret<OVal, Set<>>&& v) noexcept(noexcept(any = std::move(unsafe_access_to_internal_data(v)))) {
		static const bool is_convertible_val = std::is_convertible<OVal, Val>::value;
		static_assert(is_convertible_val, "Cannot convert `Val` type.");

		ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "move assign Ret");

		this->any = std::move(unsafe_access_to_internal_data(v));

		return *this;
	}

	/* операторов приведения типа(например к Val или ErrN) -- нет: если тип в any не совпал, то
      мы можем только бросить исключение, но эта библиотека не кидает >своих< исключений(возможно только в
      деструкторе этого класса) */

	/* набор операторов сравнения отсутствует, так как всё что они могут при
      any != Val -- кинуть исключение, а кидать исключения нельзя(в этом суть идеи) */

	~Ret() {
#ifdef ERROR_HANDLING_CHECK_EMPTY_RET
		if(!any.empty()) {
			ERROR_HANDLING_CRITICAL_ERROR("Unchecked Ret.");
//			ERROR_HANDLING_DEBUG_MSG((Ret<Val, Errors>), "Unchecked Ret");
		}
#endif
	}
};

} /* namespace detail */

} /* namespace error_handling */

#endif /* RETVALERRORS_HPP_ */
