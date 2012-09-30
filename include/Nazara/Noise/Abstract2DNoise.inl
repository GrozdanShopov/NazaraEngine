// Copyright (C) 2012 R�mi B�ges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

template <typename T>
T NzAbstract2DNoise<T>::GetBasicValue(T x, T y)
{
    return this->GetValue(x,y,this->m_resolution);
}

template <typename T>
T NzAbstract2DNoise<T>::GetMappedValue(T x, T y)
{
    return (this->GetValue(x,y,this->m_resolution) + this->m_offset) * this->m_gain;
}

#include <Nazara/Core/DebugOff.hpp>
