// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

NzNoiseBase::NzNoiseBase(unsigned int seed)
{
    Ua = 16807;
    Uc = 0;
    Um = 2147483647;
    UcurrentSeed = 0;
    Uprevious = 0;

    SetNewSeed(seed);

    for(int i(0) ; i < 512 ; i++)
        perm[i] = i & 255;

}

void NzNoiseBase::SetNewSeed(unsigned int seed)
{
    Uprevious = seed;
    UcurrentSeed = seed;
}

unsigned int NzNoiseBase::GetUniformRandomValue()
{
    Ulast = Ua*Uprevious + Uc%Um;
    Uprevious = Ulast;
    return Ulast;
}

void NzNoiseBase::ShufflePermutationTable()
{
    int xchanger;
    unsigned int ncase;

    for(unsigned int i(0) ; i < 256 ; i++)
        perm[i] = i;

    for(unsigned int j(0) ; j < 20 ; ++j)
        for (unsigned int i(0); i < 256 ; ++i)
        {
            ncase = this->GetUniformRandomValue() & 255;
            xchanger = perm[i];
            perm[i] = perm[ncase];
            perm[ncase] = xchanger;
        }

    for(unsigned int i(256) ; i < 512; ++i)
        perm[i] = perm[i & 255];
}

int NzNoiseBase::fastfloor(float n)
{
    return (n >= 0) ? static_cast<int>(n) : static_cast<int>(n-1);
}

int NzNoiseBase::JenkinsHash(int a, int b, int c)
{
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 13);
    b = b-c;  b = b - a;  b = b^(a << 8);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 13);
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 12);
    b = b-c;  b = b - a;  b = b^(a << 16);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 5);
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 3);
    b = b-c;  b = b - a;  b = b^(a << 10);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 15);
    return c;
}
