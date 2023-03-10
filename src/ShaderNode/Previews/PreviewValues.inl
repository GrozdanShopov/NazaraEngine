#include <ShaderNode/Previews/PreviewValues.hpp>

inline nzsl::Vector4f32* PreviewValues::GetData()
{
	return m_values.data();
}

inline const nzsl::Vector4f32* PreviewValues::GetData() const
{
	return m_values.data();
}

inline std::size_t PreviewValues::GetHeight() const
{
	return m_height;
}

inline std::size_t PreviewValues::GetWidth() const
{
	return m_width;
}
