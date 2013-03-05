// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/Light.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <cstring>
#include <Nazara/3D/Debug.hpp>

///TODO: Utilisation des UBOs

NzLight::NzLight(nzLightType type) :
m_type(type),
m_ambientColor((type == nzLightType_Directional) ? NzColor(50, 50, 50) : NzColor::Black),
m_diffuseColor(NzColor::White),
m_specularColor(NzColor::White),
m_boundingBoxUpdated(false),
m_attenuation(0.9f),
m_innerAngle(15.f),
m_outerAngle(45.f),
m_radius(500.f)
{
}

NzLight::NzLight(const NzLight& light) :
NzSceneNode(light)
{
	std::memcpy(this, &light, sizeof(NzLight)); // Aussi simple que ça
}

NzLight::~NzLight()
{
}

void NzLight::AddToRenderQueue(NzRenderQueue& renderQueue) const
{
	switch (m_type)
	{
		case nzLightType_Directional:
			renderQueue.directionnalLights.push_back(this);
			break;

		case nzLightType_Point:
		case nzLightType_Spot:
			renderQueue.visibleLights.push_back(this);
			break;
	}
}

void NzLight::Apply(const NzShader* shader, unsigned int lightUnit) const
{
	/*
	struct Light
	{
		int type;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;

		vec4 parameters1;
		vec4 parameters2;
		vec2 parameters3;
	};

	Directional
	-P1: vec3 direction

	Point
	-P1: vec3 position + float attenuation
	-P2: float invRadius

	Spot
	-P1: vec3 position + float attenuation
	-P2: vec3 direction + float invRadius
	-P3: float cosInnerAngle + float cosOuterAngle
	*/

	int typeLocation = shader->GetUniformLocation("Lights[0].type");
	int ambientLocation = shader->GetUniformLocation("Lights[0].ambient");
	int diffuseLocation = shader->GetUniformLocation("Lights[0].diffuse");
	int specularLocation = shader->GetUniformLocation("Lights[0].specular");
	int parameters1Location = shader->GetUniformLocation("Lights[0].parameters1");
	int parameters2Location = shader->GetUniformLocation("Lights[0].parameters2");
	int parameters3Location = shader->GetUniformLocation("Lights[0].parameters3");

	if (lightUnit > 0)
	{
		int type2Location = shader->GetUniformLocation("Lights[1].type");
		int offset = lightUnit * (type2Location - typeLocation); // type2Location - typeLocation donne la taille de la structure

		// On applique cet offset
		typeLocation += offset;
		ambientLocation += offset;
		diffuseLocation += offset;
		specularLocation += offset;
		parameters1Location += offset;
		parameters2Location += offset;
		parameters3Location += offset;
	}

	shader->SendInteger(typeLocation, m_type);
	shader->SendColor(ambientLocation, m_ambientColor);
	shader->SendColor(diffuseLocation, m_diffuseColor);
	shader->SendColor(specularLocation, m_specularColor);

	if (!m_derivedUpdated)
		UpdateDerived();

	switch (m_type)
	{
		case nzLightType_Directional:
			shader->SendVector(parameters1Location, NzVector4f(m_derivedRotation * NzVector3f::Forward()));
			break;

		case nzLightType_Point:
			shader->SendVector(parameters1Location, NzVector4f(m_derivedPosition, m_attenuation));
			shader->SendVector(parameters2Location, NzVector4f(1.f/m_radius, 0.f, 0.f, 0.f));
			break;

		case nzLightType_Spot:
			shader->SendVector(parameters1Location, NzVector4f(m_derivedPosition, m_attenuation));
			shader->SendVector(parameters2Location, NzVector4f(m_derivedRotation * NzVector3f::Forward(), 1.f/m_radius));
			shader->SendVector(parameters3Location, NzVector2f(std::cos(NzDegreeToRadian(m_innerAngle)), std::cos(NzDegreeToRadian(m_outerAngle))));
			break;
	}
}

const NzBoundingBoxf& NzLight::GetBoundingBox() const
{
	static NzBoundingBoxf dummy(nzExtend_Null);
	return dummy;
}

NzColor NzLight::GetAmbientColor() const
{
	return m_ambientColor;
}

float NzLight::GetAttenuation() const
{
	return m_attenuation;
}

NzColor NzLight::GetDiffuseColor() const
{
	return m_diffuseColor;
}

float NzLight::GetInnerAngle() const
{
	return m_innerAngle;
}

nzLightType NzLight::GetLightType() const
{
	return m_type;
}

float NzLight::GetOuterAngle() const
{
	return m_outerAngle;
}

float NzLight::GetRadius() const
{
	return m_radius;
}

nzSceneNodeType NzLight::GetSceneNodeType() const
{
	return nzSceneNodeType_Light;
}

NzColor NzLight::GetSpecularColor() const
{
	return m_specularColor;
}

void NzLight::SetAmbientColor(const NzColor& ambient)
{
	m_ambientColor = ambient;
}

void NzLight::SetAttenuation(float attenuation)
{
	m_attenuation = attenuation;
}

void NzLight::SetDiffuseColor(const NzColor& diffuse)
{
	m_diffuseColor = diffuse;
}

void NzLight::SetInnerAngle(float innerAngle)
{
	m_innerAngle = innerAngle;
}

void NzLight::SetOuterAngle(float outerAngle)
{
	m_outerAngle = outerAngle;
}

void NzLight::SetRadius(float radius)
{
	m_radius = radius;
}

void NzLight::SetSpecularColor(const NzColor& specular)
{
	m_specularColor = specular;
}

NzLight& NzLight::operator=(const NzLight& light)
{
	std::memcpy(this, &light, sizeof(NzLight));

	return *this;
}

void NzLight::Register()
{
}

void NzLight::Unregister()
{
}

bool NzLight::VisibilityTest(const NzFrustumf& frustum)
{
	NazaraUnused(frustum);

	///FIXME: Pour l'instant toujours visible
	return true; // Toujours visible
}
