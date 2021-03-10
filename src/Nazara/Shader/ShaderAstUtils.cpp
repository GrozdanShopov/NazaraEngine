// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstUtils.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	ExpressionCategory ShaderAstValueCategory::GetExpressionCategory(Expression& expression)
	{
		expression.Visit(*this);
		return m_expressionCategory;
	}

	void ShaderAstValueCategory::Visit(AccessMemberExpression& node)
	{
		node.structExpr->Visit(*this);
	}

	void ShaderAstValueCategory::Visit(AssignExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(BinaryExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(CastExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(ConditionalExpression& node)
	{
		node.truePath->Visit(*this);
		ExpressionCategory trueExprCategory = m_expressionCategory;

		node.falsePath->Visit(*this);
		ExpressionCategory falseExprCategory = m_expressionCategory;

		if (trueExprCategory == ExpressionCategory::RValue || falseExprCategory == ExpressionCategory::RValue)
			m_expressionCategory = ExpressionCategory::RValue;
		else
			m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(ConstantExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(IdentifierExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::LValue;
	}

	void ShaderAstValueCategory::Visit(IntrinsicExpression& /*node*/)
	{
		m_expressionCategory = ExpressionCategory::RValue;
	}

	void ShaderAstValueCategory::Visit(SwizzleExpression& node)
	{
		node.expression->Visit(*this);
	}
}
