// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP
#define NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP

#include <NDK/Component.hpp>
#include <Nazara/Graphics/Renderable.hpp>

namespace Ndk
{
	class NDK_API GraphicsComponent : public Component<GraphicsComponent>
	{
		public:
			GraphicsComponent() = default;
			~GraphicsComponent() = default;

			void AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const;

			static ComponentIndex componentIndex;

		private:
			struct Renderable
			{
				NzBoundingVolumef volume;
				NzRenderableRef renderable;
			};

			std::vector<Renderable> m_renderables;
	};
}

#include <NDK/Components/GraphicsComponent.inl>

#endif // NDK_COMPONENTS_GRAPHICSCOMPONENT_HPP
