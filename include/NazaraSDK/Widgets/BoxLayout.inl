// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Widgets/BoxLayout.hpp>

namespace Ndk
{
	BoxLayout::BoxLayout(BaseWidget* parent, BoxLayoutOrientation orientation) :
	BaseWidget(parent),
	m_orientation(orientation),
	m_spacing(5.f)
	{
	}
}
