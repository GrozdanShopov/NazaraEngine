// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/World.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <algorithm>

namespace Ndk
{
	inline GraphicsComponent::GraphicsComponent() :
	m_reflectiveMaterialCount(0),
	m_scissorRect(-1, -1)
	{
	}

	/*!
	* \brief Constructs a GraphicsComponent object by copy semantic
	*
	* \param graphicsComponent GraphicsComponent to copy
	*/
	inline GraphicsComponent::GraphicsComponent(const GraphicsComponent& graphicsComponent) :
	Component(graphicsComponent),
	m_reflectiveMaterialCount(0),
	m_aabb(graphicsComponent.m_aabb),
	m_transformMatrix(graphicsComponent.m_transformMatrix),
	m_scissorRect(graphicsComponent.m_scissorRect),
	m_boundingVolumesUpdated(graphicsComponent.m_boundingVolumesUpdated),
	m_transformMatrixUpdated(graphicsComponent.m_transformMatrixUpdated)
	{
		m_renderables.reserve(graphicsComponent.m_renderables.size());
		for (const Renderable& r : graphicsComponent.m_renderables)
			Attach(r.renderable, r.data.localMatrix, r.data.renderOrder);
	}

	inline void GraphicsComponent::AddToCullingList(GraphicsComponentCullingList* cullingList) const
	{
		m_cullingBoxEntries.emplace_back();
		CullingBoxEntry& entry = m_cullingBoxEntries.back();
		entry.cullingListReleaseSlot.Connect(cullingList->OnCullingListRelease, this, &GraphicsComponent::RemoveFromCullingList);
		entry.listEntry = cullingList->RegisterBoxTest(this);

		InvalidateAABB();
	}

	/*!
	* \brief Attaches a renderable to the entity
	*
	* \param renderable Reference to a renderable element
	* \param renderOrder Render order of the element
	*/
	inline void GraphicsComponent::Attach(Nz::InstancedRenderableRef renderable, int renderOrder)
	{
		return Attach(std::move(renderable), Nz::Matrix4f::Identity(), renderOrder);
	}

	/*!
	* \brief Clears every renderable elements
	*/

	inline void GraphicsComponent::Clear()
	{
		m_materialEntries.clear();
		m_renderables.clear();

		if (m_reflectiveMaterialCount > 0)
		{
			m_reflectiveMaterialCount = 0;
			InvalidateReflectionMap();
		}

		InvalidateAABB();
	}

	/*!
	* \brief Detaches a renderable to the entity
	*
	* \param renderable Reference to a renderable element
	*/

	inline void GraphicsComponent::Detach(const Nz::InstancedRenderable* renderable)
	{
		for (auto it = m_renderables.begin(); it != m_renderables.end(); ++it)
		{
			if (it->renderable == renderable)
			{
				InvalidateAABB();

				std::size_t materialCount = renderable->GetMaterialCount();
				for (std::size_t i = 0; i < materialCount; ++i)
					UnregisterMaterial(renderable->GetMaterial(i));

				m_renderables.erase(it);

				ForceCullingInvalidation();
				break;
			}
		}
	}

	/*!
	* \brief Checks if this graphics component requires real-time reflections to be generated
	*
	* If any of the materials attached to a GraphicsComponent (via the attached instanced renderable) needs real-time reflections, this function will return true.
	*
	* \return True if real-time reflections needs to be generated or false
	*/
	inline bool GraphicsComponent::DoesRequireRealTimeReflections() const
	{
		return m_reflectiveMaterialCount != 0 && m_reflectionMap;
	}

	/*!
	* \brief Ensures the bounding volume is up to date
	*/

	inline void GraphicsComponent::EnsureBoundingVolumesUpdate() const
	{
		if (!m_boundingVolumesUpdated)
			UpdateBoundingVolumes();
	}

	/*!
	* \brief Ensures the transformation matrix is up to date
	*/

	inline void GraphicsComponent::EnsureTransformMatrixUpdate() const
	{
		if (!m_transformMatrixUpdated)
			UpdateTransformMatrix();
	}

	/*!
	* \brief Gets the axis-aligned bounding box of the entity
	* \return A constant reference to the AABB
	*/
	inline const Nz::Boxf& GraphicsComponent::GetAABB() const
	{
		EnsureBoundingVolumesUpdate();

		return m_aabb;
	}

