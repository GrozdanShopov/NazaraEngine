#version 110

attribute vec3 Position;
attribute vec2 TexCoord0;

uniform mat4 WorldViewProjMatrix;

varying vec2 vTexCoord;

void main()
{
    gl_Position = WorldViewProjMatrix * vec4(Position, 1.0);
	vTexCoord = TexCoord0;
}
