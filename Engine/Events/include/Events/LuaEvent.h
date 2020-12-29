#pragma once
#include <string>
#include <functional>

enum class LuaEventType
{
    None = 0,
    LuaSetGlobalValue
};

//enum LuaEventCategory
//{
//    None = 0,
//    EventCategoryApplication = 1,
//    EventCategoryApplication2 = 2,
//    EventCategoryApplication3 = 4
//};

class LuaEvent
{
    //bool handled = false;

    //virtual LuaEventType GetEventType() const = 0;
    //virtual const char* GetName() const = 0;
    ////virtual int GetCategoryFlags() const = 0;
    //virtual std::string ToString() const { return GetName(); }

    ////bool IsInCategory(LuaEventCategory category)
    ////{
    ////    return GetCategoryFlags() & category;
    ////}

public:
    LuaEventType type = LuaEventType::None;
    LuaEvent(LuaEventType type) : type(type) {}
};