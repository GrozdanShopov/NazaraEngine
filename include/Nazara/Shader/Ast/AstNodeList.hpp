// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if !defined(NAZARA_SHADERAST_NODE) && !defined(NAZARA_SHADERAST_EXPRESSION) && !defined(NAZARA_SHADERAST_STATEMENT)
#error You must define NAZARA_SHADERAST_NODE or NAZARA_SHADERAST_EXPRESSION or NAZARA_SHADERAST_STATEMENT before including this file
#endif

#ifndef NAZARA_SHADERAST_NODE
#define NAZARA_SHADERAST_NODE(X)
#endif

#ifndef NAZARA_SHADERAST_NODE_LAST
#define NAZARA_SHADERAST_NODE_LAST(X)
#endif

#ifndef NAZARA_SHADERAST_EXPRESSION
#define NAZARA_SHADERAST_EXPRESSION(X) NAZARA_SHADERAST_NODE(X)
#endif

#ifndef NAZARA_SHADERAST_STATEMENT
#define NAZARA_SHADERAST_STATEMENT(X) NAZARA_SHADERAST_NODE(X)
#endif

#ifndef NAZARA_SHADERAST_STATEMENT_LAST
#define NAZARA_SHADERAST_STATEMENT_LAST(X) NAZARA_SHADERAST_STATEMENT(X)
#endif

NAZARA_SHADERAST_EXPRESSION(AccessIdentifierExpression)
NAZARA_SHADERAST_EXPRESSION(AccessIndexExpression)
NAZARA_SHADERAST_EXPRESSION(AssignExpression)
NAZARA_SHADERAST_EXPRESSION(BinaryExpression)
NAZARA_SHADERAST_EXPRESSION(CallFunctionExpression)
NAZARA_SHADERAST_EXPRESSION(CallMethodExpression)
NAZARA_SHADERAST_EXPRESSION(CastExpression)
NAZARA_SHADERAST_EXPRESSION(ConditionalExpression)
NAZARA_SHADERAST_EXPRESSION(ConstantExpression)
NAZARA_SHADERAST_EXPRESSION(ConstantValueExpression)
NAZARA_SHADERAST_EXPRESSION(IdentifierExpression)
NAZARA_SHADERAST_EXPRESSION(IntrinsicExpression)
NAZARA_SHADERAST_EXPRESSION(SwizzleExpression)
NAZARA_SHADERAST_EXPRESSION(VariableExpression)
NAZARA_SHADERAST_EXPRESSION(UnaryExpression)
NAZARA_SHADERAST_STATEMENT(BranchStatement)
NAZARA_SHADERAST_STATEMENT(ConditionalStatement)
NAZARA_SHADERAST_STATEMENT(DeclareConstStatement)
NAZARA_SHADERAST_STATEMENT(DeclareExternalStatement)
NAZARA_SHADERAST_STATEMENT(DeclareFunctionStatement)
NAZARA_SHADERAST_STATEMENT(DeclareOptionStatement)
NAZARA_SHADERAST_STATEMENT(DeclareStructStatement)
NAZARA_SHADERAST_STATEMENT(DeclareVariableStatement)
NAZARA_SHADERAST_STATEMENT(DiscardStatement)
NAZARA_SHADERAST_STATEMENT(ExpressionStatement)
NAZARA_SHADERAST_STATEMENT(MultiStatement)
NAZARA_SHADERAST_STATEMENT(NoOpStatement)
NAZARA_SHADERAST_STATEMENT_LAST(ReturnStatement)

#undef NAZARA_SHADERAST_EXPRESSION
#undef NAZARA_SHADERAST_NODE
#undef NAZARA_SHADERAST_NODE_LAST
#undef NAZARA_SHADERAST_STATEMENT
#undef NAZARA_SHADERAST_STATEMENT_LAST
