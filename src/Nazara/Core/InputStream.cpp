// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	InputStream::~InputStream() = default;

	String InputStream::ReadLine(unsigned int lineSize)
	{
		String line;
		if (lineSize == 0) // Taille maximale indéterminée
		{
			const unsigned int bufferSize = 64;

			char buffer[bufferSize+1];
			buffer[bufferSize] = '\0';

			unsigned int readSize;
			do
			{
				readSize = Read(buffer, bufferSize);

				const char* ptr = std::strchr(buffer, '\n');
				if (ptr)
				{
					unsigned int pos = ptr-buffer;

					if (m_streamOptions & StreamOption_Text && pos > 0 && buffer[pos-1] == '\r')
						line.Append(buffer, pos-1);
					else
						line.Append(buffer, pos);

					if (!SetCursorPos(GetCursorPos() - readSize + pos + 1))
						NazaraWarning("Failed to reset cursos pos");

					break;
				}
				else
					line.Append(buffer, readSize);
			}
			while (readSize == bufferSize);
		}
		else
		{
			line.Set(lineSize, '\0');
			unsigned int readSize = Read(&line[0], lineSize);
			unsigned int pos = line.Find('\n');
			if (pos <= readSize) // Faux uniquement si le caractère n'est pas présent (npos étant le plus grand entier)
			{
				if (m_streamOptions & StreamOption_Text && pos > 0 && line[pos-1] == '\r')
					line.Resize(pos);
				else
					line.Resize(pos+1);

				if (!SetCursorPos(GetCursorPos() - readSize + pos + 1))
					NazaraWarning("Failed to reset cursos pos");
			}
			else
				line.Resize(readSize);
		}

		return line;
	}
}
