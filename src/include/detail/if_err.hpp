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

template <class Err, class UnOp, class Val, class... Errors>
typename BuildRet<Ret, Val, typename Remove<Set<Errors...>, Err>::type>::type
if_err(Ret<Val, Errors...>&& v, UnOp op) {
//    if(unsafe_access_to_internal_data(v).type() == typeid(Err)) {
        // если func возвращает Ret<...>, то: return func(std::move(err.v));
        // если func возвращает что-то другое, то ошибка компиляции(зарезервирую bool для себя ^_^)
        /* если func возвращает void: func(std::move(err.v)); return Ret<Val>();
           (такое поведения для типов Val != Void нужно потому, что из лямбды могут кидать
           исключение или делать exit(). И если разрешить не писать типы только для Val == Void,
           то тип придётся писать и в случаях когда мы не выходим из лямбды нормально, а это неудобно) */
//    }

//    typename h::BuildRet<Ret, Val, typename Remove<set<Errors...>, Err>::type>::type ret;
//    unsafe_access_to_internal_data(ret) = std::move(unsafe_access_to_internal_data(v));
//    return ret;
}

} /* namespace detail */

} /* namespace error_handling */

#endif /* IF_ERR_HPP_ */
