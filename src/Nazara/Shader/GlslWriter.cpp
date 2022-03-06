// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Ast/AstConstantPropagationVisitor.hpp>
#include <Nazara/Shader/Ast/AstRecursiveVisitor.hpp>
#include <Nazara/Shader/Ast/AstUtils.hpp>
#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <optional>
#include <set>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		static const char* s_flipYUniformName = "_NzFlipYValue";
		static const char* s_inputPrefix = "_NzIn_";
		static const char* s_outputPrefix = "_NzOut_";
		static const char* s_outputVarName = "_nzOutput";

		template<typename T> const T& Retrieve(const std::unordered_map<std::size_t, T>& map, std::size_t id)
		{
			auto it = map.find(id);
			assert(it != map.end());
			return it->second;
		}

		struct PreVisitor : ShaderAst::AstRecursiveVisitor
		{
			using AstRecursiveVisitor::Visit;

			void Visit(ShaderAst::CallFunctionExpression& node) override
			{
				AstRecursiveVisitor::Visit(node);

				assert(currentFunction);
				currentFunction->calledFunctions.UnboundedSet(std::get<ShaderAst::FunctionType>(GetExpressionType(*node.targetFunction)).funcIndex);
			}

			void Visit(ShaderAst::ConditionalExpression& /*node*/) override
			{
				throw std::runtime_error("unexpected conditional expression, is shader sanitized?");
			}

			void Visit(ShaderAst::ConditionalStatement& /*node*/) override
			{
				throw std::runtime_error("unexpected conditional statement, is shader sanitized?");
			}

			void Visit(ShaderAst::DeclareFunctionStatement& node) override
			{
				// Dismiss function if it's an entry point of another type than the one selected
				if (node.entryStage.HasValue())
				{
					if (selectedStage)
					{
						if (!node.entryStage.IsResultingValue())
							throw std::runtime_error("unexpected unresolved value for entry attribute, is shader sanitized?");

						ShaderStageType stage = node.entryStage.GetResultingValue();
						if (stage != *selectedStage)
							return;

						assert(!entryPoint);
						entryPoint = &node;
					}
					else
					{
						if (entryPoint)
							throw std::runtime_error("multiple entry point functions found, this is not allowed in GLSL, please select one");

						entryPoint = &node;
					}
				}

				assert(node.funcIndex);
				assert(functions.find(node.funcIndex.value()) == functions.end());
				FunctionData& funcData = functions[node.funcIndex.value()];
				funcData.name = node.name;
				funcData.node = &node;

				currentFunction = &funcData;

				AstRecursiveVisitor::Visit(node);

				currentFunction = nullptr;
			}

			struct FunctionData
			{
				std::string name;
				Bitset<> calledFunctions;
				ShaderAst::DeclareFunctionStatement* node;
			};

			FunctionData* currentFunction = nullptr;

			std::optional<ShaderStageType> selectedStage;
			std::unordered_map<std::size_t, FunctionData> functions;
			ShaderAst::DeclareFunctionStatement* entryPoint = nullptr;
		};

		struct Builtin
		{
			std::string identifier;
			ShaderStageTypeFlags stageFlags;
		};

		std::unordered_map<ShaderAst::BuiltinEntry, Builtin> s_builtinMapping = {
			{ ShaderAst::BuiltinEntry::FragCoord,      { "gl_FragCoord", ShaderStageType::Fragment } },
			{ ShaderAst::BuiltinEntry::FragDepth,      { "gl_FragDepth", ShaderStageType::Fragment } },
			{ ShaderAst::BuiltinEntry::VertexPosition, { "gl_Position", ShaderStageType::Vertex } }
		};
	}


	struct GlslWriter::State
	{
		State(const GlslWriter::BindingMapping& bindings) :
		bindingMapping(bindings)
		{
		}

		struct InOutField
		{
			std::string memberName;
			std::string targetName;
		};

		std::optional<ShaderStageType> stage;
		std::stringstream stream;
		std::unordered_map<std::size_t, ShaderAst::StructDescription*> structs;
		std::unordered_map<std::size_t, std::string> variableNames;
		std::vector<InOutField> inputFields;
		std::vector<InOutField> outputFields;
		Bitset<> declaredFunctions;
		const GlslWriter::BindingMapping& bindingMapping;
		PreVisitor previsitor;
		const States* states = nullptr;
		bool isInEntryPoint = false;
		unsigned int indentLevel = 0;
	};

	std::string GlslWriter::Generate(std::optional<ShaderStageType> shaderStage, const ShaderAst::Module& module, const BindingMapping& bindingMapping, const States& states)
	{
		State state(bindingMapping);
		state.stage = shaderStage;

		m_currentState = &state;
		CallOnExit onExit([this]()
		{
			m_currentState = nullptr;
		});

		ShaderAst::ModulePtr sanitizedModule;
		ShaderAst::Statement* targetAst;
		if (!states.sanitized)
		{
			sanitizedModule = Sanitize(module, states.optionValues);
			targetAst = sanitizedModule->rootNode.get();
		}
		else
			targetAst = module.rootNode.get();


		ShaderAst::StatementPtr optimizedAst;
		if (states.optimize)
		{
			ShaderAst::StatementPtr tempAst;

			ShaderAst::DependencyCheckerVisitor::Config dependencyConfig;
			if (shaderStage)
				dependencyConfig.usedShaderStages = *shaderStage;

			tempAst = ShaderAst::PropagateConstants(*targetAst);
			optimizedAst = ShaderAst::EliminateUnusedPass(*tempAst, dependencyConfig);

			targetAst = optimizedAst.get();
		}

		state.previsitor.selectedStage = shaderStage;
		targetAst->Visit(state.previsitor);

		AppendHeader();

		targetAst->Visit(*this);

		return state.stream.str();
	}

	void GlslWriter::SetEnv(Environment environment)
	{
		m_environment = std::move(environment);
	}

	const char* GlslWriter::GetFlipYUniformName()
	{
		return s_flipYUniformName;
	}

	ShaderAst::ModulePtr GlslWriter::Sanitize(const ShaderAst::Module& module, std::unordered_map<UInt32, ShaderAst::ConstantValue> optionValues, std::string* error)
	{
		// Always sanitize for reserved identifiers
		ShaderAst::SanitizeVisitor::Options options;
		options.optionValues = std::move(optionValues);
		options.makeVariableNameUnique = true;
		options.reduceLoopsToWhile = true;
		options.removeCompoundAssignments = false;
		options.removeConstDeclaration = true;
		options.removeOptionDeclaration = true;
		options.removeScalarSwizzling = true;
		options.reservedIdentifiers = {
			// All reserved GLSL keywords as of GLSL ES 3.2
			"active", "asm", "atomic_uint", "attribute", "bool", "break", "buffer", "bvec2", "bvec3", "bvec4", "case", "cast", "centroid", "class", "coherent", "common", "const", "continue", "default", "discard", "dmat2", "dmat2x2", "dmat2x3", "dmat2x4", "dmat3", "dmat3x2", "dmat3x3", "dmat3x4", "dmat4", "dmat4x2", "dmat4x3", "dmat4x4", "do", "double", "dvec2", "dvec3", "dvec4", "else", "enum", "extern", "external", "false", "filter", "fixed", "flat", "float", "for", "fvec2", "fvec3", "fvec4", "goto", "half", "highp", "hvec2", "hvec3", "hvec4", "if", "iimage1D", "iimage1DArray", "iimage2D", "iimage2DArray", "iimage2DMS", "iimage2DMSArray", "iimage2DRect", "iimage3D", "iimageBuffer", "iimageCube", "iimageCubeArray", "image1D", "image1DArray", "image2D", "image2DArray", "image2DMS", "image2DMSArray", "image2DRect", "image3D", "imageBuffer", "imageCube", "imageCubeArray", "in", "inline", "inout", "input", "int", "interface", "invariant", "isampler1D", "isampler1DArray", "isampler2D", "isampler2DArray", "isampler2DMS", "isampler2DMSArray", "isampler2DRect", "isampler3D", "isamplerBuffer", "isamplerCube", "isamplerCubeArray", "isubpassInput", "isubpassInputMS", "itexture2D", "itexture2DArray", "itexture2DMS", "itexture2DMSArray", "itexture3D", "itextureBuffer", "itextureCube", "itextureCubeArray", "ivec2", "ivec3", "ivec4", "layout", "long", "lowp", "mat2", "mat2x2", "mat2x3", "mat2x4", "mat3", "mat3x2", "mat3x3", "mat3x4", "mat4", "mat4x2", "mat4x3", "mat4x4", "mediump", "namespace", "noinline", "noperspective", "out", "output", "partition", "patch", "precise", "precision", "public", "readonly", "resource", "restrict", "return", "sample", "sampler", "sampler1D", "sampler1DArray", "sampler1DArrayShadow", "sampler1DShadow", "sampler2D", "sampler2DArray", "sampler2DArrayShadow", "sampler2DMS", "sampler2DMSArray", "sampler2DRect", "sampler2DRectShadow", "sampler2DShadow", "sampler3D", "sampler3DRect", "samplerBuffer", "samplerCube", "samplerCubeArray", "samplerCubeArrayShadow", "samplerCubeShadow", "samplerShadow", "shared", "short", "sizeof", "smooth", "static", "struct", "subpassInput", "subpassInputMS", "subroutine", "superp", "switch", "template", "texture2D", "texture2DArray", "texture2DMS", "texture2DMSArray", "texture3D", "textureBuffer", "textureCube", "textureCubeArray", "this", "true", "typedef", "uimage1D", "uimage1DArray", "uimage2D", "uimage2DArray", "uimage2DMS", "uimage2DMSArray", "uimage2DRect", "uimage3D", "uimageBuffer", "uimageCube", "uimageCubeArray", "uint", "uniform", "union", "unsigned", "usampler1D", "usampler1DArray", "usampler2D", "usampler2DArray", "usampler2DMS", "usampler2DMSArray", "usampler2DRect", "usampler3D", "usamplerBuffer", "usamplerCube", "usamplerCubeArray", "using", "usubpassInput", "usubpassInputMS", "utexture2D", "utexture2DArray", "utexture2DMS", "utexture2DMSArray", "utexture3D", "utextureBuffer", "utextureCube", "utextureCubeArray", "uvec2", "uvec3", "uvec4", "varying", "vec2", "vec3", "vec4", "void", "volatile", "while", "writeonly"
			// GLSL functions
			"cross", "dot", "exp", "length", "max", "min", "pow", "texture"
		};

		return ShaderAst::Sanitize(module, options, error);
	}

	void GlslWriter::Append(const ShaderAst::ArrayType& /*type*/)
	{
		throw std::runtime_error("unexpected ArrayType");
	}

	void GlslWriter::Append(ShaderAst::BuiltinEntry builtin)
	{
		switch (builtin)
		{
		case ShaderAst::BuiltinEntry::FragCoord:
			Append("gl_FragCoord");
			break;

		case ShaderAst::BuiltinEntry::FragDepth:
			Append("gl_FragDepth");
			break;

		case ShaderAst::BuiltinEntry::VertexPosition:
			Append("gl_Position");
			break;
		}
	}

	void GlslWriter::Append(const ShaderAst::ExpressionType& type)
	{
		std::visit([&](auto&& arg)
		{
			Append(arg);
		}, type);
	}

	void GlslWriter::Append(const ShaderAst::ExpressionValue<ShaderAst::ExpressionType>& type)
	{
		Append(type.GetResultingValue());
	}

	void GlslWriter::Append(const ShaderAst::FunctionType& /*functionType*/)
	{
		throw std::runtime_error("unexpected function type");
	}

	void GlslWriter::Append(const ShaderAst::IdentifierType& /*identifierType*/)
	{
		throw std::runtime_error("unexpected identifier type");
	}

	void GlslWriter::Append(const ShaderAst::IntrinsicFunctionType& /*intrinsicFunctionType*/)
	{
		throw std::runtime_error("unexpected intrinsic function type");
	}

	void GlslWriter::Append(const ShaderAst::MatrixType& matrixType)
	{
		if (matrixType.columnCount == matrixType.rowCount)
		{
			Append("mat");
			Append(matrixType.columnCount);
		}
		else
		{
			Append("mat");
			Append(matrixType.columnCount);
			Append("x");
			Append(matrixType.rowCount);
		}
	}

	void GlslWriter::Append(const ShaderAst::MethodType& /*methodType*/)
	{
		throw std::runtime_error("unexpected method type");
	}

	void GlslWriter::Append(ShaderAst::PrimitiveType type)
	{
		switch (type)
		{
			case ShaderAst::PrimitiveType::Boolean: return Append("bool");
			case ShaderAst::PrimitiveType::Float32: return Append("float");
			case ShaderAst::PrimitiveType::Int32:   return Append("int");
			case ShaderAst::PrimitiveType::UInt32:  return Append("uint");
			case ShaderAst::PrimitiveType::String:  throw std::runtime_error("unexpected string constant");
		}
	}

	void GlslWriter::Append(const ShaderAst::SamplerType& samplerType)
	{
		switch (samplerType.sampledType)
		{
			case ShaderAst::PrimitiveType::Boolean:
			case ShaderAst::PrimitiveType::Float32:
				break;

			case ShaderAst::PrimitiveType::Int32:   Append("i"); break;
			case ShaderAst::PrimitiveType::UInt32:  Append("u"); break;

			case ShaderAst::PrimitiveType::String:  throw std::runtime_error("unexpected string type");
		}

		Append("sampler");

		switch (samplerType.dim)
		{
			case ImageType::E1D:       Append("1D");      break;
			case ImageType::E1D_Array: Append("1DArray"); break;
			case ImageType::E2D:       Append("2D");      break;
			case ImageType::E2D_Array: Append("2DArray"); break;
			case ImageType::E3D:       Append("3D");      break;
			case ImageType::Cubemap:   Append("Cube");    break;
		}
	}

	void GlslWriter::Append(const ShaderAst::StructType& structType)
	{
		ShaderAst::StructDescription* structDesc = Retrieve(m_currentState->structs, structType.structIndex);
		Append(structDesc->name);
	}

	void GlslWriter::Append(const ShaderAst::Type& /*type*/)
	{
		throw std::runtime_error("unexpected Type");
	}

	void GlslWriter::Append(const ShaderAst::UniformType& /*uniformType*/)
	{
		throw std::runtime_error("unexpected UniformType");
	}

	void GlslWriter::Append(const ShaderAst::VectorType& vecType)
	{
		switch (vecType.type)
		{
			case ShaderAst::PrimitiveType::Boolean: Append("b"); break;
			case ShaderAst::PrimitiveType::Float32: break;
			case ShaderAst::PrimitiveType::Int32:   Append("i"); break;
			case ShaderAst::PrimitiveType::UInt32:  Append("u"); break;
			case ShaderAst::PrimitiveType::String:  throw std::runtime_error("unexpected string type");
		}

		Append("vec");
		Append(vecType.componentCount);
	}

	void GlslWriter::Append(ShaderAst::MemoryLayout layout)
	{
		switch (layout)
		{
			case ShaderAst::MemoryLayout::Std140:
				Append("std140");
				break;
		}
	}

	void GlslWriter::Append(ShaderAst::NoType)
	{
		return Append("void");
	}

	template<typename T>
	void GlslWriter::Append(const T& param)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << param;
	}

	template<typename T1, typename T2, typename... Args>
	void GlslWriter::Append(const T1& firstParam, const T2& secondParam, Args&&... params)
	{
		Append(firstParam);
		Append(secondParam, std::forward<Args>(params)...);
	}

	void GlslWriter::AppendComment(const std::string& section)
	{
		std::size_t lineFeed = section.find('\n');
		if (lineFeed != section.npos)
		{
			std::size_t previousCut = 0;

			AppendLine("/*");
			do
			{
				AppendLine(section.substr(previousCut, lineFeed - previousCut));
				previousCut = lineFeed + 1;
			} while ((lineFeed = section.find('\n', previousCut)) != section.npos);
			AppendLine(section.substr(previousCut));
			AppendLine("*/");
		}
		else
			AppendLine("// ", section);
	}

	void GlslWriter::AppendCommentSection(const std::string& section)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		std::string stars((section.size() < 33) ? (36 - section.size()) / 2 : 3, '*');
		m_currentState->stream << "/*" << stars << ' ' << section << ' ' << stars << "*/";
		AppendLine();
	}

	void GlslWriter::AppendFunctionDeclaration(const ShaderAst::DeclareFunctionStatement& node, bool forward)
	{
		Append(node.returnType, " ", node.name, "(");

		bool first = true;
		for (const auto& parameter : node.parameters)
		{
			if (!first)
				Append(", ");

			first = false;

			AppendVariableDeclaration(parameter.type.GetResultingValue(), parameter.name);
		}
		AppendLine((forward) ? ");" : ")");
	}

	void GlslWriter::AppendHeader()
	{
		unsigned int glslVersion;
		unsigned int glVersion = m_environment.glMajorVersion * 100 + m_environment.glMinorVersion * 10;
		if (m_environment.glES)
		{
			if (glVersion >= 300)
				glslVersion = glVersion;
			else if (m_environment.glMajorVersion >= 2)
				glslVersion = 100;
			else
				throw std::runtime_error("This version of OpenGL ES does not support shaders");
		}
		else
		{
			if (glVersion >= 330)
				glslVersion = glVersion;
			else if (glVersion >= 320)
				glslVersion = 150;
			else if (glVersion >= 310)
				glslVersion = 140;
			else if (glVersion >= 300)
				glslVersion = 130;
			else if (glVersion >= 210)
				glslVersion = 120;
			else if (glVersion >= 200)
				glslVersion = 110;
			else
				throw std::runtime_error("This version of OpenGL does not support shaders");
		}

		// Header
		Append("#version ");
		Append(glslVersion);
		if (m_environment.glES)
			Append(" es");

		AppendLine();
		AppendLine();

		// Extensions

		std::vector<std::string> requiredExtensions;

		if (!m_environment.glES && m_environment.extCallback)
		{
			// GL_ARB_shading_language_420pack (required for layout(binding = X))
			if (glslVersion < 420)
			{
				if (m_environment.extCallback("GL_ARB_shading_language_420pack"))
					requiredExtensions.emplace_back("GL_ARB_shading_language_420pack");
			}

			// GL_ARB_separate_shader_objects (required for layout(location = X))
			if (glslVersion < 410)
			{
				if (m_environment.extCallback("GL_ARB_separate_shader_objects"))
					requiredExtensions.emplace_back("GL_ARB_separate_shader_objects");
			}
		}

		if (!requiredExtensions.empty())
		{
			for (const std::string& ext : requiredExtensions)
				AppendLine("#extension " + ext + " : require");

			AppendLine();
		}

		if (m_environment.glES)
		{
			AppendLine("#if GL_FRAGMENT_PRECISION_HIGH");
			AppendLine("precision highp float;");
			AppendLine("#else");
			AppendLine("precision mediump float;");
			AppendLine("#endif");
			AppendLine();
		}
	}

	void GlslWriter::AppendLine(const std::string& txt)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->stream << txt << '\n' << std::string(m_currentState->indentLevel, '\t');
	}

	template<typename... Args>
	void GlslWriter::AppendLine(Args&&... params)
	{
		(Append(std::forward<Args>(params)), ...);
		AppendLine();
	}

	void GlslWriter::AppendStatementList(std::vector<ShaderAst::StatementPtr>& statements)
	{
		bool first = true;
		for (const ShaderAst::StatementPtr& statement : statements)
		{
			if (statement->GetType() == ShaderAst::NodeType::NoOpStatement)
				continue;

			if (!first)
				AppendLine();

			statement->Visit(*this);

			first = false;
		}
	}

	void GlslWriter::AppendVariableDeclaration(const ShaderAst::ExpressionType& varType, const std::string& varName)
	{
		if (ShaderAst::IsArrayType(varType))
		{
			std::vector<UInt32> lengths;

			const ShaderAst::ExpressionType* exprType = &varType;
			while (ShaderAst::IsArrayType(*exprType))
			{
				const auto& arrayType = std::get<ShaderAst::ArrayType>(*exprType);
				lengths.push_back(arrayType.length);

				exprType = &arrayType.containedType->type;
			}

			assert(!ShaderAst::IsArrayType(*exprType));
			Append(*exprType, " ", varName);

			for (UInt32 lengthAttribute : lengths)
				Append("[", lengthAttribute, "]");
		}
		else
			Append(varType, " ", varName);
	}

	void GlslWriter::EnterScope()
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->indentLevel++;
		AppendLine("{");
	}

	void GlslWriter::LeaveScope(bool skipLine)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		m_currentState->indentLevel--;
		AppendLine();

		if (skipLine)
			AppendLine("}");
		else
			Append("}");
	}

	void GlslWriter::HandleEntryPoint(ShaderAst::DeclareFunctionStatement& node)
	{
		if (node.entryStage.GetResultingValue() == ShaderStageType::Fragment && node.earlyFragmentTests.HasValue() && node.earlyFragmentTests.GetResultingValue())
		{
			unsigned int glVersion = m_environment.glMajorVersion * 100 + m_environment.glMinorVersion * 10;
			if ((m_environment.glES && glVersion >= 310) || (!m_environment.glES && glVersion >= 420) || (m_environment.extCallback && m_environment.extCallback("GL_ARB_shader_image_load_store")))
			{
				AppendLine("layout(early_fragment_tests) in;");
				AppendLine();
			}
		}

		HandleInOut();
		AppendLine("void main()");
		EnterScope();
		{
			if (!m_currentState->inputFields.empty())
			{
				assert(!node.parameters.empty());

				auto& parameter = node.parameters.front();
				const std::string& varName = parameter.name;
				RegisterVariable(*parameter.varIndex, varName);

				assert(IsStructType(parameter.type.GetResultingValue()));
				std::size_t structIndex = std::get<ShaderAst::StructType>(parameter.type.GetResultingValue()).structIndex;
				const ShaderAst::StructDescription* structDesc = Retrieve(m_currentState->structs, structIndex);

				AppendLine(structDesc->name, " ", varName, ";");
				for (const auto& [memberName, targetName] : m_currentState->inputFields)
					AppendLine(varName, ".", memberName, " = ", targetName, ";");

				AppendLine();
			}

			// Output struct is handled on return node
			m_currentState->isInEntryPoint = true;

			AppendStatementList(node.statements);

			m_currentState->isInEntryPoint = false;
		}
		LeaveScope();
	}

	void GlslWriter::HandleInOut()
	{
		auto AppendInOut = [this](const ShaderAst::StructDescription& structDesc, std::vector<State::InOutField>& fields, const char* keyword, const char* targetPrefix)
		{
			for (const auto& member : structDesc.members)
			{
				if (member.cond.HasValue() && !member.cond.GetResultingValue())
					continue;

				if (member.builtin.HasValue())
				{
					auto it = s_builtinMapping.find(member.builtin.GetResultingValue());
					assert(it != s_builtinMapping.end());

					const Builtin& builtin = it->second;
					if (m_currentState->stage && !builtin.stageFlags.Test(*m_currentState->stage))
						continue; //< This builtin is not active in this stage, skip it

					fields.push_back({
						member.name,
						builtin.identifier
					});
				}
				else if (member.locationIndex.HasValue())
				{
					Append("layout(location = ");
					Append(member.locationIndex.GetResultingValue());
					Append(") ", keyword, " ");
					AppendVariableDeclaration(member.type.GetResultingValue(), targetPrefix + member.name);
					AppendLine(";");

					fields.push_back({
						member.name,
						targetPrefix + member.name
					});
				}
			}
			AppendLine();
		};

		const ShaderAst::DeclareFunctionStatement& node = *m_currentState->previsitor.entryPoint;

		const ShaderAst::StructDescription* inputStruct = nullptr;

		if (!node.parameters.empty())
		{
			assert(node.parameters.size() == 1);
			auto& parameter = node.parameters.front();
			assert(std::holds_alternative<ShaderAst::StructType>(parameter.type.GetResultingValue()));

			std::size_t inputStructIndex = std::get<ShaderAst::StructType>(parameter.type.GetResultingValue()).structIndex;
			inputStruct = Retrieve(m_currentState->structs, inputStructIndex);

			AppendCommentSection("Inputs");
			AppendInOut(*inputStruct, m_currentState->inputFields, "in", s_inputPrefix);
		}

		if (m_currentState->stage == ShaderStageType::Vertex && m_environment.flipYPosition)
		{
			AppendLine("uniform float ", s_flipYUniformName, ";");
			AppendLine();
		}

		if (node.returnType.HasValue())
		{
			assert(std::holds_alternative<ShaderAst::StructType>(node.returnType.GetResultingValue()));
			std::size_t outputStructIndex = std::get<ShaderAst::StructType>(node.returnType.GetResultingValue()).structIndex;

			const ShaderAst::StructDescription* outputStruct = Retrieve(m_currentState->structs, outputStructIndex);

			AppendCommentSection("Outputs");
			AppendInOut(*outputStruct, m_currentState->outputFields, "out", s_outputPrefix);
		}
	}

	void GlslWriter::RegisterStruct(std::size_t structIndex, ShaderAst::StructDescription* desc)
	{
		assert(m_currentState->structs.find(structIndex) == m_currentState->structs.end());
		m_currentState->structs.emplace(structIndex, desc);
	}

	void GlslWriter::RegisterVariable(std::size_t varIndex, std::string varName)
	{
		assert(m_currentState->variableNames.find(varIndex) == m_currentState->variableNames.end());
		m_currentState->variableNames.emplace(varIndex, std::move(varName));
	}

	void GlslWriter::ScopeVisit(ShaderAst::Statement& node)
	{
		if (node.GetType() != ShaderAst::NodeType::ScopedStatement)
		{
			EnterScope();
			node.Visit(*this);
			LeaveScope(true);
		}
		else
			node.Visit(*this);
	}

	void GlslWriter::Visit(ShaderAst::ExpressionPtr& expr, bool encloseIfRequired)
	{
		bool enclose = encloseIfRequired && (GetExpressionCategory(*expr) != ShaderAst::ExpressionCategory::LValue);

		if (enclose)
			Append("(");

		expr->Visit(*this);

		if (enclose)
			Append(")");
	}

	void GlslWriter::Visit(ShaderAst::AccessIdentifierExpression& node)
	{
		Visit(node.expr, true);

		const ShaderAst::ExpressionType& exprType = GetExpressionType(*node.expr);
		assert(IsStructType(exprType));

		for (const std::string& identifier : node.identifiers)
			Append(".", identifier);
	}

	void GlslWriter::Visit(ShaderAst::AccessIndexExpression& node)
	{
		Visit(node.expr, true);

		const ShaderAst::ExpressionType& exprType = GetExpressionType(*node.expr);
		assert(!IsStructType(exprType));

		// Array access
		assert(node.indices.size() == 1);
		Append("[");
		Visit(node.indices.front());
		Append("]");
	}

	void GlslWriter::Visit(ShaderAst::AssignExpression& node)
	{
		node.left->Visit(*this);

		switch (node.op)
		{
			case ShaderAst::AssignType::Simple: Append(" = "); break;
			case ShaderAst::AssignType::CompoundAdd: Append(" += "); break;
			case ShaderAst::AssignType::CompoundDivide: Append(" /= "); break;
			case ShaderAst::AssignType::CompoundMultiply: Append(" *= "); break;
			case ShaderAst::AssignType::CompoundLogicalAnd: Append(" &&= "); break;
			case ShaderAst::AssignType::CompoundLogicalOr: Append(" ||= "); break;
			case ShaderAst::AssignType::CompoundSubtract: Append(" -= "); break;
		}

		node.right->Visit(*this);
	}

	void GlslWriter::Visit(ShaderAst::BinaryExpression& node)
	{
		Visit(node.left, true);

		switch (node.op)
		{
			case ShaderAst::BinaryType::Add:       Append(" + "); break;
			case ShaderAst::BinaryType::Subtract:  Append(" - "); break;
			case ShaderAst::BinaryType::Multiply:  Append(" * "); break;
			case ShaderAst::BinaryType::Divide:    Append(" / "); break;

			case ShaderAst::BinaryType::CompEq:    Append(" == "); break;
			case ShaderAst::BinaryType::CompGe:    Append(" >= "); break;
			case ShaderAst::BinaryType::CompGt:    Append(" > ");  break;
			case ShaderAst::BinaryType::CompLe:    Append(" <= "); break;
			case ShaderAst::BinaryType::CompLt:    Append(" < ");  break;
			case ShaderAst::BinaryType::CompNe:    Append(" != "); break;

			case ShaderAst::BinaryType::LogicalAnd: Append(" && "); break;
			case ShaderAst::BinaryType::LogicalOr:  Append(" || "); break;
		}

		Visit(node.right, true);
	}

	void GlslWriter::Visit(ShaderAst::CallFunctionExpression& node)
	{
		std::size_t functionIndex = std::get<ShaderAst::FunctionType>(GetExpressionType(*node.targetFunction)).funcIndex;
		const std::string& targetName = Retrieve(m_currentState->previsitor.functions, functionIndex).name;

		Append(targetName, "(");
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			if (i != 0)
				Append(", ");

			node.parameters[i]->Visit(*this);
		}
		Append(")");
	}

	void GlslWriter::Visit(ShaderAst::CastExpression& node)
	{
		Append(node.targetType);
		Append("(");

		bool first = true;
		for (const auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			if (!first)
				m_currentState->stream << ", ";

			exprPtr->Visit(*this);
			first = false;
		}

		Append(")");
	}

	void GlslWriter::Visit(ShaderAst::ConstantValueExpression& node)
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, Vector2i32> || std::is_same_v<T, Vector3i32> || std::is_same_v<T, Vector4i32>)
				Append("i"); //< for ivec

			if constexpr (std::is_same_v<T, ShaderAst::NoValue>)
				throw std::runtime_error("invalid type (value expected)");
			else if constexpr (std::is_same_v<T, std::string>)
				throw std::runtime_error("unexpected string litteral");
			else if constexpr (std::is_same_v<T, bool>)
				Append((arg) ? "true" : "false");
			else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, Int32>)
				Append(std::to_string(arg));
			else if constexpr (std::is_same_v<T, UInt32>)
				Append(std::to_string(arg), "u");
			else if constexpr (std::is_same_v<T, Vector2f> || std::is_same_v<T, Vector2i32>)
				Append("vec2(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ")");
			else if constexpr (std::is_same_v<T, Vector3f> || std::is_same_v<T, Vector3i32>)
				Append("vec3(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ")");
			else if constexpr (std::is_same_v<T, Vector4f> || std::is_same_v<T, Vector4i32>)
				Append("vec4(" + std::to_string(arg.x) + ", " + std::to_string(arg.y) + ", " + std::to_string(arg.z) + ", " + std::to_string(arg.w) + ")");
			else
				static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
		}, node.value);
	}
	
	
	void GlslWriter::Visit(ShaderAst::IntrinsicExpression& node)
	{
		switch (node.intrinsic)
		{
			case ShaderAst::IntrinsicType::CrossProduct:
				Append("cross");
				break;

			case ShaderAst::IntrinsicType::DotProduct:
				Append("dot");
				break;

			case ShaderAst::IntrinsicType::Exp:
				Append("exp");
				break;

			case ShaderAst::IntrinsicType::Length:
				Append("length");
				break;

			case ShaderAst::IntrinsicType::Max:
				Append("max");
				break;

			case ShaderAst::IntrinsicType::Min:
				Append("min");
				break;

			case ShaderAst::IntrinsicType::Normalize:
				Append("normalize");
				break;

			case ShaderAst::IntrinsicType::Pow:
				Append("pow");
				break;

			case ShaderAst::IntrinsicType::Reflect:
				Append("reflect");
				break;

			case ShaderAst::IntrinsicType::SampleTexture:
				Append("texture");
				break;
		}

		Append("(");
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			if (i != 0)
				Append(", ");

			node.parameters[i]->Visit(*this);
		}
		Append(")");
	}

	void GlslWriter::Visit(ShaderAst::SwizzleExpression& node)
	{
		Visit(node.expression, true);
		Append(".");

		const char* componentStr = "xyzw";
		for (std::size_t i = 0; i < node.componentCount; ++i)
			Append(componentStr[node.components[i]]);
	}

	void GlslWriter::Visit(ShaderAst::UnaryExpression& node)
	{
		switch (node.op)
		{
			case ShaderAst::UnaryType::LogicalNot:
				Append("!");
				break;

			case ShaderAst::UnaryType::Minus:
				Append("-");
				break;

			case ShaderAst::UnaryType::Plus:
				Append("+");
				break;
		}

		Visit(node.expression);
	}

	void GlslWriter::Visit(ShaderAst::VariableExpression& node)
	{
		const std::string& varName = Retrieve(m_currentState->variableNames, node.variableId);
		Append(varName);
	}


	void GlslWriter::Visit(ShaderAst::BranchStatement& node)
	{
		assert(!node.isConst);

		bool first = true;
		for (const auto& statement : node.condStatements)
		{
			if (!first)
				Append("else ");

			Append("if (");
			statement.condition->Visit(*this);
			AppendLine(")");

			ScopeVisit(*statement.statement);

			first = false;
		}

		if (node.elseStatement)
		{
			AppendLine("else");

			ScopeVisit(*node.elseStatement);
		}
	}

	void GlslWriter::Visit(ShaderAst::DeclareConstStatement& /*node*/)
	{
		/* nothing to do */
	}

	void GlslWriter::Visit(ShaderAst::DeclareExternalStatement& node)
	{
		for (const auto& externalVar : node.externalVars)
		{
			bool isStd140 = false;
			if (IsUniformType(externalVar.type.GetResultingValue()))
			{
				auto& uniform = std::get<ShaderAst::UniformType>(externalVar.type.GetResultingValue());
				ShaderAst::StructDescription* structInfo = Retrieve(m_currentState->structs, uniform.containedType.structIndex);
				if (structInfo->layout.HasValue())
					isStd140 = structInfo->layout.GetResultingValue() == StructLayout::Std140;
			}

			if (!m_currentState->bindingMapping.empty() || isStd140)
				Append("layout(");

			if (!m_currentState->bindingMapping.empty())
			{
				assert(externalVar.bindingIndex.HasValue());

				UInt64 bindingIndex = externalVar.bindingIndex.GetResultingValue();
				UInt64 bindingSet;
				if (externalVar.bindingSet.HasValue())
					bindingSet = externalVar.bindingSet.GetResultingValue();
				else
					bindingSet = 0;

				auto bindingIt = m_currentState->bindingMapping.find(bindingSet << 32 | bindingIndex);
				if (bindingIt == m_currentState->bindingMapping.end())
					throw std::runtime_error("no binding found for (set=" + std::to_string(bindingSet) + ", binding=" + std::to_string(bindingIndex) + ")");

				Append("binding = ", bindingIt->second);
				if (isStd140)
					Append(", ");
			}

			if (isStd140)
				Append("std140");

			if (!m_currentState->bindingMapping.empty() || isStd140)
				Append(") ");

			Append("uniform ");

			if (IsUniformType(externalVar.type.GetResultingValue()))
			{
				Append("_NzBinding_");
				AppendLine(externalVar.name);

				EnterScope();
				{
					auto& uniform = std::get<ShaderAst::UniformType>(externalVar.type.GetResultingValue());
					auto& structDesc = Retrieve(m_currentState->structs, uniform.containedType.structIndex);

					bool first = true;
					for (const auto& member : structDesc->members)
					{
						if (member.cond.HasValue() && !member.cond.GetResultingValue())
							continue;

						if (!first)
							AppendLine();

						first = false;

						AppendVariableDeclaration(member.type.GetResultingValue(), member.name);
						Append(";");
					}
				}
				LeaveScope(false);

				Append(" ");
				Append(externalVar.name);
			}
			else
				AppendVariableDeclaration(externalVar.type.GetResultingValue(), externalVar.name);

			AppendLine(";");

			if (IsUniformType(externalVar.type.GetResultingValue()))
				AppendLine();

			assert(externalVar.varIndex);
			RegisterVariable(*externalVar.varIndex, externalVar.name);
		}
	}

	void GlslWriter::Visit(ShaderAst::DeclareFunctionStatement& node)
	{
		NazaraAssert(m_currentState, "This function should only be called while processing an AST");

		if (node.entryStage.HasValue() && m_currentState->previsitor.entryPoint != &node)
			return; //< Ignore other entry points

		assert(node.funcIndex);
		auto& funcData = Retrieve(m_currentState->previsitor.functions, node.funcIndex.value());

		// Declare functions called by this function which aren't already defined
		bool hasPredeclaration = false;
		for (std::size_t i = funcData.calledFunctions.FindFirst(); i != funcData.calledFunctions.npos; i = funcData.calledFunctions.FindNext(i))
		{
			if (!m_currentState->declaredFunctions.UnboundedTest(i))
			{
				hasPredeclaration = true;

				auto& targetFunc = Retrieve(m_currentState->previsitor.functions, i);
				AppendFunctionDeclaration(*targetFunc.node, true);

				m_currentState->declaredFunctions.UnboundedSet(i);
			}
		}

		if (hasPredeclaration)
			AppendLine();

		if (node.entryStage.HasValue())
			return HandleEntryPoint(node);

		for (const auto& parameter : node.parameters)
		{
			assert(parameter.varIndex);
			RegisterVariable(*parameter.varIndex, parameter.name);
		}

		AppendFunctionDeclaration(node);
		EnterScope();
		{
			AppendStatementList(node.statements);
		}
		LeaveScope();

		m_currentState->declaredFunctions.UnboundedSet(node.funcIndex.value());
	}

	void GlslWriter::Visit(ShaderAst::DeclareOptionStatement& /*node*/)
	{
		/* nothing to do */
	}

	void GlslWriter::Visit(ShaderAst::DeclareStructStatement& node)
	{
		assert(node.structIndex);
		RegisterStruct(*node.structIndex, &node.description);

		Append("struct ");
		AppendLine(node.description.name);
		EnterScope();
		{
			bool first = true;
			for (const auto& member : node.description.members)
			{
				if (member.cond.HasValue() && !member.cond.GetResultingValue())
					continue;

				if (!first)
					AppendLine();

				first = false;

				AppendVariableDeclaration(member.type.GetResultingValue(), member.name);
				Append(";");
			}

			// Empty structs are not allowed in GLSL
			if (first)
				AppendLine("int dummy;");
		}
		LeaveScope(false);
		AppendLine(";");
	}

	void GlslWriter::Visit(ShaderAst::DeclareVariableStatement& node)
	{
		assert(node.varIndex);
		RegisterVariable(*node.varIndex, node.varName);

		AppendVariableDeclaration(node.varType.GetResultingValue(), node.varName);
		if (node.initialExpression)
		{
			Append(" = ");
			node.initialExpression->Visit(*this);
		}

		Append(";");
	}

	void GlslWriter::Visit(ShaderAst::DiscardStatement& /*node*/)
	{
		Append("discard;");
	}

	void GlslWriter::Visit(ShaderAst::ExpressionStatement& node)
	{
		node.expression->Visit(*this);
		Append(";");
	}

	void GlslWriter::Visit(ShaderAst::ImportStatement& /*node*/)
	{
		/* nothing to do */
	}

	void GlslWriter::Visit(ShaderAst::MultiStatement& node)
	{
		if (!node.sectionName.empty())
			AppendComment(node.sectionName);

		AppendStatementList(node.statements);

		if (!node.sectionName.empty())
			AppendComment("End: " + node.sectionName);
	}

	void GlslWriter::Visit(ShaderAst::NoOpStatement& /*node*/)
	{
		/* nothing to do */
	}

	void GlslWriter::Visit(ShaderAst::ReturnStatement& node)
	{
		if (m_currentState->isInEntryPoint)
		{
			assert(node.returnExpr);

			const ShaderAst::ExpressionType& returnType = GetExpressionType(*node.returnExpr);
			assert(IsStructType(returnType));
			std::size_t structIndex = std::get<ShaderAst::StructType>(returnType).structIndex;
			const ShaderAst::StructDescription* structDesc = Retrieve(m_currentState->structs, structIndex);

			std::string outputStructVarName;
			if (node.returnExpr->GetType() == ShaderAst::NodeType::VariableExpression)
				outputStructVarName = Retrieve(m_currentState->variableNames, static_cast<ShaderAst::VariableExpression&>(*node.returnExpr).variableId);
			else
			{
				AppendLine();
				Append(structDesc->name, " ", s_outputVarName, " = ");
				node.returnExpr->Visit(*this);
				AppendLine(";");

				outputStructVarName = s_outputVarName;
			}

			AppendLine();

			for (const auto& [name, targetName] : m_currentState->outputFields)
			{
				bool isOutputPosition = (m_currentState->stage == ShaderStageType::Vertex && targetName == "gl_Position");

				AppendLine(targetName, " = ", outputStructVarName, ".", name, ";");
				if (isOutputPosition)
				{
					// https://veldrid.dev/articles/backend-differences.html
					if (m_environment.flipYPosition)
						AppendLine(targetName, ".y *= ", s_flipYUniformName, ";");

					if (m_environment.remapZPosition)
						AppendLine(targetName, ".z = ", targetName, ".z * 2.0 - ", targetName, ".w; ");
				}
			}

			Append("return;"); //< TODO: Don't return if it's the last statement of the function
		}
		else
		{
			if (node.returnExpr)
			{
				Append("return ");
				node.returnExpr->Visit(*this);
				Append(";");
			}
			else
				Append("return;");
		}
	}

	void GlslWriter::Visit(ShaderAst::ScopedStatement& node)
	{
		EnterScope();
		node.statement->Visit(*this);
		LeaveScope(true);
	}

	void GlslWriter::Visit(ShaderAst::WhileStatement& node)
	{
		Append("while (");
		node.condition->Visit(*this);
		AppendLine(")");

		ScopeVisit(*node.body);
	}

	bool GlslWriter::HasExplicitBinding(ShaderAst::StatementPtr& shader)
	{
		/*for (const auto& uniform : shader.GetUniforms())
		{
			if (uniform.bindingIndex.has_value())
				return true;
		}*/

		return false;
	}

	bool GlslWriter::HasExplicitLocation(ShaderAst::StatementPtr& shader)
	{
		/*for (const auto& input : shader.GetInputs())
		{
			if (input.locationIndex.has_value())
				return true;
		}

		for (const auto& output : shader.GetOutputs())
		{
			if (output.locationIndex.has_value())
				return true;
		}*/

		return false;
	}
}
