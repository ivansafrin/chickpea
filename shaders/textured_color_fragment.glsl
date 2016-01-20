
uniform sampler2D diffuse;
varying vec2 texCoordVar;
varying vec4 colorVar;

void main() {
	vec4 base = texture2D(diffuse, texCoordVar);
	gl_FragColor = base * colorVar;
}
