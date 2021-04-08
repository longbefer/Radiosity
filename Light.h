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

template <INHERTGAMEOBJECT T>
class LightShape : public T {
    using T::T;

    virtual std::string GetName() const {
        return this->name + "(µÆ)";
    }
};

struct is_kind_of_light_ {
    template <template <class> class TM, class T> static std::true_type check(TM<T>);
    template <template <class> class TM> static std::false_type check(...);
    template <template <int> class TM, int N> static std::true_type check(TM<N>);
    template <template <int> class TM> static std::false_type check(...);
};
#define is_kind_of_light(TM, ...) decltype(is_kind_of_light_::check<TM>(std::declval<__VA_ARGS__>()))
