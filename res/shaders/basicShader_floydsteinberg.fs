#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;

uniform vec4 lightColor;
uniform sampler2D sampler;
uniform vec4 lightDirection;

out vec4 fragColor;
void main()
{
	vec4 texColor = texture(sampler, texCoord0);
    float grayscale = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
    float quantized = floor(grayscale * 16.0) / 16.0;
    fragColor = vec4(vec3(quantized), 1.0);
}