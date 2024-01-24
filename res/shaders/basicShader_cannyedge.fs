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
	//NOTE FOR SELF - DO NOTE THAT I'M USING VEC3 FOR GX AND GY, CONSIDERING MAYBE CHANGING THEM TO FLOAT
	//AND PROCESS THE IMAGE IN A SINGLE CHANNEL, FOR NOW I KEEP IT AS IS
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
	vec3 Gx = vec3(0,0,0);
	vec3 Gy = vec3(0,0,0);
	for(int i = -1; i <= 1; i++) // -1, 0, 1 cells, with 0 being center
	{
		for(int j = -1; j <= 1; j++,sobel_index++)
		{
			Gx += texture(sampler, texCoord0 + vec2(float(i),float(j)) * texelSize).rgb * sobelX[sobel_index];
		}
	}
	sobel_index = 0;
	for(int i = -1; i <= 1; i++) // -1, 0, 1 cells, with 0 being center
	{
		for(int j = -1; j <= 1; j++,sobel_index++)
		{
			Gy += texture(sampler, texCoord0 + vec2(float(i),float(j)) * texelSize).rgb * sobelY[sobel_index];
		}
	}
	float magnitude = sqrt(dot(Gx.x,Gx.x)+dot(Gy.y,Gy.y)); //should probably change it to Gy.x too.
	fragColor = vec4(vec3(magnitude),1.0); //you must have gl_FragColor

	//3. Non-maximum suppression to thin the edges
	float gradient_direction_radians = atan(Gy.y,Gx.x);
	float pi = 3.14159;
	float gradient_direction_degrees = (gradient_direction_radians * 180 / pi); // % 360 modulo of 360 degrees
	vec2 neighbour_1 = vec2(0,0);
	vec2 neighbour_2 = vec2(0,0); //this will always be the negative of neighbour 1
	float current_fragment = vec4(vec3(magnitude),1.0).r; // I know that this makes little sense since I could simply equate it to magnitude, but I made it a point to copy the latest result of fragColor

	//checking for the gradient direction to take neighbours to compare my pixel to
	if ((gradient_direction_degrees >= -22.5) && (gradient_direction_degrees <= 22.5) )
	{
		neighbour_1 = vec2(1,0);
	} 
	else if ((gradient_direction_degrees > 22.5) && (gradient_direction_degrees <= 67.5) )
	{
		neighbour_1 = vec2(1,1);
	} 
	else if ((gradient_direction_degrees > 67.5) && (gradient_direction_degrees <= 112.5) )
	{
		neighbour_1 = vec2(0,1);
	}
	else if ((gradient_direction_degrees > 112.5) && (gradient_direction_degrees <= 157.5) )
	{
		neighbour_1 = vec2(-1,1);
	}
	//technically it can only go up to 180 but whatever
	else if ((gradient_direction_degrees > 157.5) && (gradient_direction_degrees <= 202.5) ) 
	{
		neighbour_1 = vec2(-1,0);
	}
	//going to the negatives
	else if ((gradient_direction_degrees < -22.5) && (gradient_direction_degrees >= -67.5) )
	{
		neighbour_1 = vec2(1,-1);
	}
	else if ((gradient_direction_degrees < -67.5) && (gradient_direction_degrees >= -112.5) )
	{
		neighbour_1 = vec2(0,-1);
	}
	else if ((gradient_direction_degrees < -112.5) && (gradient_direction_degrees >= -157.5) )
	{
		neighbour_1 = vec2(-1,-1);
	}
	else if ((gradient_direction_degrees < -157.5) && (gradient_direction_degrees >= -202.5) )
	{
		neighbour_1 = vec2(-1,0);
	}
	neighbour_2 = -neighbour_1;

	if ((current_fragment < texture(sampler, texCoord0 + neighbour_1 * texelSize).r) 
	|| (current_fragment < texture(sampler, texCoord0 + neighbour_2 * texelSize).r) )
	{
		current_fragment = 0.0;
		//fragColor = vec4(vec3(texture(sampler, texCoord0).r),1.0);
	}
	fragColor = vec4(vec3(current_fragment),1.0);

	//4.Double thresholding
	float high_threshold = 0.6; // WHITE edge. may change these values
	float low_threshold = 0.4;	// GRAY edge
	float threshold_curr_fragment = 0.0;

	if(magnitude >= high_threshold)
	{
		threshold_curr_fragment = 1.0;
	}
	else if(magnitude >= low_threshold) //weak edge
	{
		threshold_curr_fragment = 0.5; // weak edge val, hardcoded
	}

	fragColor = vec4(vec3(threshold_curr_fragment),1.0);


	//5. Hysteresis - looking to integrate our weak edges to our strong edges
	//since we are only doing a small, static picture, I will not bother with attempting to optimize this one by marking out traversed pixels

	if (threshold_curr_fragment == 0.5) //meaning we are a weak edge
	{
		for(int i = -1; i <= 1; i++) // -1, 0, 1 cells, with 0 being center
		{
			for(int j = -1; j <= 1; j++)
			{
				//do note that we're pointlessely going over 0,0 which is ourselves. It does not affect us
				if(texture(sampler, texCoord0 + vec2(float(i),float(j)) * texelSize).r >= high_threshold) //checking if hysteresis neighbour is a strong edge
				{
					fragColor = vec4(vec3(1.0),1.0);
				}
			}
		}
		//somewhat of a spaghetti code, but essentially, if we haven't been made strong inside the loop - we're not a strong edge neighbour, so we paint ourselves black
		if(fragColor != vec4(vec3(1.0),1.0))
		{
			fragColor = vec4(vec3(0.0),1.0);
		}
	}

}