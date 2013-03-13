// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TUPLE_HPP
#define NAZARA_TUPLE_HPP

#include <tuple>

template<typename F, typename... ArgsT> void NzUnpackTuple(F func, const std::tuple<ArgsT...>& t);

#include <Nazara/Core/Tuple.inl>

#endif // NAZARA_TUPLE_HPP
