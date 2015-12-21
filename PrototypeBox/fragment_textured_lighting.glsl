
uniform sampler2D diffuse;
uniform vec2 lightPositions[16];

varying vec2 texCoordVar;
varying vec4 vertexColor;
varying vec2 varPosition;

float attenuate(float dist, float a, float b) {
	return 1.0 / (1.0 + a*dist + b*dist*dist);
}

void main() {
	float brightness = 0.0;
	
	for (int i = 0; i < 8; i++) {
		brightness += attenuate(distance(lightPositions[i], varPosition), 0.0, 24.0);
    }
	brightness = min(brightness, 1.0);

	vec4 texColor = texture2D(diffuse, texCoordVar);
	texColor.r *= vertexColor.r;
	texColor.g *= vertexColor.g;
	texColor.b *= vertexColor.b;
	texColor.a *= vertexColor.a;
	gl_FragColor = texColor * brightness;
	gl_FragColor.a = texColor.a;
}