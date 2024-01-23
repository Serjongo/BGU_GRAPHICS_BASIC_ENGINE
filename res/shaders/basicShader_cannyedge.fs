#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

//uniform vec4 lightColor;
uniform sampler2D sampler;
//uniform vec4 lightDirection;


out vec4 fragColor;
void main()
{
	const float resX= 512.0;
	const float resY= 512.0;
	vec2 texelSize = 1.0/vec2(resX,resY); //texture pixel size
	vec3 colorSum = vec3(0.0);

	//constants for gaussian blur, notice that it is hardcoded for 3x3
	float kernel[9] = float[9](
		1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
		2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
		1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
	);
	//canny edge detector steps:
	//1. gaussian blur - hardcoded for 3 by 3
	
	int kernel_index = 0; //to iterate over the kernel
	for(int i = -1; i <= 1; i++) // -1, 0, 1 cells, with 0 being center
	{
		for(int j = -1; j <= 1; j++,kernel_index++)
		{
			colorSum += texture(sampler, texCoord0 + vec2(float(i),float(j)) * texelSize).rgb * kernel[kernel_index];
		}
	}
	//kernel size
	fragColor = vec4(colorSum,1.0); //you must have gl_FragColor


	//2. intensity gradient
	float sobelX[9] = float[9]
	(
		-1.0 , 0.0 , +1.0,
		-2.0 , 0.0 , +2.0,
		-1.0 , 0.0 , +1.0
	);
	float sobelY[9] = float[9]
	(
		-1.0 , -2.0 , -1.0,
		0.0 , 0.0 , 0.0,
		+1.0 , +2.0 , +1.0
	);

	int sobel_index = 0; //to iterate over the kernel
	for(int i = -1; i <= 1; i++) // -1, 0, 1 cells, with 0 being center
	{
		for(int j = -1; j <= 1; j++,kernel_index++)
		{
			colorSum += texture(sampler, texCoord0 + vec2(float(i),float(j)) * texelSize).rgb * kernel[kernel_index];
		}
	}
	




}