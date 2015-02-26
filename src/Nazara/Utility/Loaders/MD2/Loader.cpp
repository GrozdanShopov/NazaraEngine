// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD2.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/Loaders/MD2/Constants.hpp>
#include <cstddef>
#include <cstring>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool IsSupported(const NzString& extension)
	{
		return (extension == "md2");
	}

	nzTernary Check(NzInputStream& stream, const NzMeshParams& parameters)
	{
		NazaraUnused(parameters);

		nzUInt32 magic[2];
		if (stream.Read(&magic[0], 2*sizeof(nzUInt32)) == 2*sizeof(nzUInt32))
		{
			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&magic[0], sizeof(nzUInt32));
			NzByteSwap(&magic[1], sizeof(nzUInt32));
			#endif

			if (magic[0] == md2Ident && magic[1] == 8)
				return nzTernary_True;
		}

		return nzTernary_False;
	}

	bool Load(NzMesh* mesh, NzInputStream& stream, const NzMeshParams& parameters)
	{
		MD2_Header header;
		if (stream.Read(&header, sizeof(MD2_Header)) != sizeof(MD2_Header))
		{
			NazaraError("Failed to read header");
			return false;
		}

		#ifdef NAZARA_BIG_ENDIAN
		NzByteSwap(&header.skinwidth, sizeof(nzUInt32));
		NzByteSwap(&header.skinheight, sizeof(nzUInt32));
		NzByteSwap(&header.framesize, sizeof(nzUInt32));
		NzByteSwap(&header.num_skins, sizeof(nzUInt32));
		NzByteSwap(&header.num_vertices, sizeof(nzUInt32));
		NzByteSwap(&header.num_st, sizeof(nzUInt32));
		NzByteSwap(&header.num_tris, sizeof(nzUInt32));
		NzByteSwap(&header.num_glcmds, sizeof(nzUInt32));
		NzByteSwap(&header.num_frames, sizeof(nzUInt32));
		NzByteSwap(&header.offset_skins, sizeof(nzUInt32));
		NzByteSwap(&header.offset_st, sizeof(nzUInt32));
		NzByteSwap(&header.offset_tris, sizeof(nzUInt32));
		NzByteSwap(&header.offset_frames, sizeof(nzUInt32));
		NzByteSwap(&header.offset_glcmds, sizeof(nzUInt32));
		NzByteSwap(&header.offset_end, sizeof(nzUInt32));
		#endif

		if (stream.GetSize() < header.offset_end)
		{
			NazaraError("Incomplete MD2 file");
			return false;
		}

		/// Création du mesh
		// Le moteur ne supporte plus les animations image-clé, nous ne pouvons charger qu'en statique
		if (!mesh->CreateStatic()) // Ne devrait jamais échouer
		{
			NazaraInternalError("Failed to create mesh");
			return false;
		}

		/// Chargement des skins
		if (header.num_skins > 0)
		{
			mesh->SetMaterialCount(header.num_skins);
			stream.SetCursorPos(header.offset_skins);
			{
				NzString baseDir = stream.GetDirectory();
				char skin[68];
				for (unsigned int i = 0; i < header.num_skins; ++i)
				{
					stream.Read(skin, 68*sizeof(char));
					mesh->SetMaterial(i, baseDir + skin);
				}
			}
		}

		/// Chargement des submesh
		// Actuellement le loader ne charge qu'un submesh
		NzIndexBufferRef indexBuffer = NzIndexBuffer::New(false, header.num_tris*3, parameters.storage, nzBufferUsage_Static);

		/// Lecture des triangles
		std::vector<MD2_Triangle> triangles(header.num_tris);

		stream.SetCursorPos(header.offset_tris);
		stream.Read(&triangles[0], header.num_tris*sizeof(MD2_Triangle));

		NzBufferMapper<NzIndexBuffer> indexMapper(indexBuffer, nzBufferAccess_DiscardAndWrite);
		nzUInt16* index = reinterpret_cast<nzUInt16*>(indexMapper.GetPointer());

		for (unsigned int i = 0; i < header.num_tris; ++i)
		{
			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&triangles[i].vertices[0], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[0], sizeof(nzUInt16));

			NzByteSwap(&triangles[i].vertices[1], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[1], sizeof(nzUInt16));

			NzByteSwap(&triangles[i].vertices[2], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[2], sizeof(nzUInt16));
			#endif

			// On respécifie le triangle dans l'ordre attendu
			*index++ = triangles[i].vertices[0];
			*index++ = triangles[i].vertices[2];
			*index++ = triangles[i].vertices[1];
		}

		indexMapper.Unmap();

		if (parameters.optimizeIndexBuffers)
			indexBuffer->Optimize();

		/// Lecture des coordonnées de texture
		std::vector<MD2_TexCoord> texCoords(header.num_st);

		stream.SetCursorPos(header.offset_st);
		stream.Read(&texCoords[0], header.num_st*sizeof(MD2_TexCoord));

		#ifdef NAZARA_BIG_ENDIAN
		for (unsigned int i = 0; i < header.num_st; ++i)
		{
			NzByteSwap(&texCoords[i].u, sizeof(nzInt16));
			NzByteSwap(&texCoords[i].v, sizeof(nzInt16));
		}
		#endif

		NzVertexBufferRef vertexBuffer = NzVertexBuffer::New(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Normal_UV_Tangent), header.num_vertices, parameters.storage, nzBufferUsage_Static);
		NzStaticMeshRef subMesh = NzStaticMesh::New(mesh);
		if (!subMesh->Create(vertexBuffer))
		{
			NazaraError("Failed to create SubMesh");
			return false;
		}

		/// Chargement des vertices
		stream.SetCursorPos(header.offset_frames);

		std::unique_ptr<MD2_Vertex[]> vertices(new MD2_Vertex[header.num_vertices]);
		NzVector3f scale, translate;
		stream.Read(scale, sizeof(NzVector3f));
		stream.Read(translate, sizeof(NzVector3f));
		stream.Read(nullptr, 16*sizeof(char)); // Nom de la frame, inutile ici
		stream.Read(vertices.get(), header.num_vertices*sizeof(MD2_Vertex));

		#ifdef NAZARA_BIG_ENDIAN
		NzByteSwap(&scale.x, sizeof(float));
		NzByteSwap(&scale.y, sizeof(float));
		NzByteSwap(&scale.z, sizeof(float));

		NzByteSwap(&translate.x, sizeof(float));
		NzByteSwap(&translate.y, sizeof(float));
		NzByteSwap(&translate.z, sizeof(float));
		#endif

		// Un personnage de taille moyenne fait ~50 unités de haut dans Quake 2
		// Avec Nazara, 1 unité = 1 mètre, nous devons donc adapter l'échelle
		NzVector3f s(parameters.scale/29.f); // 50/29 = 1.72 (Soit 1.72 mètre, proche de la taille moyenne d'un individu)
		scale *= s;
		translate *= s;

		NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer, nzBufferAccess_DiscardAndWrite);
		NzMeshVertex* vertex = reinterpret_cast<NzMeshVertex*>(vertexMapper.GetPointer());

		/// Chargement des coordonnées de texture
		const unsigned int indexFix[3] = {0, 2, 1}; // Pour respécifier les indices dans le bon ordre
		for (unsigned int i = 0; i < header.num_tris; ++i)
		{
			for (unsigned int j = 0; j < 3; ++j)
			{
				const unsigned int fixedIndex = indexFix[j];
				const MD2_TexCoord& texC = texCoords[triangles[i].texCoords[fixedIndex]];
				float u = static_cast<float>(texC.u) / header.skinwidth;
				float v = static_cast<float>(texC.v) / header.skinheight;

				vertex[triangles[i].vertices[fixedIndex]].uv.Set(u, (parameters.flipUVs) ? 1.f - v : v);
			}
		}

		/// Chargement des positions
		// Pour que le modèle soit correctement aligné, on génère un quaternion que nous appliquerons à chacune des vertices
		NzQuaternionf rotationQuat = NzEulerAnglesf(-90.f, 90.f, 0.f);

		for (unsigned int v = 0; v < header.num_vertices; ++v)
		{
			const MD2_Vertex& vert = vertices[v];
			NzVector3f position = rotationQuat * NzVector3f(vert.x*scale.x + translate.x, vert.y*scale.y + translate.y, vert.z*scale.z + translate.z);

			vertex->position = position;
			vertex->normal = rotationQuat * md2Normals[vert.n];

			vertex++;
		}

		vertexMapper.Unmap();

		subMesh->SetIndexBuffer(indexBuffer);
		subMesh->SetMaterialIndex(0);

		subMesh->GenerateAABB();
		subMesh->GenerateTangents();

		mesh->AddSubMesh(subMesh);

		if (parameters.center)
			mesh->Recenter();

		return true;
	}
}

void NzLoaders_MD2_Register()
{
	NzMeshLoader::RegisterLoader(IsSupported, Check, Load);
}

void NzLoaders_MD2_Unregister()
{
	NzMeshLoader::UnregisterLoader(IsSupported, Check, Load);
}
