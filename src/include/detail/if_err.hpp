/*
 * if_err.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: mdraven
 */

#ifndef IF_ERR_HPP_
#define IF_ERR_HPP_

#include "Set.hpp"
#include "BuildRet.hpp"

namespace error_handling {

namespace detail {

template <class Err, class Val, class... Errors>
class RetTypeFor_IfErrValErrors {
	using WithoutErr = typename Remove<Set<Errors...>, Err>::type;
public:
	using type = typename BuildRet<Ret, Val, WithoutErr>::type;
};

template <class Err, class UnOp, class Val, class... Errors>
class ConstraintsFor_IfErrValErrors {
	static_assert(IsContains<Set<Errors...>, Err>::value, "`Err` isn't contains in `Errors...`");
};

class AssignHelper {
	template <class Err, class UnOp, class Val, class... Errors>
	friend
	typename RetTypeFor_IfErrValErrors<Err, Val, Errors...>::type
	if_err(Ret<Val, Errors...>&& v, UnOp op);

	template <class Val, class OVal, class... OErrors>
	static void assign(Ret<Val>& v, Ret<OVal, OErrors...>&& ov) {
		unsafe_access_to_internal_data(v) = std::move(boost::any_cast<Val>(unsafe_access_to_internal_data(ov)));
	}

	template <class Val, class Err, class... Errors, class OVal, class... OErrors>
	static void assign(Ret<Val, Err, Errors...>& v, Ret<OVal, OErrors...>&& ov) {
		unsafe_access_to_internal_data(v) = std::move(unsafe_access_to_internal_data(ov));
	}
};

template <class Err, class UnOp, class Val, class... Errors>
typename RetTypeFor_IfErrValErrors<Err, Val, Errors...>::type
if_err(Ret<Val, Errors...>&& v, UnOp op) {
	ConstraintsFor_IfErrValErrors<Err, UnOp, Val, Errors...>();

//    if(unsafe_access_to_internal_data(v).type() == typeid(Err)) {
        // если func возвращает Ret<...>, то: return func(std::move(err.v));
        // если func возвращает что-то другое, то ошибка компиляции(зарезервирую bool для себя ^_^)
        /* если func возвращает void: func(std::move(err.v)); return Ret<Val>();
           (такое поведения для типов Val != Void нужно потому, что из лямбды могут кидать
           исключение или делать exit(). И если разрешить не писать типы только для Val == Void,
           то тип придётся писать и в случаях когда мы не выходим из лямбды нормально, а это неудобно) */
//    }

	typename RetTypeFor_IfErrValErrors<Err, Val, Errors...>::type ret;
	AssignHelper::assign(ret, std::move(v));
    return ret;
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* IF_ERR_HPP_ */
