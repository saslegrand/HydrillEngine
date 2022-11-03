#pragma once

#include <Refureku/Properties/PropertySettings.h>
#include <string>
#include <functional>

#include "EngineDll.hpp"
#include "Maths/Maths.hpp"

#include "Core/Logger.hpp"

#include "Generated/RFKProperties.rfkh.h"

/**
Field property when modifying a value in inspector. Specify a reflected method name in the class owner of the field.
The specified function should have 0 or 1 parameter (parameter need to have the same value type as the field)
*/
class HY_CLASS(rfk::PropertySettings(rfk::EEntityKind::Field, false, true)) ENGINE_API
    POnModify : public rfk::Property
{
    #pragma warning (disable:4251)
    std::string m_methodName = "";
    #pragma warning (default:4251)

public:
    POnModify(const char* methodName)
        : m_methodName(methodName) {}

    void Call(rfk::Class const& arch, void* object) const
    {
        if (rfk::Method const* method = arch.getMethodByName(m_methodName.c_str()))
        {
            // Call for no parameter function
            if (method->getParametersCount() == 0)
                method->invokeUnsafe(object);
        }
    }

    template <typename Type>
    void Call(rfk::Class const& arch, void* object, void* data) const
    {
        if (rfk::Method const* method = arch.getMethodByName(m_methodName.c_str()))
        {
            // Call for no parameter function
            if (method->getParametersCount() == 0)
            {
                method->invokeUnsafe(object);
                return;
            }

            // Call for one parameter function
            if (method->getParametersCount() == 1)
            {
                rfk::Archetype const* parameterArch = method->getParameterAt(0).getType().getArchetype();
                if (parameterArch->getId() != rfk::getType<Type>().getArchetype()->getId())
                    return;

                Type& castData = *static_cast<Type*>(data);
                method->invokeUnsafe(object, std::move(castData));

                return;
            }

            //Logger::Warning("POnModify - '" + m_methodName + "' has more than 1 parameter, call discard");
        }

        //Logger::Warning("POnModify - '" + m_methodName + "' not exists or is not reflected, call discard");
    }

    POnModify_GENERATED
};


class HY_CLASS(rfk::PropertySettings(rfk::EEntityKind::Field, false, true)) ENGINE_API
    PRange : public rfk::Property
{
    double minValue = 0.0;
    double maxValue = 0.0;

public:
    PRange(float min, float max) noexcept;
    PRange(float value) noexcept;

    double const& getMin() const { return minValue; }
    double const& getMax() const { return maxValue; }

	PRange_GENERATED
};


class HY_CLASS(rfk::PropertySettings(rfk::EEntityKind::Field, false, true)) ENGINE_API
    PReadOnly : public rfk::Property
{
    PReadOnly_GENERATED
};


class HY_CLASS(rfk::PropertySettings(rfk::EEntityKind::Field, false, true)) ENGINE_API
    PHide : public rfk::Property
{
    PHide_GENERATED
};


class HY_CLASS(rfk::PropertySettings(rfk::EEntityKind::Field, false, true)) ENGINE_API
    PHeader : public rfk::Property
{
#pragma warning (disable:4251)
    std::string m_title;
#pragma warning (default:4251)

public:
    PHeader(const char* title) noexcept;

    std::string GetTitle() const;

    PHeader_GENERATED
};

File_RFKProperties_GENERATED