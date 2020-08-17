// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVWRITER_HPP
#define NAZARA_SPIRVWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAst.hpp>
#include <Nazara/Shader/ShaderAstVisitor.hpp>
#include <Nazara/Shader/ShaderVarVisitor.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Nz
{
	class SpirvSection;

	class NAZARA_SHADER_API SpirvWriter : public ShaderAstVisitor, public ShaderVarVisitor
	{
		public:
			struct Environment;

			SpirvWriter();
			SpirvWriter(const SpirvWriter&) = delete;
			SpirvWriter(SpirvWriter&&) = delete;
			~SpirvWriter() = default;

			std::vector<UInt32> Generate(const ShaderAst& shader);

			void SetEnv(Environment environment);

			struct Environment
			{
				UInt32 spvMajorVersion = 1;
				UInt32 spvMinorVersion = 0;
			};

		private:
			struct ExtVar;

			UInt32 AllocateResultId();

			void AppendConstants();
			void AppendHeader();
			void AppendStructType(std::size_t structIndex, UInt32 resultId);
			void AppendTypes();

			UInt32 EvaluateExpression(const ShaderNodes::ExpressionPtr& expr);

			UInt32 GetConstantId(const ShaderNodes::Constant::Variant& value) const;
			UInt32 GetTypeId(const ShaderExpressionType& type) const;

			void PushResultId(UInt32 value);
			UInt32 PopResultId();

			UInt32 ReadVariable(ExtVar& var);
			UInt32 RegisterType(ShaderExpressionType type);

			using ShaderAstVisitor::Visit;
			void Visit(ShaderNodes::AccessMember& node) override;
			void Visit(ShaderNodes::AssignOp& node) override;
			void Visit(ShaderNodes::Branch& node) override;
			void Visit(ShaderNodes::BinaryOp& node) override;
			void Visit(ShaderNodes::Cast& node) override;
			void Visit(ShaderNodes::Constant& node) override;
			void Visit(ShaderNodes::DeclareVariable& node) override;
			void Visit(ShaderNodes::ExpressionStatement& node) override;
			void Visit(ShaderNodes::Identifier& node) override;
			void Visit(ShaderNodes::IntrinsicCall& node) override;
			void Visit(ShaderNodes::Sample2D& node) override;
			void Visit(ShaderNodes::StatementBlock& node) override;
			void Visit(ShaderNodes::SwizzleOp& node) override;

			using ShaderVarVisitor::Visit;
			void Visit(ShaderNodes::BuiltinVariable& var) override;
			void Visit(ShaderNodes::InputVariable& var) override;
			void Visit(ShaderNodes::LocalVariable& var) override;
			void Visit(ShaderNodes::OutputVariable& var) override;
			void Visit(ShaderNodes::ParameterVariable& var) override;
			void Visit(ShaderNodes::UniformVariable& var) override;

			static void MergeBlocks(std::vector<UInt32>& output, const SpirvSection& from);

			struct Context
			{
				const ShaderAst* shader = nullptr;
				const ShaderAst::Function* currentFunction = nullptr;
			};

			struct State;

			Context m_context;
			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Shader/SpirvWriter.inl>

#endif
