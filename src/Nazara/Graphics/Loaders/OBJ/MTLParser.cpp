// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Loaders/OBJ/MTLParser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cstdio>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzMTLParser::NzMTLParser(NzInputStream& stream) :
m_stream(stream),
m_streamFlags(stream.GetStreamOptions())
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags | nzStreamOption_Text);
}

NzMTLParser::~NzMTLParser()
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags);
}

const NzMTLParser::Material* NzMTLParser::GetMaterial(const NzString& materialName) const
{
	auto it = m_materials.find(materialName);
	if (it != m_materials.end())
		return &it->second;
	else
		return nullptr;
}

bool NzMTLParser::Parse()
{
	m_keepLastLine = false;
	m_lineCount = 0;
	m_materials.clear();

	Material* currentMaterial = nullptr;

	while (Advance(false))
	{
		NzString keyword = m_currentLine.GetWord(0).ToLower();
		if (keyword == "ka")
		{
			float r, g, b;
			if (std::sscanf(&m_currentLine[3], "%f %f %f", &r, &g, &b) == 3)
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->ambient = NzColor(r*255.f, g*255.f, b*255.f);
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "kd")
		{
			float r, g, b;
			if (std::sscanf(&m_currentLine[3], "%f %f %f", &r, &g, &b) == 3)
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->diffuse = NzColor(r*255.f, g*255.f, b*255.f);
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "ks")
		{
			float r, g, b;
			if (std::sscanf(&m_currentLine[3], "%f %f %f", &r, &g, &b) == 3)
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->specular = NzColor(r*255.f, g*255.f, b*255.f);
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "ni")
		{
			float density;
			if (std::sscanf(&m_currentLine[3], "%f", &density) == 1)
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->refractionIndex = density;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "ns")
		{
			float coef;
			if (std::sscanf(&m_currentLine[3], "%f", &coef) == 1)
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->shininess = coef;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == 'd' || keyword == "tr")
		{
			float alpha;
			if (std::sscanf(&m_currentLine[(keyword[0] == 'd') ? 2 : 3], "%f", &alpha) == 1)
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->alpha = alpha;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "illum")
		{
			unsigned int model;
			if (std::sscanf(&m_currentLine[6], "%u", &model) == 1)
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->illumModel = model;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "map_ka")
		{
			NzString map = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!map.IsEmpty())
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->ambientMap = map;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "map_kd")
		{
			NzString map = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!map.IsEmpty())
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->diffuseMap = map;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "map_ks")
		{
			NzString map = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!map.IsEmpty())
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->specularMap = map;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "map_bump" || keyword == "bump")
		{
			NzString map = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!map.IsEmpty())
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->bumpMap = map;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "map_d")
		{
			NzString map = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!map.IsEmpty())
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->alphaMap = map;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "map_decal" || keyword == "decal")
		{
			NzString map = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!map.IsEmpty())
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->decalMap = map;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "map_disp" || keyword == "disp")
		{
			NzString map = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!map.IsEmpty())
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->displacementMap = map;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "map_refl" || keyword == "refl")
		{
			NzString map = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!map.IsEmpty())
			{
				if (!currentMaterial)
					currentMaterial = &m_materials["default"];

				currentMaterial->reflectionMap = map;
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		else if (keyword == "newmtl")
		{
			NzString materialName = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
			if (!materialName.IsEmpty())
				currentMaterial = &m_materials[materialName];
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}
		#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
		else
			UnrecognizedLine();
		#endif
	}

	return true;
}

bool NzMTLParser::Advance(bool required)
{
	if (!m_keepLastLine)
	{
		do
		{
			if (m_stream.EndOfStream())
			{
				if (required)
					Error("Incomplete MTL file");

				return false;
			}

			m_lineCount++;

			m_currentLine = m_stream.ReadLine();
			m_currentLine = m_currentLine.SubstrTo("#"); // On ignore les commentaires
			m_currentLine.Simplify(); // Pour un traitement plus simple
		}
		while (m_currentLine.IsEmpty());
	}
	else
		m_keepLastLine = false;

	return true;
}

void NzMTLParser::Error(const NzString& message)
{
	NazaraError(message + " at line #" + NzString::Number(m_lineCount));
}

void NzMTLParser::Warning(const NzString& message)
{
	NazaraWarning(message + " at line #" + NzString::Number(m_lineCount));
}

void NzMTLParser::UnrecognizedLine(bool error)
{
	NzString message = "Unrecognized \"" + m_currentLine + '"';

	if (error)
		Error(message);
	else
		Warning(message);
}
