// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/GuillotineImageAtlas.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	const unsigned int s_atlasStartSize = 512;
}

NzGuillotineImageAtlas::NzGuillotineImageAtlas() :
m_rectChoiceHeuristic(NzGuillotineBinPack::RectBestAreaFit),
m_rectSplitHeuristic(NzGuillotineBinPack::SplitMinimizeArea)
{
}

NzGuillotineImageAtlas::~NzGuillotineImageAtlas() = default;

void NzGuillotineImageAtlas::Clear()
{
	m_layers.clear();
	NotifyCleared();
}

void NzGuillotineImageAtlas::Free(NzSparsePtr<const NzRectui> rects, NzSparsePtr<unsigned int> layers, unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		#ifdef NAZARA_DEBUG
		if (layers[i] >= m_layers.size())
		{
			NazaraWarning("Rectangle #" + NzString::Number(i) + " belong to an out-of-bounds layer (" + NzString::Number(i) + " >= " + NzString::Number(m_layers.size()) + ")");
			continue;
		}
		#endif

		m_layers[layers[i]].binPack.FreeRectangle(rects[i]);
		m_layers[layers[i]].freedRectangles++;
	}
}

NzGuillotineBinPack::FreeRectChoiceHeuristic NzGuillotineImageAtlas::GetRectChoiceHeuristic() const
{
	return m_rectChoiceHeuristic;
}

NzGuillotineBinPack::GuillotineSplitHeuristic NzGuillotineImageAtlas::GetRectSplitHeuristic() const
{
	return m_rectSplitHeuristic;
}

NzAbstractImage* NzGuillotineImageAtlas::GetLayer(unsigned int layerIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (layerIndex >= m_layers.size())
	{
		NazaraError("Layer index out of range (" + NzString::Number(layerIndex) + " >= " + NzString::Number(m_layers.size()) + ')');
		return nullptr;
	}
	#endif

	Layer& layer = m_layers[layerIndex];
	ProcessGlyphQueue(layer);

	return layer.image.get();
}

unsigned int NzGuillotineImageAtlas::GetLayerCount() const
{
	return m_layers.size();
}

bool NzGuillotineImageAtlas::Insert(const NzImage& image, NzRectui* rect, bool* flipped, unsigned int* layerIndex)
{
	if (m_layers.empty())
		// On créé une première couche s'il n'y en a pas
		m_layers.resize(1);

	// Cette fonction ne fait qu'insérer un rectangle de façon virtuelle, l'insertion des images se fait après
	for (unsigned int i = 0; i < m_layers.size(); ++i)
	{
		Layer& layer = m_layers[i];

		// Une fois qu'un certain nombre de rectangles ont étés libérés d'une couche, on fusionne les rectangles libres
		if (layer.freedRectangles > 10) // Valeur totalement arbitraire
		{
			while (layer.binPack.MergeFreeRectangles()); // Tant qu'une fusion est possible
			layer.freedRectangles = 0; // Et on repart de zéro
		}

		if (layer.binPack.Insert(rect, flipped, 1, false, m_rectChoiceHeuristic, m_rectSplitHeuristic))
		{
			// Insertion réussie dans l'une des couches, on place le glyphe en file d'attente
			layer.queuedGlyphs.resize(layer.queuedGlyphs.size()+1);
			QueuedGlyph& glyph = layer.queuedGlyphs.back();
			glyph.flipped = *flipped;
			glyph.image = image; // Merci le Copy-On-Write
			glyph.rect = *rect;

			*layerIndex = i;
			return true;
		}
		else if (i == m_layers.size() - 1) // Dernière itération ?
		{
			// Dernière couche, et le glyphe ne rentre pas, peut-on agrandir la taille de l'image ?
			NzVector2ui newSize = layer.binPack.GetSize()*2;
			if (ResizeLayer(layer, newSize))
			{
				// Oui on peut !
				layer.binPack.Expand(newSize); // On ajuste l'atlas virtuel

				// Et on relance la boucle sur la nouvelle dernière couche
				i--;
			}
			else
			{
				// On ne peut plus agrandir la dernière couche, il est temps d'en créer une nouvelle
				newSize.Set(s_atlasStartSize);

				Layer newLayer;
				if (!ResizeLayer(newLayer, newSize))
				{
					// Impossible d'allouer une nouvelle couche, nous manquons probablement de mémoire (ou le glyphe est trop grand)
					NazaraError("Failed to allocate new layer, we are probably out of memory");
					return false;
				}

				newLayer.binPack.Reset(newSize);

				m_layers.emplace_back(std::move(newLayer)); // Insertion du layer

				// On laisse la boucle insérer toute seule le rectangle à la prochaine itération
			}
		}
	}

	NazaraInternalError("Unknown error"); // Normalement on ne peut pas arriver ici
	return false;
}

