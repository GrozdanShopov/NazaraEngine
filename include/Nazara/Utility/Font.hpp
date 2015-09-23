// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

///TODO: FontManager ?

#pragma once

#ifndef NAZARA_FONT_HPP
#define NAZARA_FONT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Utility/AbstractAtlas.hpp>
#include <memory>
#include <unordered_map>

struct NAZARA_UTILITY_API NzFontParams
{
	bool IsValid() const;
};

class NzFont;
class NzFontData;

struct NzFontGlyph;

using NzFontConstRef = NzObjectRef<const NzFont>;
using NzFontLibrary = NzObjectLibrary<NzFont>;
using NzFontLoader = NzResourceLoader<NzFont, NzFontParams>;
using NzFontRef = NzObjectRef<NzFont>;

class NAZARA_UTILITY_API NzFont : public NzRefCounted, public NzResource
{
	friend NzFontLibrary;
	friend NzFontLoader;
	friend class NzUtility;

	public:
		struct Glyph;
		struct SizeInfo;

		NzFont();
        NzFont(const NzFont&) = delete;
        NzFont(NzFont&&) = delete;
		~NzFont();

		void ClearGlyphCache();
		void ClearKerningCache();
		void ClearSizeInfoCache();

		bool Create(NzFontData* data);
		void Destroy();

		bool ExtractGlyph(unsigned int characterSize, char32_t character, nzUInt32 style, NzFontGlyph* glyph) const;

		const std::shared_ptr<NzAbstractAtlas>& GetAtlas() const;
		unsigned int GetCachedGlyphCount(unsigned int characterSize, nzUInt32 style) const;
		unsigned int GetCachedGlyphCount() const;
		NzString GetFamilyName() const;
		int GetKerning(unsigned int characterSize, char32_t first, char32_t second) const;
		const Glyph& GetGlyph(unsigned int characterSize, nzUInt32 style, char32_t character) const;
		unsigned int GetGlyphBorder() const;
		unsigned int GetMinimumStepSize() const;
		const SizeInfo& GetSizeInfo(unsigned int characterSize) const;
		NzString GetStyleName() const;

		bool IsValid() const;

		bool Precache(unsigned int characterSize, nzUInt32 style, char32_t character) const;
		bool Precache(unsigned int characterSize, nzUInt32 style, const NzString& characterSet) const;

		// Open
		bool OpenFromFile(const NzString& filePath, const NzFontParams& params = NzFontParams());
		bool OpenFromMemory(const void* data, std::size_t size, const NzFontParams& params = NzFontParams());
		bool OpenFromStream(NzInputStream& stream, const NzFontParams& params = NzFontParams());

		void SetAtlas(const std::shared_ptr<NzAbstractAtlas>& atlas);
		void SetGlyphBorder(unsigned int borderSize);
		void SetMinimumStepSize(unsigned int minimumStepSize);

        NzFont& operator=(const NzFont&) = delete;
        NzFont& operator=(NzFont&&) = delete;

		static std::shared_ptr<NzAbstractAtlas> GetDefaultAtlas();
		static NzFont* GetDefault();
		static unsigned int GetDefaultGlyphBorder();
		static unsigned int GetDefaultMinimumStepSize();

		template<typename... Args> static NzFontRef New(Args&&... args);

		static void SetDefaultAtlas(const std::shared_ptr<NzAbstractAtlas>& atlas);
		static void SetDefaultGlyphBorder(unsigned int borderSize);
		static void SetDefaultMinimumStepSize(unsigned int minimumStepSize);

		struct Glyph
		{
			NzRecti aabb;
			NzRectui atlasRect;
			bool requireFauxBold;
			bool requireFauxItalic;
			bool flipped;
			bool valid;
			int advance;
			unsigned int layerIndex;
		};

		struct SizeInfo
		{
			int spaceAdvance;
			unsigned int lineHeight;
			float underlinePosition;
			float underlineThickness;
		};

		// Signals:
		NazaraSignal(OnFontAtlasChanged, const NzFont* /*font*/);
		NazaraSignal(OnFontAtlasLayerChanged, const NzFont* /*font*/, NzAbstractImage* /*oldLayer*/, NzAbstractImage* /*newLayer*/);
		NazaraSignal(OnFontDestroy, const NzFont* /*font*/);
		NazaraSignal(OnFontGlyphCacheCleared, const NzFont* /*font*/);
		NazaraSignal(OnFontKerningCacheCleared, const NzFont* /*font*/);
		NazaraSignal(OnFontRelease, const NzFont* /*font*/);
		NazaraSignal(OnFontSizeInfoCacheCleared, const NzFont* /*font*/);

	private:
		using GlyphMap = std::unordered_map<char32_t, Glyph>;

		nzUInt64 ComputeKey(unsigned int characterSize, nzUInt32 style) const;
		void OnAtlasCleared(const NzAbstractAtlas* atlas);
		void OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer);
		void OnAtlasRelease(const NzAbstractAtlas* atlas);
		const Glyph& PrecacheGlyph(GlyphMap& glyphMap, unsigned int characterSize, nzUInt32 style, char32_t character) const;

		static bool Initialize();
		static void Uninitialize();

		NazaraSlot(NzAbstractAtlas, OnAtlasCleared, m_atlasClearedSlot);
		NazaraSlot(NzAbstractAtlas, OnAtlasLayerChange, m_atlasLayerChangeSlot);
		NazaraSlot(NzAbstractAtlas, OnAtlasRelease, m_atlasReleaseSlot);

		std::shared_ptr<NzAbstractAtlas> m_atlas;
		std::unique_ptr<NzFontData> m_data;
		mutable std::unordered_map<nzUInt64, std::unordered_map<nzUInt64, int>> m_kerningCache;
		mutable std::unordered_map<nzUInt64, GlyphMap> m_glyphes;
		mutable std::unordered_map<nzUInt64, SizeInfo> m_sizeInfoCache;
		unsigned int m_glyphBorder;
		unsigned int m_minimumStepSize;

		static std::shared_ptr<NzAbstractAtlas> s_defaultAtlas;
		static NzFontRef s_defaultFont;
		static NzFontLibrary::LibraryMap s_library;
		static NzFontLoader::LoaderList s_loaders;
		static unsigned int s_defaultGlyphBorder;
		static unsigned int s_defaultMinimumStepSize;
};

#include <Nazara/Utility/Font.inl>

#endif // NAZARA_FONT_HPP
