// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const Color& Sprite::GetColor() const
	{
		return m_color;
	}

	inline const Color& Sprite::GetCornerColor(RectCorner corner) const
	{
		return m_cornerColor[UnderlyingCast(corner)];
	}

	inline const Vector2f& Sprite::GetOrigin() const
	{
		return m_origin;
	}

	inline const Vector2f& Sprite::GetSize() const
	{
		return m_size;
	}

	inline const Rectf& Sprite::GetTextureCoords() const
	{
		return m_textureCoords;
	}

	inline void Sprite::SetColor(const Color& color)
	{
		m_color = color;

		UpdateVertices();
	}

	inline void Sprite::SetCornerColor(RectCorner corner, const Color& color)
	{
		m_cornerColor[UnderlyingCast(corner)] = color;

		UpdateVertices();
	}

	inline void Sprite::SetMaterial(std::shared_ptr<MaterialInstance> material)
	{
		assert(material);

		if (m_material != material)
		{
			OnMaterialInvalidated(this, 0, material);
			m_material = std::move(material);

			OnElementInvalidated(this);
		}
	}

	inline void Sprite::SetOrigin(const Vector2f& origin)
	{
		m_origin = origin;

		UpdateVertices();
	}

	inline void Sprite::SetSize(const Vector2f& size)
	{
		m_size = size;

		UpdateVertices();
	}

	inline void Sprite::SetTextureCoords(const Rectf& textureCoords)
	{
		m_textureCoords = textureCoords;

		UpdateVertices();
	}

	inline void Sprite::SetTextureRect(const Rectf& textureRect)
	{
		Vector2f invTextureSize = 1.f / Vector2f(Vector2ui(GetTextureSize()));
		return SetTextureCoords(Rectf(textureRect.x * invTextureSize.x, textureRect.y * invTextureSize.y, textureRect.width * invTextureSize.x, textureRect.height * invTextureSize.y));
	}

	inline void Sprite::UpdateVertices()
	{
		Boxf aabb(-1.f, -1.f, -1.f);

		VertexStruct_XYZ_Color_UV* vertices = m_vertices.data();

		std::array<Vector2f, RectCornerCount> cornerExtent;
		cornerExtent[UnderlyingCast(RectCorner::LeftBottom)] = Vector2f(0.f, 0.f);
		cornerExtent[UnderlyingCast(RectCorner::RightBottom)] = Vector2f(1.f, 0.f);
		cornerExtent[UnderlyingCast(RectCorner::LeftTop)] = Vector2f(0.f, 1.f);
		cornerExtent[UnderlyingCast(RectCorner::RightTop)] = Vector2f(1.f, 1.f);

		Vector3f originShift = m_origin * m_size;

		for (RectCorner corner : { RectCorner::LeftBottom, RectCorner::RightBottom, RectCorner::LeftTop, RectCorner::RightTop })
		{
			vertices->color = m_color * m_cornerColor[UnderlyingCast(corner)];
			vertices->position = Vector3f(m_size * cornerExtent[UnderlyingCast(corner)], 0.f) - originShift;
			vertices->uv = m_textureCoords.GetCorner(corner);

			if (aabb.IsValid())
				aabb.ExtendTo(vertices->position);
			else
				aabb.Set(vertices->position);

			vertices++;
		}

		UpdateAABB(aabb);
		OnElementInvalidated(this);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
