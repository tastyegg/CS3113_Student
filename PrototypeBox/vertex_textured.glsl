attribute vec4 position;
attribute vec4 color;
attribute vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec2 texCoordVar;
varying vec4 vertexColor;
varying vec2 varPosition;

void main()
{
	vec4 p = viewMatrix * modelMatrix  * position;
    texCoordVar = texCoord;
    vertexColor = color;
	varPosition = vec2(p.x, p.y);
	gl_Position = projectionMatrix * p;
}