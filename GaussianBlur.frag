uniform sampler2D 	source;
uniform vec2 		offsetFactor;

const float Threshold = 0.45;//0.7;
const float Factor   = 3.0;

void main()
{
	vec2 textureCoordinates = gl_TexCoord[0].xy;
	vec4 color = vec4(0.0);
	color += texture2D(source, textureCoordinates - 4.0 * offsetFactor) * 0.0162162162;
	color += texture2D(source, textureCoordinates - 3.0 * offsetFactor) * 0.0540540541;
	color += texture2D(source, textureCoordinates - 2.0 * offsetFactor) * 0.1216216216;
	color += texture2D(source, textureCoordinates - offsetFactor) * 0.1945945946;
	color += texture2D(source, textureCoordinates) * 0.2270270270;
	color += texture2D(source, textureCoordinates + offsetFactor) * 0.1945945946;
	color += texture2D(source, textureCoordinates + 2.0 * offsetFactor) * 0.1216216216;
	color += texture2D(source, textureCoordinates + 3.0 * offsetFactor) * 0.0540540541;
	color += texture2D(source, textureCoordinates + 4.0 * offsetFactor) * 0.0162162162;

	//float luminance = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
	float luminance = (color.r + color.g + color.b)/3.0;
	color *= clamp(luminance - Threshold, 0.0, 1.0) * Factor;

	vec4 sourceColor = texture2D(source, textureCoordinates);

	gl_FragColor = color + sourceColor;
}