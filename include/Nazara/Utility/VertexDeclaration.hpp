// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXDECLARATION_HPP
#define NAZARA_VERTEXDECLARATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Utility/Enums.hpp>

struct NzVertexElement
{
	unsigned int offset;
	unsigned int usageIndex = 0;
	nzElementStream stream = nzElementStream_VertexData;
	nzElementType type;
	nzElementUsage usage;
};

class NzVertexDeclaration;

using NzVertexDeclarationConstRef = NzResourceRef<const NzVertexDeclaration>;
using NzVertexDeclarationRef = NzResourceRef<NzVertexDeclaration>;

struct NzVertexDeclarationImpl;

class NAZARA_API NzVertexDeclaration : public NzResource
{
	public:
		NzVertexDeclaration() = default;
		NzVertexDeclaration(const NzVertexElement* elements, unsigned int elementCount);
		NzVertexDeclaration(const NzVertexDeclaration& declaration);
		NzVertexDeclaration(NzVertexDeclaration&& declaration) noexcept;
		~NzVertexDeclaration();

		bool Create(const NzVertexElement* elements, unsigned int elementCount);
		void Destroy();

		const NzVertexElement* GetElement(unsigned int i) const;
		const NzVertexElement* GetElement(nzElementStream stream, unsigned int i) const;
		const NzVertexElement* GetElement(nzElementStream stream, nzElementUsage usage, unsigned int usageIndex = 0) const;
		unsigned int GetElementCount() const;
		unsigned int GetElementCount(nzElementStream stream) const;
		unsigned int GetStride(nzElementStream stream) const;

		bool HasElement(unsigned int i) const;
		bool HasElement(nzElementStream stream, unsigned int i) const;
		bool HasElement(nzElementStream stream, nzElementUsage usage, unsigned int usageIndex = 0) const;
		bool HasStream(nzElementStream stream) const;

		bool IsValid() const;

		NzVertexDeclaration& operator=(const NzVertexDeclaration& declaration);
		NzVertexDeclaration& operator=(NzVertexDeclaration&& declaration) noexcept;

		static unsigned int GetElementCount(nzElementType type);
		static unsigned int GetElementSize(nzElementType type);

	private:
		NzVertexDeclarationImpl* m_sharedImpl = nullptr;
};

#endif // NAZARA_VERTEXDECLARATION_HPP
