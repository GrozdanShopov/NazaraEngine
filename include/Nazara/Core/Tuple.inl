// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://stackoverflow.com/questions/687490/c0x-how-do-i-expand-a-tuple-into-variadic-template-function-arguments
// Merci � Ryan "FullMetal Alchemist" Lahfa
// Merci aussi � Freedom de siteduzero.com

#include <Nazara/Utility/Debug.hpp>

template<unsigned int N> struct NzTupleUnpack
{
	template <typename F, typename... ArgsT, typename... Args>
	void operator()(F func, const std::tuple<ArgsT...>& t,  Args&... args)
	{
		NzTupleUnpack<N-1>()(func, t, std::get<N-1>(t), args...);
	}
};

template<> struct NzTupleUnpack<0>
{
	template <typename F, typename... ArgsT, typename... Args>
	void operator()(F func, const std::tuple<ArgsT...>&, Args&... args)
	{
		func(args...);
	}
};

template<typename F, typename... ArgsT>
void NzUnpackTuple(F func, const std::tuple<ArgsT...>& t )
{
	NzTupleUnpack<sizeof...(ArgsT)>()(func, t);
}

#include <Nazara/Utility/DebugOff.hpp>
