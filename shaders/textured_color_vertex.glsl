attribute vec4 position;
attribute vec4 color;
attribute vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec2 texCoordVar;
varying vec4 colorVar;

void main()
{
	vec4 p = viewMatrix * modelMatrix  * position;
	texCoordVar = texCoord;
	colorVar = color;
	gl_Position = projectionMatrix * p;
}
