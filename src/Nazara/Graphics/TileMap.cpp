// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TileMap.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::TileMap
	* \brief Graphics class that represent several tiles of the same size assembled into a grid
	*  This class is far more efficient than using a sprite for every tile
	*/

	/*!
	* \brief Adds the TileMap to the rendering queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data for the instance
	*/
	void TileMap::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Recti& scissorRect) const
	{
		const VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const VertexStruct_XYZ_Color_UV*>(instanceData.data.data());

		std::size_t spriteCount = 0;
		for (std::size_t layerIndex = 0; layerIndex < m_layers.size(); ++layerIndex)
		{
			const auto& layer = m_layers[layerIndex];
			if (layer.tiles.empty())
				continue;

			renderQueue->AddSprites(instanceData.renderOrder, GetMaterial(layerIndex), &vertices[4 * spriteCount], layer.tiles.size(), scissorRect);

			spriteCount += layer.tiles.size();
		}
	}

	/*!
	* \brief Clones this tilemap
	*/
	std::unique_ptr<InstancedRenderable> TileMap::Clone() const
	{
		return std::make_unique<TileMap>(*this);
	}

	void TileMap::MakeBoundingVolume() const
	{
		Nz::Vector2f size = GetSize();
		m_boundingVolume.Set(Vector3f(0.f), size.x*Vector3f::Right() + size.y*Vector3f::Down());
	}

	void TileMap::UpdateData(InstanceData* instanceData) const
	{
		std::size_t spriteCount = 0;
		for (const Layer& layer : m_layers)
			spriteCount += layer.tiles.size();

		instanceData->data.resize(4 * spriteCount * sizeof(VertexStruct_XYZ_Color_UV));
		VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<VertexStruct_XYZ_Color_UV*>(instanceData->data.data());

		spriteCount = 0;
		for (const Layer& layer : m_layers)
		{
			SparsePtr<Color> colorPtr(&vertices[4 * spriteCount].color, sizeof(VertexStruct_XYZ_Color_UV));
			SparsePtr<Vector3f> posPtr(&vertices[4 * spriteCount].position, sizeof(VertexStruct_XYZ_Color_UV));
			SparsePtr<Vector2f> texCoordPtr(&vertices[4 * spriteCount].uv, sizeof(VertexStruct_XYZ_Color_UV));

			for (std::size_t tileIndex : layer.tiles)
			{
				const Tile& tile = m_tiles[tileIndex];
				NazaraAssert(tile.enabled, "Tile specified for rendering is not enabled");

				std::size_t x = tileIndex % m_mapSize.x;
				std::size_t y = tileIndex / m_mapSize.x;

				Vector3f tileLeftCorner;
				if (m_isometricModeEnabled)
					tileLeftCorner.Set(x * m_tileSize.x + m_tileSize.x/2.f * (y % 2), y/2.f * -m_tileSize.y, 0.f);
				else
					tileLeftCorner.Set(x * m_tileSize.x, y * -m_tileSize.y, 0.f);

				*colorPtr++ = tile.color;
				*posPtr++ = instanceData->transformMatrix.Transform(tileLeftCorner);
				*texCoordPtr++ = tile.textureCoords.GetCorner(RectCorner_LeftTop);

				*colorPtr++ = tile.color;
				*posPtr++ = instanceData->transformMatrix.Transform(tileLeftCorner + m_tileSize.x * Vector3f::Right());
				*texCoordPtr++ = tile.textureCoords.GetCorner(RectCorner_RightTop);

				*colorPtr++ = tile.color;
				*posPtr++ = instanceData->transformMatrix.Transform(tileLeftCorner + m_tileSize.y * Vector3f::Down());
				*texCoordPtr++ = tile.textureCoords.GetCorner(RectCorner_LeftBottom);

				*colorPtr++ = tile.color;
				*posPtr++ = instanceData->transformMatrix.Transform(tileLeftCorner + m_tileSize.x * Vector3f::Right() + m_tileSize.y * Vector3f::Down());
				*texCoordPtr++ = tile.textureCoords.GetCorner(RectCorner_RightBottom);
			}
			spriteCount += layer.tiles.size();
		}
	}

	/*!
	* \brief Initializes the tilemap library
	* \return true If successful
	*
	* \remark Produces a NazaraError if the tilemap library failed to be initialized
	*/

	bool TileMap::Initialize()
	{
		if (!TileMapLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the tilemap library
	*/

	void TileMap::Uninitialize()
	{
		TileMapLibrary::Uninitialize();
	}

	TileMapLibrary::LibraryMap TileMap::s_library;
}
