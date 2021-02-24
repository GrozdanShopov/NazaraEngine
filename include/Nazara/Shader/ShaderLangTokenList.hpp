// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if !defined(NAZARA_SHADERLANG_TOKEN)
#error You must define NAZARA_SHADERLANG_TOKEN before including this file
#endif

#ifndef NAZARA_SHADERLANG_TOKENT_LAST
#define NAZARA_SHADERLANG_TOKEN_LAST(X) NAZARA_SHADERLANG_TOKEN(X)
#endif

NAZARA_SHADERLANG_TOKEN(Add)
NAZARA_SHADERLANG_TOKEN(BoolFalse)
NAZARA_SHADERLANG_TOKEN(BoolTrue)
NAZARA_SHADERLANG_TOKEN(ClosingParenthesis)
NAZARA_SHADERLANG_TOKEN(ClosingCurlyBracket)
NAZARA_SHADERLANG_TOKEN(Colon)
NAZARA_SHADERLANG_TOKEN(Comma)
NAZARA_SHADERLANG_TOKEN(Divide)
NAZARA_SHADERLANG_TOKEN(Dot)
NAZARA_SHADERLANG_TOKEN(FloatingPointValue)
NAZARA_SHADERLANG_TOKEN(EndOfStream)
NAZARA_SHADERLANG_TOKEN(FunctionDeclaration)
NAZARA_SHADERLANG_TOKEN(FunctionReturn)
NAZARA_SHADERLANG_TOKEN(IntegerValue)
NAZARA_SHADERLANG_TOKEN(Identifier)
NAZARA_SHADERLANG_TOKEN(Multiply)
NAZARA_SHADERLANG_TOKEN(OpenCurlyBracket)
NAZARA_SHADERLANG_TOKEN(OpenParenthesis)
NAZARA_SHADERLANG_TOKEN(Semicolon)
NAZARA_SHADERLANG_TOKEN(Return)
NAZARA_SHADERLANG_TOKEN(Subtract)

#undef NAZARA_SHADERLANG_TOKEN
#undef NAZARA_SHADERLANG_TOKEN_LAST
