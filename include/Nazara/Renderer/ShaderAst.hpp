// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_HPP
#define NAZARA_SHADER_AST_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <memory>

namespace Nz
{
	class ShaderWriter;

	namespace ShaderAst
	{
		enum class AssignType
		{
			Simple //< =
		};

		enum class BinaryType
		{
			Add,       //< +
			Substract, //< -
			Multiply,  //< *
			Divide,    //< /
			Equality   //< ==
		};

		enum class Builtin
		{
			VertexPosition, // gl_Position
		};

		enum class ExpressionType
		{
			Boolean, // bool
			Float1,  // float
			Float2,  // vec2
			Float3,  // vec3
			Float4,  // vec4
			Mat4x4,  // mat4

			None     // void
		};

		enum class VariableType
		{
			Builtin,
			Input,
			Output,
			Parameter,
			Uniform,
			Variable
		};

		//////////////////////////////////////////////////////////////////////////

		class Node;

		using NodePtr = std::shared_ptr<Node>;

		class NAZARA_RENDERER_API Node
		{
			public:
				virtual ~Node() = default;

				virtual void Register(ShaderWriter& visitor) = 0;
				virtual void Visit(ShaderWriter& visitor) = 0;
		};

		class Statement;

		using StatementPtr = std::shared_ptr<Statement>;

		class NAZARA_RENDERER_API Statement : public Node
		{
		};

		class Expression;

		using ExpressionPtr = std::shared_ptr<Expression>;

		class NAZARA_RENDERER_API Expression : public Node
		{
			public:
				virtual ExpressionType GetExpressionType() const = 0;
		};

		class NAZARA_RENDERER_API ExpressionStatement : public Statement
		{
			public:
				inline explicit ExpressionStatement(ExpressionPtr expr);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				ExpressionPtr expression;
		};

		//////////////////////////////////////////////////////////////////////////

		class NAZARA_RENDERER_API StatementBlock : public Statement
		{
			public:
				template<typename... Args> explicit StatementBlock(Args&&... args);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				std::vector<StatementPtr> statements;
		};

		class Variable;

		using VariablePtr = std::shared_ptr<Variable>;

		class NAZARA_RENDERER_API Variable : public Expression
		{
			public:
				inline Variable(VariableType varKind, ExpressionType varType);

				ExpressionType GetExpressionType() const override;

				ExpressionType type;
				VariableType   kind;
		};

		class NamedVariable;

		using NamedVariablePtr = std::shared_ptr<NamedVariable>;

		class NAZARA_RENDERER_API NamedVariable : public Variable
		{
			public:
				inline NamedVariable(VariableType varKind, const Nz::String& varName, ExpressionType varType);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				Nz::String name;
		};

		class NAZARA_RENDERER_API BuiltinVariable : public Variable
		{
			public:
				inline BuiltinVariable(Builtin variable, ExpressionType varType);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				Builtin var;
		};

		//////////////////////////////////////////////////////////////////////////

		class NAZARA_RENDERER_API AssignOp : public Expression
		{
			public:
				inline AssignOp(AssignType Op, VariablePtr Var, ExpressionPtr Right);

				ExpressionType GetExpressionType() const override;
				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				AssignType    op;
				VariablePtr   variable;
				ExpressionPtr right;
		};

		class NAZARA_RENDERER_API BinaryOp : public Expression
		{
			public:
				inline BinaryOp(BinaryType Op, ExpressionPtr Left, ExpressionPtr Right);

				ExpressionType GetExpressionType() const override;
				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				BinaryType    op;
				ExpressionPtr left;
				ExpressionPtr right;
		};

		class NAZARA_RENDERER_API Branch : public Statement
		{
			public:
				inline Branch(ExpressionPtr condition, StatementPtr trueStatement, StatementPtr falseStatement = nullptr);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				struct ConditionalStatement
				{
					ExpressionPtr condition;
					StatementPtr  statement;
				};

				std::vector<ConditionalStatement> condStatements;
				StatementPtr elseStatement;
		};

		class NAZARA_RENDERER_API Constant : public Expression
		{
			public:
				inline explicit Constant(float value);

				ExpressionType GetExpressionType() const override;
				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				ExpressionType exprType;

				union
				{
					float vec1;
					Nz::Vector2f vec2;
					Nz::Vector3f vec3;
					Nz::Vector4f vec4;
				} values;
		};
	}
}

#include <Nazara/Renderer/ShaderAst.inl>

#endif // NAZARA_SHADER_AST_HPP
