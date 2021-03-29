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