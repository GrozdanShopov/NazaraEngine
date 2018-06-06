// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTSPRITE_HPP
#define NAZARA_TEXTSPRITE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/VertexStruct.hpp>

namespace Nz
{
	class AbstractTextDrawer;
	class TextSprite;

	using TextSpriteConstRef = ObjectRef<const TextSprite>;
	using TextSpriteLibrary = ObjectLibrary<TextSprite>;
	using TextSpriteRef = ObjectRef<TextSprite>;

	class NAZARA_GRAPHICS_API TextSprite : public InstancedRenderable
	{
		public:
			inline TextSprite();
			inline TextSprite(const AbstractTextDrawer& drawer);
			inline TextSprite(const TextSprite& sprite);
			~TextSprite() = default;

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Recti& scissorRect) const override;

			inline void Clear();

			std::unique_ptr<InstancedRenderable> Clone() const override;

			inline const Color& GetColor() const;
			inline float GetScale() const;

			inline void SetColor(const Color& color);
			inline void SetDefaultMaterial();
			using InstancedRenderable::SetMaterial;
			inline void SetMaterial(MaterialRef material);
			inline void SetMaterial(std::size_t skinIndex, MaterialRef material);
			inline void SetScale(float scale);

			void Update(const AbstractTextDrawer& drawer);

			inline TextSprite& operator=(const TextSprite& text);

			template<typename... Args> static TextSpriteRef New(Args&&... args);

		private:
			inline void InvalidateVertices();
			void MakeBoundingVolume() const override;
			void OnAtlasInvalidated(const AbstractAtlas* atlas);
			void OnAtlasLayerChange(const AbstractAtlas* atlas, AbstractImage* oldLayer, AbstractImage* newLayer);
			void UpdateData(InstanceData* instanceData) const override;

			struct RenderIndices
			{
				unsigned int first;
				unsigned int count;
			};

			struct AtlasSlots
			{
				bool used;
				NazaraSlot(AbstractAtlas, OnAtlasCleared, clearSlot);
				NazaraSlot(AbstractAtlas, OnAtlasLayerChange, layerChangeSlot);
				NazaraSlot(AbstractAtlas, OnAtlasRelease, releaseSlot);
			};

			std::unordered_map<const AbstractAtlas*, AtlasSlots> m_atlases;
			mutable std::unordered_map<Texture*, RenderIndices> m_renderInfos;
			mutable std::vector<VertexStruct_XY_Color_UV> m_localVertices;
			Color m_color;
			Recti m_localBounds;
			float m_scale;

			static TextSpriteLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/TextSprite.inl>

#endif // NAZARA_TEXTSPRITE_HPP
