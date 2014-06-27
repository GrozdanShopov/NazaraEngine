// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Scene.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Graphics/SceneRoot.hpp>
#include <Nazara/Graphics/SkinningManager.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <functional>
#include <memory>
#include <set>
#include <vector>
#include <Nazara/Graphics/Debug.hpp>

struct NzSceneImpl
{
	NzSceneImpl(NzScene* scene) :
	root(scene)
	{
	}

	std::unique_ptr<NzAbstractBackground> background;
	std::unique_ptr<NzAbstractRenderTechnique> renderTechnique;
	std::vector<NzUpdatable*> updateList;
	std::vector<NzUpdatable*> visibleUpdateList;
	NzClock updateClock;
	NzColor ambientColor = NzColor(25,25,25);
	NzSceneRoot root;
	NzAbstractViewer* viewer = nullptr;
	bool backgroundEnabled = true;
	bool update = false;
	float frameTime;
	float updateTime;
	int renderTechniqueRanking;
	unsigned int updatePerSecond = 60;
};

NzScene::NzScene()
{
	m_impl = new NzSceneImpl(this);
}

NzScene::~NzScene()
{
	delete m_impl;
}

void NzScene::AddToVisibilityList(NzUpdatable* object)
{
	m_impl->visibleUpdateList.push_back(object);
}

void NzScene::Cull()
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_impl->viewer)
	{
		NazaraError("No viewer");
		return;
	}
	#endif

	NzAbstractRenderQueue* renderQueue = GetRenderTechnique()->GetRenderQueue();
	renderQueue->Clear(false);

	m_impl->visibleUpdateList.clear();

	// Frustum culling
	RecursiveFrustumCull(renderQueue, m_impl->viewer->GetFrustum(), &m_impl->root);

	///TODO: Occlusion culling

	///TODO: Light culling
}

void NzScene::Draw()
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_impl->viewer)
	{
		NazaraError("No viewer");
		return;
	}
	#endif

	m_impl->viewer->ApplyView();

	try
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException);
		m_impl->renderTechnique->Clear(this);
		m_impl->renderTechnique->Draw(this);
	}
	catch (const std::exception& e)
	{
		NzString oldName = m_impl->renderTechnique->GetName();

		if (m_impl->renderTechniqueRanking > 0)
		{
			m_impl->renderTechnique.reset(NzRenderTechniques::GetByRanking(m_impl->renderTechniqueRanking-1, &m_impl->renderTechniqueRanking));
			NazaraError("Render technique \"" + oldName + "\" failed, fallback to \"" + m_impl->renderTechnique->GetName() + '"');
		}
		else
		{
			NzErrorFlags errFlags(nzErrorFlag_ThrowException);
			NazaraError("Render technique \"" + oldName + "\" failed and no fallback is available");
		}

		return;
	}
}

void NzScene::EnableBackground(bool enable)
{
	m_impl->backgroundEnabled = enable;
}

NzColor NzScene::GetAmbientColor() const
{
	return m_impl->ambientColor;
}

NzAbstractBackground* NzScene::GetBackground() const
{
	if (!m_impl->background)
		m_impl->background.reset(new NzColorBackground);

	return m_impl->background.get();
}

NzAbstractRenderTechnique* NzScene::GetRenderTechnique() const
{
	if (!m_impl->renderTechnique)
		m_impl->renderTechnique.reset(NzRenderTechniques::GetByRanking(-1, &m_impl->renderTechniqueRanking));

	return m_impl->renderTechnique.get();
}

NzSceneNode& NzScene::GetRoot() const
{
	return m_impl->root;
}

NzAbstractViewer* NzScene::GetViewer() const
{
	return m_impl->viewer;
}

float NzScene::GetUpdateTime() const
{
	return m_impl->updateTime;
}

unsigned int NzScene::GetUpdatePerSecond() const
{
	return m_impl->updatePerSecond;
}

bool NzScene::IsBackgroundEnabled() const
{
	return m_impl->backgroundEnabled;
}

void NzScene::RegisterForUpdate(NzUpdatable* object)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!object)
	{
		NazaraError("Invalid object");
		return;
	}
	#endif

	m_impl->updateList.push_back(object);
}

void NzScene::SetAmbientColor(const NzColor& color)
{
	m_impl->ambientColor = color;
}

void NzScene::SetBackground(NzAbstractBackground* background)
{
	m_impl->background.reset(background);
}

void NzScene::SetRenderTechnique(NzAbstractRenderTechnique* renderTechnique)
{
	m_impl->renderTechnique.reset(renderTechnique);
}

void NzScene::SetViewer(NzAbstractViewer* viewer)
{
	m_impl->viewer = viewer;
}

void NzScene::SetViewer(NzAbstractViewer& viewer)
{
	SetViewer(&viewer);
}

void NzScene::SetUpdatePerSecond(unsigned int updatePerSecond)
{
	m_impl->updatePerSecond = updatePerSecond;
}

void NzScene::UnregisterForUpdate(NzUpdatable* object)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!object)
	{
		NazaraError("Invalid object");
		return;
	}
	#endif

	auto it = std::find(m_impl->updateList.begin(), m_impl->updateList.end(), object);
	if (it != m_impl->updateList.end())
		m_impl->updateList.erase(it);
}

void NzScene::Update()
{
	m_impl->update = (m_impl->updatePerSecond == 0 || m_impl->updateClock.GetMilliseconds() > 1000/m_impl->updatePerSecond);
	if (m_impl->update)
	{
		m_impl->updateTime = m_impl->updateClock.GetSeconds();
		m_impl->updateClock.Restart();

		for (NzUpdatable* updatable : m_impl->updateList)
			///TODO: Multihreading
			updatable->Update();
	}
}

void NzScene::UpdateVisible()
{
	NzSkinningManager::Skin();

	if (m_impl->update)
	{
		for (NzUpdatable* node : m_impl->visibleUpdateList)
			node->Update();
	}
}

NzScene::operator const NzSceneNode&() const
{
	return m_impl->root;
}

void NzScene::RecursiveFrustumCull(NzAbstractRenderQueue* renderQueue, const NzFrustumf& frustum, NzNode* node)
{
	for (NzNode* child : node->GetChilds())
	{
		if (child->GetNodeType() == nzNodeType_Scene)
		{
			NzSceneNode* sceneNode = static_cast<NzSceneNode*>(child);

			///TODO: Empêcher le rendu des enfants si le parent est cullé selon un flag
			sceneNode->UpdateVisibility(frustum);
			if (sceneNode->IsVisible())
				sceneNode->AddToRenderQueue(renderQueue);
		}

		if (child->HasChilds())
			RecursiveFrustumCull(renderQueue, frustum, child);
	}
}
