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
#include <vector>

struct ErrA {};
struct ErrB {};
struct ErrC {};
struct ErrUnk {};

struct Base {};
struct Derived : public Base {};

struct Ops {
	error_handling::R<std::string>
	operator()(ErrA&&) {
		std::cout << "ErrA&&" << std::endl;
		return std::string();
	}

	error_handling::R<std::string>
	operator()(ErrB&&) {
		std::cout << "ErrB&&" << std::endl;
		return std::string();
	}

	error_handling::R<std::string>
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

struct LastIter {};

struct EndSeq {};

template <class T>
class VectorIter {
	typename std::vector<T>::iterator f;
	typename std::vector<T>::iterator l;
	typename std::vector<T>::iterator endvec;
public:
	VectorIter(typename std::vector<T>::iterator f,
			typename std::vector<T>::iterator l,
			typename std::vector<T>::iterator endvec) :
				f(f), l(l), endvec(endvec) {}

	bool operator==(const LastIter&) const {
		return f == l;
	}

	bool operator!=(const LastIter&) const {
		return f != l;
	}

	error_handling::R<T, EndSeq> operator*() {
		if(f == endvec)
			return EndSeq();
		return *f;
	}

	VectorIter operator++(int) {
		VectorIter ret(f, l, endvec);
		if(f != l)
			++f;
		return ret;
	}

