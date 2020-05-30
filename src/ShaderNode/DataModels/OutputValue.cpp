#include <ShaderNode/DataModels/OutputValue.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>

OutputValue::OutputValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_onOutputListUpdateSlot.Connect(GetGraph().OnOutputListUpdate, [&](ShaderGraph*) { OnOutputListUpdate(); });
	m_onOutputUpdateSlot.Connect(GetGraph().OnOutputUpdate, [&](ShaderGraph*, std::size_t inputIndex)
	{
		if (m_currentOutputIndex == inputIndex)
			UpdatePreview();
	});

	if (graph.GetOutputCount() > 0)
	{
		auto& firstOutput = graph.GetOutput(0);
		m_currentOutputIndex = 0;
		m_currentOutputText = firstOutput.name;
	}

	EnablePreview();
	SetPreviewSize({ 128, 128 });
	DisableCustomVariableName();
}

void OutputValue::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QComboBox* outputSelection = new QComboBox;

	connect(outputSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentOutputIndex = static_cast<std::size_t>(index);
		else
			m_currentOutputIndex.reset();

		UpdatePreview();
	});

	for (const auto& outputEntry : GetGraph().GetOutputs())
		outputSelection->addItem(QString::fromStdString(outputEntry.name));

	layout->addRow(tr("Output"), outputSelection);
}

Nz::ShaderAst::ExpressionPtr OutputValue::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	using namespace Nz::ShaderAst;
	using namespace Nz::ShaderBuilder;

	assert(count == 1);

	if (!m_currentOutputIndex)
		throw std::runtime_error("no output");

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);

	Nz::ShaderAst::ExpressionType expression = [&]
	{
		switch (outputEntry.type)
		{
			case InOutType::Bool:   return Nz::ShaderAst::ExpressionType::Boolean;
			case InOutType::Float1: return Nz::ShaderAst::ExpressionType::Float1;
			case InOutType::Float2: return Nz::ShaderAst::ExpressionType::Float2;
			case InOutType::Float3: return Nz::ShaderAst::ExpressionType::Float3;
			case InOutType::Float4: return Nz::ShaderAst::ExpressionType::Float4;
		}

		assert(false);
		throw std::runtime_error("Unhandled output type");
	}();

	auto output = Nz::ShaderBuilder::Output(outputEntry.name, expression);

	return Nz::ShaderBuilder::Assign(std::move(output), *expressions);
}

QtNodes::NodeDataType OutputValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::In);
	assert(portIndex == 0);

	if (!m_currentOutputIndex)
		return Vec4Data::Type();

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	switch (outputEntry.type)
	{
		//case InOutType::Bool:   return Nz::ShaderAst::ExpressionType::Boolean;
		//case InOutType::Float1: return Nz::ShaderAst::ExpressionType::Float1;
		case InOutType::Float2: return Vec2Data::Type();
		case InOutType::Float3: return Vec3Data::Type();
		case InOutType::Float4: return Vec4Data::Type();
	}

	assert(false);
	throw std::runtime_error("Unhandled output type");
}

unsigned int OutputValue::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return 0;
	}

	return 0;
}

std::shared_ptr<QtNodes::NodeData> OutputValue::outData(QtNodes::PortIndex /*port*/)
{
	return {};
}

void OutputValue::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0);
	if (value)
	{
		assert(dynamic_cast<Vec4Data*>(value.get()) != nullptr);
		m_input = std::static_pointer_cast<Vec4Data>(value);
	}
	else
		m_input.reset();

	UpdatePreview();
}

bool OutputValue::ComputePreview(QPixmap& pixmap)
{
	if (!m_input)
		return false;

	pixmap = QPixmap::fromImage(m_input->preview);
	return true;
}

void OutputValue::OnOutputListUpdate()
{
	m_currentOutputIndex.reset();

	std::size_t inputIndex = 0;
	for (const auto& inputEntry : GetGraph().GetOutputs())
	{
		if (inputEntry.name == m_currentOutputText)
		{
			m_currentOutputIndex = inputIndex;
			break;
		}

		inputIndex++;
	}
}
