//
// Created by 12132 on 2022/8/6.
//

#ifndef COMMON_DEFER_H_
#define COMMON_DEFER_H_

template<typename T>
struct remove_reference {
    typedef T Type;
};

template<typename T>
struct remove_reference<T&> {
    typedef T Type;
};

template<typename T>
struct remove_reference<T&&> {
    typedef T Type;
};

template<typename T>
inline T&& forward(typename remove_reference<T>::Type& t) { return static_cast<T&&>(t); }

template<typename T>
inline T&& forward(typename remove_reference<T>::Type&& t) { return static_cast<T&&>(t); }

template<typename T>
inline T&& move(T&& t) { return static_cast<typename remove_reference<T>::Type&&>(t); }

template<typename F>
struct defer_wrapper {
    F f;

    defer_wrapper(F&& f) : f(forward<F>(f)) {}

    ~defer_wrapper() { f(); }
};

template<typename F>
defer_wrapper<F> __defer_func(F&& f) { return defer_wrapper<F>(forward<F>(f)); }

#define _DEFER_1(x, y) x##y
#define _DEFER_2(x, y) _DEFER_1(x, y)
#define _DEFER_3(x)    _DEFER_2(x, __COUNTER__)
#define defer(code)    auto _DEFER_3(_defer_) = __defer_func([&]()->void{code;})

#endif // COMMON_DEFER_H_
