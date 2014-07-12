/*
 * test.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: mdraven
 */

#include <error_handling/error_handling.hpp>

#include <string>
#include <iostream>
#include <memory>

// TODO: delete
#include <boost/fusion/container.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/front.hpp>
// ^^^^

struct ErrA {};
struct ErrB {};
struct ErrC {};

struct Base {};
struct Derived : public Base {};

struct Ops {
	error_handling::Ret<std::string, error_handling::Set<>>
	operator()(ErrA&&) {
		std::cout << "ErrA&&" << std::endl;
		return std::string();
	}

	error_handling::Ret<std::string, error_handling::Set<>>
	operator()(ErrB&&) {
		std::cout << "ErrB&&" << std::endl;
		return std::string();
	}

	error_handling::Ret<std::string, error_handling::Set<>>
	operator()(ErrC&&) {
		std::cout << "ErrC&&" << std::endl;
		return std::string();
	}
};

struct FromString {
	FromString() = default;
	FromString(const FromString&) = default;
	FromString(FromString&&) = default;
	explicit FromString(const std::string&) {}

	FromString& operator=(FromString&&) = default;
};

//FromString func() {
//	return std::string("hello");
//}

int main() {
	using error_handling::Ret;
	using error_handling::if_err;
	using error_handling::T;
	using error_handling::N;
	using error_handling::V;
	using error_handling::Set;

	std::string str("hello");
	Ret<std::string, Set<>> ret(std::move(str));

	ErrA erra;
	ErrB errb;

	Ret<std::string, Set<ErrA>> ret2(erra);
//	Ret<std::string, ErrA> ret3(errb); // ERR

	Ret<std::string, Set<ErrA>> ret4(std::move(erra));
//	Ret<std::string, ErrA> ret5(std::move(errb)); // ERR
	Ret<std::string, Set<ErrA, ErrB>> ret6{Ret<std::string, Set<ErrA>>()};
//	Ret<std::string, ErrA, ErrB> ret7{Ret<std::string, ErrA, ErrC>()}; // ERR

	// присваивать нельзя
//	ret6 = Ret<std::string, Set<ErrB>>(); // ERR
//	ret6 = Ret<std::string, Set<ErrC>>(); // ERR
//	ret6 = Ret<std::string, ErrC>(); // ERR

//	Ret<N> ret8{Ret<std::string>()}; // ERR
	Ret<T, Set<>> ret9{Ret<std::string, Set<>>()};

	Ret<Base, Set<ErrA>> ret10{Ret<Derived, Set<ErrA>>()};
//	Ret<Derived, ErrA> ret11{Ret<Base, ErrA>()}; // ERR

//	ret10 = Ret<Derived, Set<ErrA>>(); // ERR
//	Ret<Derived, ErrA> ret12; ret12 = Ret<Base, ErrA>();  //ERR

	Ret<std::string, Set<ErrA, ErrB>> ret13{std::string("hello")};
	Ret<std::string, Set<ErrA>> ret14 = if_err<Set<ErrB>>(std::move(ret13), boost::fusion::make_list([](ErrB&&) { return; }));
//	Ret<std::string, ErrA> ret15 = if_err<Set<ErrB>>(std::move(ret14), boost::fusion::make_list([](){ return; })); // ERR
	Ret<std::string, Set<>> ret16 = if_err<Set<ErrA>>(std::move(ret14), boost::fusion::make_list([](ErrA&&) { return; }));

	std::cout << ret16.data() << std::endl;

	Ret<std::string, Set<int>> ret17{int(666)};
	Ret<std::string, Set<>> ret18 = if_err<Set<int>>(std::move(ret17), boost::fusion::make_list([](int&& i) { std::cout << i << std::endl; return; }));

	Ret<std::string, Set<ErrA, ErrB>> ret19{std::string("hello")}; // N14error_handling6detail3RetIISs4ErrA4ErrBEEE
	if_err<Set<ErrB>>(std::move(ret19), boost::fusion::make_list([](ErrB&&) -> Ret<std::string, Set<ErrA>> { return std::string(); })); // N14error_handling6detail3RetIISs4ErrAEEE

	Ret<std::string, Set<ErrA, ErrB>> ret20{ErrB()};
	if_err<Set<ErrB>>(std::move(ret20), boost::fusion::make_list([](ErrB&&) -> Ret<std::string, Set<ErrA>> { std::cout << "err_b" << std::endl; return std::string(); }));

	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret21{ErrB()};
	if_err<Set<ErrA, ErrB, ErrC>>(std::move(ret21), boost::fusion::make_list(Ops()));

	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret22{ErrB()};
//	Ret<std::string, Set<ErrC>> ret23 = if_err<Set<ErrA>>(std::move(ret22), boost::fusion::make_list([](ErrA&&) { return; })); // ERR

	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret24{ErrB()};
	if_err<Set<ErrA, ErrB, ErrC>>(std::move(ret24), boost::fusion::make_list([](ErrB&&) {return;},
			[](ErrC&&) {return;}, [](ErrA&&) {return;}));

//	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret25{ErrB()};
//	if_err<Set<ErrA, ErrB, ErrC>>(std::move(ret25), boost::fusion::make_list([](ErrB&&) {return;},
//			[](ErrC&&) {return;})); // ERR

	std::string ret26{(std::string)Ret<std::string, Set<>>{std::string("hello")}};

	{
		Ret<std::string, Set<ErrA>> ret27{std::string("hello")};
		Ret<std::string, Set<>> ret28{(std::string)if_err<Set<ErrA>>(std::move(ret27), boost::fusion::make_list([](ErrA&&) -> Ret<std::string, Set<>> { return std::string("bye"); }))};
		Ret<std::string, Set<ErrA>> ret29{std::string("hello")};
		Ret<std::string, Set<>> ret30{(std::string)if_err<Set<ErrA>>(std::move(ret29), boost::fusion::make_list([](ErrA&&) -> std::string { return "bye"; }))};
		Ret<std::string, Set<ErrA>> ret31{std::string("hello")};
		std::string ret32{(std::string)if_err<Set<ErrA>>(std::move(ret31), boost::fusion::make_list([](ErrA&&) -> std::string { return "bye"; }))};
		Ret<std::string, Set<ErrA>> ret33{std::string("hello")};
		std::string ret34{(std::string)if_err<Set<ErrA>>(std::move(ret33), boost::fusion::make_list([](ErrA&&) { return "bye"; }))};
	}

    Ret<FromString, Set<ErrA>> ret32{ErrA()};
    FromString ret33{(FromString)if_err<Set<ErrA>>(std::move(ret32), boost::fusion::make_list([](ErrA&&) { return FromString(std::string("bye")); }))};
//    FromString ret34{(FromString)if_err<Set<ErrA>>(std::move(ret32), boost::fusion::make_list([](ErrA&&) { return std::string("bye"); }))}; // ERR

    Ret<std::unique_ptr<int>, Set<ErrA>> ret35{std::unique_ptr<int>(new int)};

//    Ret<std::string, Set<>> ret36;
//    if_err<Set<>>(std::move(ret36), boost::fusion::make_list([](ErrA&&) { return; })); // ERR

    // тест на множественный вызов if_err для одного Ret<> и для одной ошибки.
#if 0
    Ret<std::string, Set<ErrA>> ret37{ErrA()};
    if_err<Set<ErrA>>(std::move(ret37), boost::fusion::make_list([](ErrA&&) { return; }));
    if_err<Set<ErrA>>(std::move(ret37), boost::fusion::make_list([](ErrA&&) { return; })); // ERR
    if_err<Set<ErrA>>(std::move(ret37), boost::fusion::make_list([](ErrA&&) { return; })); // ERR
    if_err<Set<ErrA>>(std::move(ret37), boost::fusion::make_list([](ErrA&&) { return; })); // ERR
#endif

#if 1
    {
    	Ret<std::string, Set<ErrA>> ret1{ErrA()};
    	Ret<std::string, Set<ErrB>> ret2 = if_err<Set<ErrA>>(std::move(ret1),
    			boost::fusion::make_list([](ErrA&&) { return Ret<std::string, Set<ErrB>>(ErrB()); }));
    }
#endif

//	std::cout << IsUnOp<ErrA, Ops>::value << std::endl;
//	std::cout << IsUnOp<ErrB, Ops>::value << std::endl;
//	std::cout << IsUnOp<ErrC, Ops>::value << std::endl;
//	std::cout << IsUnOp<Base, Ops>::value << std::endl;
//	typename error_handling::detail::UnOpArgSet<error_handling::detail::Set<ErrA, Derived, ErrB, ErrC, Base>, Ops>::type x = 10;

//	error_handling::detail::EnableIfNotUniversalRef<ErrA&>::value;  // OK
//	error_handling::detail::EnableIfNotUniversalRef<ErrA&&>::value;  // OK
//	typename error_handling::detail::EnableIfNotUniversalRef<ErrA&>::type xxx1;  // OK
//	typename error_handling::detail::EnableIfNotUniversalRef<ErrA&&>::type xxx2;  // OK
//	std::is_same<typename error_handling::detail::EnableIfNotUniversalRef<ErrA&>::type::type, void>::value; // compile-time ERR
//	std::is_same<typename error_handling::detail::EnableIfNotUniversalRef<ErrA&&>::type::type, void>::value; // OK

//	static_assert(boost::mpl::empty<boost::mpl::set<>>::value, "");  // true
//	static_assert(boost::mpl::empty<boost::mpl::set<int>>::value, "");  // false
//	static_assert(boost::mpl::empty<ErrA>::value, ""); // false
//	static_assert(boost::mpl::size<boost::mpl::set<>>::value == 0, ""); // true
//	static_assert(boost::mpl::size<ErrA>::value == 0, ""); //

//    error_handling::detail::Any<std::string, error_handling::detail::Set<int>> a1;
//    error_handling::detail::Any<std::string, error_handling::detail::Set<int>> a2{std::string("hello")};
//    std::cout << error_handling::detail::unsafe_cast<std::string>(a2) << std::endl;
}