	VectorIter& operator++() {
		if(f != l)
			++f;
		return *this;
	}
};

template <class FIter, class LIter, class Init, class F>
Init fold_rec(FIter first, LIter last, Init&& init, F f) {
	using Ret = error_handling::detail::IsRet<Init>;
	using Val = typename Ret::val_type::type;

	using ORet = error_handling::detail::IsRet<decltype(*std::declval<FIter>())>;
	using OVal = typename ORet::val_type::type;

	struct H {
		static Init call(FIter first, LIter last, Init&& init, F f) {
			Init res = error_handling::detail::repack<Val>(std::move(init), [&](Val&& val) -> Init {
				if(first == last)
					return std::move(val);

				auto ret(*(first++));
				Init res = error_handling::detail::repack<Val>(std::move(ret),
						[&val, f](OVal&& oval) -> Init { return f(std::move(val), std::move(oval)); });

				return H::call(first, last, std::forward<Init>(res), f);
			});
			return res;
		}
	};
	return H::call(first, last, std::forward<Init>(init), f);
}

template <class FIter, class LIter, class Init, class F>
Init fold_iter(FIter first, LIter last, Init&& init, F f) {
	using Ret = error_handling::detail::IsRet<Init>;
	using Val = typename Ret::val_type::type;

	using FRet = decltype(*std::declval<FIter>());
	using ORet = error_handling::detail::IsRet<FRet>;
	using OVal = typename ORet::val_type::type;

	for(; first != last; ++first) {
		init = error_handling::detail::repack<Val>(std::move(init), [&](Val&& val) -> Init {
			FRet ret(*first);
			return error_handling::detail::repack<Val>(std::move(ret),
					[&val, f](OVal&& oval) -> Init { return f(std::move(val), std::move(oval)); });
		});
	}

	return std::forward<Init>(init);
}

template <class FIter, class LIter, class Init, class F>
Init fold_iter2(FIter first, LIter last, Init&& init, F f) {
	for(; first != last; ++first) {
		auto ret = *first;
		init = f(std::move(init), std::move(ret));
	}

	return init;
}

error_handling::R<error_handling::N, ErrA, ErrB, ErrC, ErrUnk>
old_err_codes_to_new(int err_code) {
	if(err_code == 1)
		return ErrA();
	else if(err_code == 2)
		return ErrB();
	else if(err_code == 3)
		return ErrC();
	return ErrUnk();
}

const error_handling::ErOp<ErrA, ErrB, ErrC, ErrUnk> ErrX{};

decltype(error_handling::R<int>() + ErrX) func() {
	return 10;
}

int main() {
	using error_handling::R;
	using error_handling::if_err;
	using error_handling::repack;
	using error_handling::T;
	using error_handling::N;
	using error_handling::V;

#if 0
	std::string str("hello");
	R<std::string> ret(std::move(str));

	ErrA erra;
	ErrB errb;

	R<std::string, ErrA> ret2(erra);
//	Ret<std::string, ErrA> ret3(errb); // ERR

	R<std::string, ErrA> ret4(std::move(erra));
//	Ret<std::string, ErrA> ret5(std::move(errb)); // ERR
	R<std::string, ErrA, ErrB> ret6{R<std::string, ErrA>(std::string())};
//	Ret<std::string, ErrA, ErrB> ret7{Ret<std::string, ErrA, ErrC>()}; // ERR

	ret6 = R<std::string, ErrB>(std::string());
//	ret6 = Ret<std::string, Set<ErrC>>(); // ERR
//	ret6 = Ret<std::string, ErrC>(); // ERR

//	Ret<N> ret8{Ret<std::string>()}; // ERR
	R<T> ret9{R<std::string>()};

	R<Base, ErrA> ret10{R<Derived, ErrA>(Derived())};
//	Ret<Derived, ErrA> ret11{Ret<Base, ErrA>()}; // ERR

	ret10 = R<Derived, ErrA>(Derived());
//	Ret<Derived, ErrA> ret12; ret12 = Ret<Base, ErrA>();  //ERR

	R<std::string, ErrA, ErrB> ret13{std::string("hello")};
	R<std::string, ErrA> ret14 = if_err<ErrB>(std::move(ret13), [](ErrB&&) { return; });
//	Ret<std::string, ErrA> ret15 = if_err<Set<ErrB>>(std::move(ret14), FSet([](){ return; })); // ERR
	R<std::string> ret16 = if_err<ErrA>(std::move(ret14), [](ErrA&&) { return; });

	std::cout << ret16.data() << std::endl;

	R<std::string, int> ret17{int(666)};
	R<std::string> ret18 = if_err<int>(std::move(ret17), [](int&& i) { std::cout << i << std::endl; return; });

	R<std::string, ErrA, ErrB> ret19{std::string("hello")}; // N14error_handling6detail3RetIISs4ErrA4ErrBEEE
	if_err<ErrB>(std::move(ret19), [](ErrB&&) -> R<std::string, ErrA> { return std::string(); }); // N14error_handling6detail3RetIISs4ErrAEEE

	R<std::string, ErrA, ErrB> ret20{ErrB()};
	if_err<ErrB>(std::move(ret20), [](ErrB&&) -> R<std::string, ErrA> { std::cout << "err_b" << std::endl; return std::string(); });

	{
		R<std::string, ErrA, ErrB, ErrC> ret21{ErrB()};
		if_err<ErrA, ErrB, ErrC>(std::move(ret21), Ops());
	}

	{
		R<std::string, ErrA, ErrB, ErrC> ret21{ErrB()};
		R<std::string> ret22 = if_err<ErrA, ErrB, ErrC>(std::move(ret21), Ops());
	}

	R<std::string, ErrA, ErrB, ErrC> ret22{ErrB()};
//	Ret<std::string, Set<ErrC>> ret23 = if_err<Set<ErrA>>(std::move(ret22), boost::fusion::make_list([](ErrA&&) { return; })); // ERR

	R<std::string, ErrA, ErrB, ErrC> ret24{ErrB()};
	if_err<ErrA, ErrB, ErrC>(std::move(ret24), [](ErrB&&) {return;},
			[](ErrC&&) {return;}, [](ErrA&&) {return;});

//	Ret<std::string, Set<ErrA, ErrB, ErrC>> ret25{ErrB()};
//	if_err<Set<ErrA, ErrB, ErrC>>(std::move(ret25), FSet([](ErrB&&) {return;},
//			[](ErrC&&) {return;})); // ERR

	std::string ret26{(std::string)R<std::string>{std::string("hello")}};

	{
		R<std::string, ErrA> ret27{std::string("hello")};
		R<std::string> ret28{(std::string)if_err<ErrA>(std::move(ret27), [](ErrA&&) -> R<std::string> { return std::string("bye"); })};
		R<std::string, ErrA> ret29{std::string("hello")};
		R<std::string> ret30{(std::string)if_err<ErrA>(std::move(ret29), [](ErrA&&) -> std::string { return "bye"; })};
		R<std::string, ErrA> ret31{std::string("hello")};
		std::string ret32{(std::string)if_err<ErrA>(std::move(ret31), [](ErrA&&) -> std::string { return "bye"; })};
		R<std::string, ErrA> ret33{std::string("hello")};
		std::string ret34{(std::string)if_err<ErrA>(std::move(ret33), [](ErrA&&) { return "bye"; })};
	}

    R<FromString, ErrA> ret32{ErrA()};
    FromString ret33{(FromString)if_err<ErrA>(std::move(ret32), [](ErrA&&) { return FromString(std::string("bye")); })};
//    FromString ret34{(FromString)if_err<Set<ErrA>>(std::move(ret32), boost::fusion::make_list([](ErrA&&) { return std::string("bye"); }))}; // ERR

    R<std::unique_ptr<int>, ErrA> ret35{std::unique_ptr<int>(new int)};

//    Ret<std::string, Set<>> ret36;
//    if_err<>(std::move(ret36), [](ErrA&&) { return; }); // ERR

#endif
    // тест на множественный вызов if_err для одного Ret<> и для одной ошибки.
#if 0
    Ret<std::string, Set<ErrA>> ret37{ErrA()};
    if_err<ErrA>(std::move(ret37), [](ErrA&&) { return; });
    if_err<ErrA>(std::move(ret37), [](ErrA&&) { return; }); // runtime ERR
    if_err<ErrA>(std::move(ret37), [](ErrA&&) { return; }); // runtime ERR
    if_err<ErrA>(std::move(ret37), [](ErrA&&) { return; }); // runtime ERR
#endif

#if 0
    {
    	Ret<std::string, Set<ErrA>> ret = Ret<std::string, Set<ErrA>>{}; // ERR
    }
#endif

#if 0
    {
    	Ret<std::string, Set<ErrA>> ret1{ErrA()};
    	Ret<std::string, Set<ErrB>> ret2 = if_err<ErrA>(std::move(ret1),
    			[](ErrA&&) { return Ret<std::string, Set<ErrB>>(ErrB()); });
    	Ret<std::string, Set<ErrA>> ret3{ErrA()};
    	Ret<std::string, Set<ErrB>> ret4 = if_err<ErrA>(std::move(ret3),
    			[](ErrA&&) { return ErrB(); });
    }
#endif

#if 0
    {
    	Ret<std::string, Set<ErrA>> ret1{ErrA()};
    	Ret<std::string, Set<ErrA>> ret2 = if_err<Set<std::string>>(std::move(ret1),
    			FSet([](std::string&&) { return; }));  // ERR
    }
#endif

#if 0
    {
//    	R<std::string> v1 = R<N>(); // ERR
//    	R<std::string, ErrA> v2 = R<N>(); // ERR
//    	R<N> v3 = R<std::string>(); // ERR
//    	R<N, ErrA> v4 = R<std::string>(); // ERR
//    	R<N, ErrA> v5 = R<N>(); // ERR
        R<std::string, ErrA> v6 = R<N, ErrA>(ErrA()); // OK
        R<N, ErrA, ErrB> v7 = R<N, ErrA>(ErrA()); // OK
        R<N> v8 = R<N>(); // OK
    }
#endif

#if 0
    {
       	Ret<std::string, Set<ErrA>> ret1{std::string()};
       	Ret<int, Set<ErrA>> ret2 = repack<int>(std::move(ret1), [](std::string&& s) {});
       	Ret<std::string, Set<ErrA>> ret3{std::string()};
       	Ret<int, Set<ErrA>> ret4 = repack<int>(std::move(ret3), [](std::string&& s) { return 13; });
       	Ret<std::string, Set<ErrA>> ret5{std::string()};
       	Ret<int, Set<ErrA>> ret6 = repack<int>(std::move(ret5), [](std::string&& s) { return Ret<int, Set<ErrA>>(ErrA()); });
       	Ret<std::string, Set<ErrA>> ret7{std::string()};
       	Ret<int, Set<ErrA>> ret8 = repack<int>(std::move(ret7), [](std::string&& s) { return ErrA(); });
       	Ret<std::string, Set<ErrA>> ret9{std::string()};
       	Ret<int, Set<ErrA, ErrC>> ret10 = repack<int>(std::move(ret9), [](std::string&& s) { return ErrC(); });
    }
#endif

#if 0
    {
    	std::vector<int> num{1, 2, 3, 13, 15};
    	for(size_t i = 0; i < 100000; ++i)
    		num.push_back(i);

//    	VectorIter<int> it(num.begin(), num.end() + 15, num.end());
    	VectorIter<int> it(num.begin(), num.end(), num.end());

//    	R<std::string, EndSeq> ret1 = fold_rec(it, LastIter(), R<std::string, EndSeq>(std::string("")),
//    			[](std::string&& str, int&& num) { return str + std::to_string(num); });
//    	R<V, EndSeq> res1 = repack<V>(std::move(ret1),
//    			[](std::string&& str) { std::cout << str.size() << std::endl; return; });
//    	if_err<EndSeq>(std::move(res1), [](EndSeq&&) { std::cout << "EndSeq" << std::endl; });

//    	Ret<std::string, Set<EndSeq>> ret2 = fold_iter(it, LastIter(), Ret<std::string, Set<EndSeq>>(std::string("")),
//    	    			[](std::string&& str, int&& num) { return str + std::to_string(num); });
//    	Ret<V, Set<EndSeq>> res2 = repack<V>(std::move(ret2),
//    			[](std::string&& str) { std::cout << str.size() << std::endl; return; });
//    	if_err<EndSeq>(std::move(res2), [](EndSeq&&) { std::cout << "EndSeq" << std::endl; });

    	std::string res3 = fold_iter2(num.begin(), num.end(), std::string(""),
    			[](std::string&& str, int&& num) { return str + std::to_string(num); });
    	std::cout << res3.size() << std::endl;
    }
#endif

#if 0
    {
    	std::vector<int> num{1, 2, 3, 13, 15};
    	for(size_t i = 0; i < 100000000; ++i)
    		num.push_back(i);

    	VectorIter<int> it(num.begin(), num.end(), num.end());

    	R<unsigned long, EndSeq> ret2 = fold_iter(it, LastIter(), R<unsigned long, EndSeq>(0UL),
    			[](unsigned long&& num1, unsigned long&& num2) { return num1 + num2; });
    	R<V, EndSeq> res2 = repack<V>(std::move(ret2),
    			[](unsigned long&& num) { std::cout << num << std::endl; return; });
    	if_err<EndSeq>(std::move(res2), [](EndSeq&&) { std::cout << "EndSeq" << std::endl; });

//    	unsigned long res3 = fold_iter2(num.begin(), num.end(), 0UL,
//    			[](unsigned long&& num1, unsigned long&& num2) { return num1 + num2; });
//    	std::cout << res3 << std::endl;
    }
#endif

#if 1
    {
    	R<int, ErrA, ErrB, ErrC, ErrUnk> ret = func();
    	auto z = ErrX;
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
