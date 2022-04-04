// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	struct EliminateUnusedPassVisitor::Context
	{
		const DependencyCheckerVisitor::UsageSet& usageSet;
	};

	ModulePtr EliminateUnusedPassVisitor::Process(const Module& shaderModule, const DependencyCheckerVisitor::UsageSet& usageSet)
	{
		auto rootNode = StaticUniquePointerCast<MultiStatement>(Process(*shaderModule.rootNode, usageSet));
		
		return std::make_shared<Module>(shaderModule.metadata, std::move(rootNode), shaderModule.importedModules);
	}

	StatementPtr EliminateUnusedPassVisitor::Process(Statement& statement, const DependencyCheckerVisitor::UsageSet& usageSet)
	{
		Context context{
			usageSet
		};

		m_context = &context;
		CallOnExit onExit([this]()
		{
			m_context = nullptr;
		});

		return Clone(statement);
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareAliasStatement& node)
	{
		assert(node.aliasIndex);
		if (!IsAliasUsed(*node.aliasIndex))
			return ShaderBuilder::NoOp();

		return AstCloner::Clone(node);
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareExternalStatement& node)
	{
		bool isUsed = false;
		for (const auto& externalVar : node.externalVars)
		{
			assert(externalVar.varIndex);
			std::size_t varIndex = *externalVar.varIndex;

			if (IsVariableUsed(varIndex))
			{
				isUsed = true;
				break;
			}
		}

		if (!isUsed)
			return ShaderBuilder::NoOp();

		auto clonedNode = AstCloner::Clone(node);

		auto& externalStatement = static_cast<DeclareExternalStatement&>(*clonedNode);
		for (auto it = externalStatement.externalVars.begin(); it != externalStatement.externalVars.end(); )
		{
			const auto& externalVar = *it;
			assert(externalVar.varIndex);
			std::size_t varIndex = *externalVar.varIndex;

			if (!IsVariableUsed(varIndex))
				it = externalStatement.externalVars.erase(it);
			else
				++it;
		}

		return clonedNode;
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareFunctionStatement& node)
	{
		assert(node.funcIndex);
		if (!IsFunctionUsed(*node.funcIndex))
			return ShaderBuilder::NoOp();

		return AstCloner::Clone(node);
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareStructStatement& node)
	{
		assert(node.structIndex);
		if (!IsStructUsed(*node.structIndex))
			return ShaderBuilder::NoOp();

		return AstCloner::Clone(node);
	}

	StatementPtr EliminateUnusedPassVisitor::Clone(DeclareVariableStatement& node)
	{
		assert(node.varIndex);
		if (!IsVariableUsed(*node.varIndex))
			return ShaderBuilder::NoOp();

		return AstCloner::Clone(node);
	}

	bool EliminateUnusedPassVisitor::IsAliasUsed(std::size_t aliasIndex) const
	{
		assert(m_context);
		return m_context->usageSet.usedAliases.UnboundedTest(aliasIndex);
	}

	bool EliminateUnusedPassVisitor::IsFunctionUsed(std::size_t funcIndex) const
	{
		assert(m_context);
		return m_context->usageSet.usedFunctions.UnboundedTest(funcIndex);
	}

	bool EliminateUnusedPassVisitor::IsStructUsed(std::size_t structIndex) const
	{
		assert(m_context);
		return m_context->usageSet.usedStructs.UnboundedTest(structIndex);
	}

	bool EliminateUnusedPassVisitor::IsVariableUsed(std::size_t varIndex) const
	{
		assert(m_context);
		return m_context->usageSet.usedVariables.UnboundedTest(varIndex);
	}
}
