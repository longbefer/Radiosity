#pragma once
#include "Box.h"
class Light :
    public Box
{
public:
    using Box::Box;

    virtual std::string GetName() const {
        return this->name + "(µÆ)";
    }
};

/**
* for test, change the shape of light
*/

#if _HAS_CXX20
template <class T>
concept INHERTGAMEOBJECT = std::is_base_of<GameObject, T>::value;
#else
#define INHERTGAMEOBJECT class
#endif

#if 0
template <INHERTGAMEOBJECT T>
class LightShape : public T {
public:
    using T::T;

    virtual std::string GetName() const {
        return this->name + "(µÆ)";
    }

    static_assert((std::is_base_of<GameObject, T>::value), "must inhert GameObject class");

};

struct is_kind_of_ {
    template <template <class> class TM, class T> static std::true_type check(TM<T>);
    template <template <class> class TM> static std::false_type check(...);
    //template <template <int> class TM, int N> static std::true_type check(TM<N>);
    //template <template <int> class TM> static std::false_type check(...);
};

#else
// reference: https://bbs.csdn.net/topics/80225646?page=2
// declarative way
template<typename T, typename Enable = void>
struct LightShape;

template<typename T>
struct LightShape<T, typename std::enable_if<std::is_base_of_v<GameObject, T>>::type>
    : public T
{
public:
    using T::T;

    virtual std::string GetName() const {
        return this->name + "(µÆ)";
    }
};

struct is_kind_of_ {
    template <template <class, class> class TM, class T, class Enable> static std::true_type check(TM<T, Enable>);
    template <template <class, class> class TM> static std::false_type check(...);
};
#endif

#define is_kind_of(TM, ...) decltype(is_kind_of_::check<TM>(std::declval<__VA_ARGS__>()))
#define is_kind_of_v(TM, ...) decltype(is_kind_of_::check<TM>(std::declval<decltype(__VA_ARGS__)>()))
#define is_kind_of_light(TYPE) is_kind_of(LightShape, TYPE)
#define is_kind_of_light_v(VAL) is_kind_of_v(LightShape, VAL)
