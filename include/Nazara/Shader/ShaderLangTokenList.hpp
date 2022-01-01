// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

#if !defined(NAZARA_SHADERLANG_TOKEN)
#error You must define NAZARA_SHADERLANG_TOKEN before including this file
#endif

#ifndef NAZARA_SHADERLANG_TOKEN_LAST
#define NAZARA_SHADERLANG_TOKEN_LAST(X) NAZARA_SHADERLANG_TOKEN(X)
#endif

NAZARA_SHADERLANG_TOKEN(Assign)
NAZARA_SHADERLANG_TOKEN(BoolFalse)
NAZARA_SHADERLANG_TOKEN(BoolTrue)
NAZARA_SHADERLANG_TOKEN(ClosingParenthesis)
NAZARA_SHADERLANG_TOKEN(ClosingCurlyBracket)
NAZARA_SHADERLANG_TOKEN(ClosingSquareBracket)
NAZARA_SHADERLANG_TOKEN(Colon)
NAZARA_SHADERLANG_TOKEN(Comma)
NAZARA_SHADERLANG_TOKEN(Const)
NAZARA_SHADERLANG_TOKEN(ConstSelect)
NAZARA_SHADERLANG_TOKEN(Discard)
NAZARA_SHADERLANG_TOKEN(Divide)
NAZARA_SHADERLANG_TOKEN(DivideAssign)
NAZARA_SHADERLANG_TOKEN(Dot)
NAZARA_SHADERLANG_TOKEN(Equal)
NAZARA_SHADERLANG_TOKEN(Else)
NAZARA_SHADERLANG_TOKEN(EndOfStream)
NAZARA_SHADERLANG_TOKEN(External)
NAZARA_SHADERLANG_TOKEN(FloatingPointValue)
NAZARA_SHADERLANG_TOKEN(FunctionDeclaration)
NAZARA_SHADERLANG_TOKEN(FunctionReturn)
NAZARA_SHADERLANG_TOKEN(GreaterThan)
NAZARA_SHADERLANG_TOKEN(GreaterThanEqual)
NAZARA_SHADERLANG_TOKEN(IntegerValue)
NAZARA_SHADERLANG_TOKEN(Identifier)
NAZARA_SHADERLANG_TOKEN(If)
NAZARA_SHADERLANG_TOKEN(LessThan)
NAZARA_SHADERLANG_TOKEN(LessThanEqual)
NAZARA_SHADERLANG_TOKEN(Let)
NAZARA_SHADERLANG_TOKEN(LogicalAnd)
NAZARA_SHADERLANG_TOKEN(LogicalAndAssign)
NAZARA_SHADERLANG_TOKEN(LogicalOr)
NAZARA_SHADERLANG_TOKEN(LogicalOrAssign)
NAZARA_SHADERLANG_TOKEN(Multiply)
NAZARA_SHADERLANG_TOKEN(MultiplyAssign)
NAZARA_SHADERLANG_TOKEN(Minus)
NAZARA_SHADERLANG_TOKEN(MinusAssign)
NAZARA_SHADERLANG_TOKEN(Not)
NAZARA_SHADERLANG_TOKEN(NotEqual)
NAZARA_SHADERLANG_TOKEN(Plus)
NAZARA_SHADERLANG_TOKEN(PlusAssign)
NAZARA_SHADERLANG_TOKEN(OpenCurlyBracket)
NAZARA_SHADERLANG_TOKEN(OpenSquareBracket)
NAZARA_SHADERLANG_TOKEN(OpenParenthesis)
NAZARA_SHADERLANG_TOKEN(Option)
NAZARA_SHADERLANG_TOKEN(Semicolon)
NAZARA_SHADERLANG_TOKEN(Return)
NAZARA_SHADERLANG_TOKEN(Struct)
NAZARA_SHADERLANG_TOKEN(While)

#undef NAZARA_SHADERLANG_TOKEN
#undef NAZARA_SHADERLANG_TOKEN_LAST