	/*!
	* \brief Gets the set of renderable elements
	*
	* \param renderables Pointer to the list of renderables
	*
	* \remark Produces a NazaraAssert if renderables is invalid
	*/

	inline void GraphicsComponent::GetAttachedRenderables(RenderableList* renderables) const
	{
		NazaraAssert(renderables, "Invalid renderable list");

		renderables->reserve(renderables->size() + m_renderables.size());
		for (const Renderable& r : m_renderables)
			renderables->push_back(r.renderable);
	}

	/*!
	* \brief Gets the number of renderable elements attached to the entity
	* \return Number of renderable elements
	*/

	inline std::size_t GraphicsComponent::GetAttachedRenderableCount() const
	{
		return m_renderables.size();
	}

	inline const Nz::BoundingVolumef& GraphicsComponent::GetBoundingVolume(std::size_t renderableIndex) const
	{
		EnsureBoundingVolumesUpdate();

		assert(renderableIndex < m_renderables.size());
		return m_renderables[renderableIndex].boundingVolume;
	}

	inline const Nz::Matrix4f& GraphicsComponent::GetLocalMatrix(std::size_t renderableIndex) const
	{
		assert(renderableIndex < m_renderables.size());
		return m_renderables[renderableIndex].data.localMatrix;
	}

	inline const Nz::Matrix4f& GraphicsComponent::GetTransformMatrix(std::size_t renderableIndex) const
	{
		EnsureBoundingVolumesUpdate();

		assert(renderableIndex < m_renderables.size());
		return m_renderables[renderableIndex].data.transformMatrix;
	}

	/*!
	* \brief Calls a function for every renderable attached to this component
	*
	* \param func Callback function which will be called with renderable data
	*/
	template<typename Func>
	void GraphicsComponent::ForEachRenderable(const Func& func) const
	{
		for (const auto& renderableData : m_renderables)
			func(renderableData.renderable, renderableData.data.localMatrix, renderableData.data.renderOrder);
	}

	inline void GraphicsComponent::RemoveFromCullingList(GraphicsComponentCullingList* cullingList) const
	{
		for (auto it = m_cullingBoxEntries.begin(); it != m_cullingBoxEntries.end(); ++it)
		{
			if (it->listEntry.GetParent() == cullingList)
			{
				if (m_cullingBoxEntries.size() > 1)
					*it = std::move(m_cullingBoxEntries.back());

				m_cullingBoxEntries.pop_back();
				break;
			}
		}
	}

	inline void GraphicsComponent::SetScissorRect(const Nz::Recti& scissorRect)
	{
		m_scissorRect = scissorRect;

		for (CullingBoxEntry& entry : m_cullingBoxEntries)
			entry.listEntry.ForceInvalidation(); //< Invalidate render queues
	}

	inline void GraphicsComponent::UpdateLocalMatrix(const Nz::InstancedRenderable* instancedRenderable, const Nz::Matrix4f& localMatrix)
	{
		for (auto& renderable : m_renderables)
		{
			if (renderable.renderable == instancedRenderable)
			{
				renderable.data.localMatrix = localMatrix;

				InvalidateAABB();
				break;
			}
		}
	}

	inline void GraphicsComponent::UpdateRenderOrder(const Nz::InstancedRenderable* instancedRenderable, int renderOrder)
	{
		for (auto& renderable : m_renderables)
		{
			if (renderable.renderable == instancedRenderable)
			{
				renderable.data.renderOrder = renderOrder;
				break;
			}
		}
	}

	/*!
	* \brief Invalidates the bounding volume
	*/

	inline void GraphicsComponent::ForceCullingInvalidation()
	{
		for (CullingBoxEntry& entry : m_cullingBoxEntries)
			entry.listEntry.ForceInvalidation(); //< Invalidate render queues
	}

	inline void GraphicsComponent::InvalidateAABB() const
	{
		m_boundingVolumesUpdated = false;
	}

	/*!
	* \brief Invalidates every renderable elements
	*/

	inline void GraphicsComponent::InvalidateRenderables()
	{
		for (Renderable& r : m_renderables)
			r.dataUpdated = false;
	}

	/*!
	* \brief Invalidates the transformation matrix
	*/

	inline void GraphicsComponent::InvalidateTransformMatrix()
	{
		m_transformMatrixUpdated = false;

		InvalidateAABB();
		InvalidateRenderables();
	}
}