void NzGuillotineImageAtlas::SetRectChoiceHeuristic(NzGuillotineBinPack::FreeRectChoiceHeuristic heuristic)
{
	m_rectChoiceHeuristic = heuristic;
}

void NzGuillotineImageAtlas::SetRectSplitHeuristic(NzGuillotineBinPack::GuillotineSplitHeuristic heuristic)
{
	m_rectSplitHeuristic = heuristic;
}

NzAbstractImage* NzGuillotineImageAtlas::ResizeImage(NzAbstractImage* oldImage, const NzVector2ui& size) const
{
	std::unique_ptr<NzImage> newImage(new NzImage(nzImageType_2D, nzPixelFormat_A8, size.x, size.y));
	if (oldImage)
	{
		NzImage& image = *static_cast<NzImage*>(oldImage);
		newImage->Copy(image, NzRectui(size), NzVector2ui(0, 0)); // Copie des anciennes données
	}

	return newImage.release();
}

bool NzGuillotineImageAtlas::ResizeLayer(Layer& layer, const NzVector2ui& size)
{
	NzAbstractImage* oldLayer = layer.image.get();

	std::unique_ptr<NzAbstractImage> newImage(ResizeImage(layer.image.get(), size));
	if (!newImage)
		return false; // Nous n'avons pas pu allouer

	if (newImage.get() == oldLayer) // Le layer a été agrandi dans le même objet, pas de souci
	{
		newImage.release(); // On possède déjà un unique_ptr sur cette ressource
		return true;
	}

	// On indique à ceux que ça intéresse qu'on a changé de pointeur
	// (chose très importante pour ceux qui le stockent)
	NotifyLayerChange(layer.image.get(), newImage.get());

	// Et on ne met à jour le pointeur qu'après (car cette ligne libère également l'ancienne image)
	layer.image = std::move(newImage);

	return true;
}

void NzGuillotineImageAtlas::ProcessGlyphQueue(Layer& layer) const
{
	std::vector<nzUInt8> pixelBuffer;

	for (QueuedGlyph& glyph : layer.queuedGlyphs)
	{
		unsigned int glyphWidth = glyph.image.GetWidth();
		unsigned int glyphHeight = glyph.image.GetHeight();

		// Calcul de l'éventuel padding (pixels de contour)
		unsigned int paddingX;
		unsigned int paddingY;
		if (glyph.flipped)
		{
			paddingX = (glyph.rect.height - glyphWidth)/2;
			paddingY = (glyph.rect.width - glyphHeight)/2;
		}
		else
		{
			paddingX = (glyph.rect.width - glyphWidth)/2;
			paddingY = (glyph.rect.height - glyphHeight)/2;
		}

		if (paddingX > 0 || paddingY > 0)
		{
			// On remplit les contours
			pixelBuffer.resize(glyph.rect.width * glyph.rect.height);
			std::memset(pixelBuffer.data(), 0, glyph.rect.width*glyph.rect.height*sizeof(nzUInt8));

			layer.image->Update(pixelBuffer.data(), glyph.rect);
		}

		const nzUInt8* pixels;
		// On copie le glyphe dans l'atlas
		if (glyph.flipped)
		{
			pixelBuffer.resize(glyphHeight * glyphWidth);

			// On tourne le glyphe pour qu'il rentre dans le rectangle
			const nzUInt8* src = glyph.image.GetConstPixels();
			nzUInt8* ptr = pixelBuffer.data();

			unsigned int lineStride = glyphWidth*sizeof(nzUInt8); // BPP = 1
			src += lineStride-1; // Départ en haut à droite
			for (unsigned int x = 0; x < glyphWidth; ++x)
			{
				for (unsigned int y = 0; y < glyphHeight; ++y)
				{
					*ptr++ = *src;
					src += lineStride;
				}

				src -= glyphHeight*lineStride + 1;
			}

			pixels = pixelBuffer.data();
			std::swap(glyphWidth, glyphHeight);
		}
		else
			pixels = glyph.image.GetConstPixels();

		layer.image->Update(pixels, NzRectui(glyph.rect.x + paddingX, glyph.rect.y + paddingY, glyphWidth, glyphHeight), 0, glyphWidth, glyphHeight);
		glyph.image.Destroy(); // On libère l'image dès que possible (pour réduire la consommation)
	}

	layer.queuedGlyphs.clear();
}
