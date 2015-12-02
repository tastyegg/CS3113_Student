
uniform sampler2D diffuse;
varying vec2 texCoordVar;
varying vec4 vertexColor;

void main() {
    vec4 texColor = texture2D(diffuse, texCoordVar);
	texColor.r *= vertexColor.r;
	texColor.g *= vertexColor.g;
	texColor.b *= vertexColor.b;
	texColor.a *= vertexColor.a;
	gl_FragColor = texColor;
}