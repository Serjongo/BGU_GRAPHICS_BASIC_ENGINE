//#define GLEW_STATIC
// #define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"
#include "stb_image.h"
#include "../res/includes/glad/include/glad/glad.h"
#include <iostream>
#include <fstream> //added by me

Texture::Texture(const std::string& fileName)
{
	int width, height, numComponents;
    unsigned char* data = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4);
	
    if(data == NULL)
		std::cerr << "Unable to load texture: " << fileName << std::endl;
        
    glGenTextures(1, &m_texture);
    Bind(m_texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_LOD_BIAS,-0.4f);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}

//wrote this one, it simply clamps our values between 0 - 255
unsigned char clamp(unsigned char val) 
{
    if (val < 0) return 0;
    else if (val > 255) return 255;
    else return val;
}

//s:custom constructor for our filters, ADDED.
Texture::Texture(const std::string& fileName,int filter_num)
{
    int width, height, numComponents;
    unsigned char* data = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4);

    if (data == NULL)
        std::cerr << "Unable to load texture: " << fileName << std::endl;

    glGenTextures(1, &m_texture);
    Bind(m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(filter_num == 1) //floyd-steinberg
    {
        //EXTREMELY INNEFICIENT CODE - but I'm working on a static image so whatever
        //grayscale, REALLY IRRELEVANT, just did it to test other pictures
        for (int i = 0 ; i < width ; i++)
        {
            for (int j = 0; j < height; j++)
            {
                //the first colour value will be copied to all of them, except for the 4th byte which is alpha, although honestly I'm not sure we even use it lol
                data[(i*4) + (j * 4 * width)] = 0.2126 * data[(i * 4) + (j * 4 * width)] + 0.7152 * data[(i * 4) + 1 + (j * 4 * width)] + 0.0722 * data[(i * 4) + 2 + (j * 4 * width)];
                data[((i * 4) + 1) + (j * 4 * width)] = data[(i * 4) + (j * 4 * width)];
                data[((i * 4) + 2) + (j * 4 * width)] = data[(i * 4) + (j * 4 * width)];
                data[((i * 4) + 3) + (j * 4 * width)] = 0;
            }

        }

        //now we're grayscale with 256 colours, let's diwndle the spectrum to 16 and spread the error as per floyd steinberg
        //colour quantization innit
        unsigned char error;
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++) //[x,y]
            {
                // --- ONE PIXEL START 
                error = data[(i * 4) + (j * 4 * width)] % 16;
                data[(i * 4) + (j * 4 * width)] = clamp(data[(i * 4) + (j * 4 * width)] - error); //quantized color, closest multiplication of 16 smaller than n
                //data[(i * 4) + (j * 4 * width)] =  + data[(i * 4) + (j * 4 * width)] % 16;
                data[((i * 4) + 1) + (j * 4 * width)] = data[(i * 4) + (j * 4 * width)];
                data[((i * 4) + 2) + (j * 4 * width)] = data[(i * 4) + (j * 4 * width)];
                data[((i * 4) + 3) + (j * 4 * width)] = 255;
                // ONE PIXEL END ---

                if (i < width-1) //[x+1,y]
                {
                    // --- 7/16 PIXEL START
                    //int error = data[(i * 4) + (j * 4 * width)] % 16;
                    data[((i + 1) * 4) + (j * 4 * width)] = clamp(data[((i + 1) * 4) + (j * 4 * width)] + (error * 7/16)); //neighbour R
                    //data[(i * 4) + (j * 4 * width)] =  + data[(i * 4) + (j * 4 * width)] % 16;
                    data[(((i + 1) * 4) + 1) + (j * 4 * width)] = data[((i + 1) * 4) + (j * 4 * width)]; //neighbour G
                    data[(((i + 1) * 4) + 2) + (j * 4 * width)] = data[((i + 1) * 4) + (j * 4 * width)]; // neighbour B
                    data[(((i + 1) * 4) + 3) + (j * 4 * width)] = 255; //neighbour alpha
                    // 7/16 PIXEL END ---
                }


                if (i > 0 && j < height-1) //[x-1,y+1]
                {
                    // --- 3/16 PIXEL START
                    //int error = data[(i * 4) + (j * 4 * width)] % 16;
                    data[(((i - 1) * 4)) + (((j + 1) * 4) * width)] = clamp(data[(((i - 1) * 4)) + (((j + 1) * 4) * width)] + (error * 3 / 16)); //quantized color, closest multiplication of 16 smaller than n
                    //data[(i * 4) + (j * 4 * width)] =  + data[(i * 4) + (j * 4 * width)] % 16;
                    data[(((i - 1) * 4) + 1) + ((j + 1) * 4 * width)] = data[(((i - 1) * 4)) + (((j + 1) * 4) * width)];
                    data[(((i - 1) * 4) + 2) + ((j + 1) * 4 * width)] = data[(((i - 1) * 4)) + (((j + 1) * 4) * width)];
                    data[(((i - 1) * 4) + 3) + ((j + 1) * 4 * width)] = 255;
                    // 7/16 PIXEL END ---
                }



                if (j < height-1) //[x,y+1]
                {
                    // --- 5/16 PIXEL START
                    //int error = data[(i * 4) + (j * 4 * width)] % 16;
                    data[(i * 4) + ((j + 1) * 4 * width)] = clamp(data[(i * 4) + ((j + 1) * 4 * width)] + (error * 5 / 16)); //quantized color, closest multiplication of 16 smaller than n
                    //data[(i * 4) + (j * 4 * width)] =  + data[(i * 4) + (j * 4 * width)] % 16;
                    data[((i * 4) + 1) + ((j + 1) * 4 * width)] = data[(i * 4) + ((j + 1) * 4 * width)];
                    data[((i * 4) + 2) + ((j + 1) * 4 * width)] = data[(i * 4) + ((j + 1) * 4 * width)];
                    data[((i * 4) + 3) + ((j + 1) * 4 * width)] = 255;
                    // 5/16 PIXEL END ---
                }
                if (i < width-1 && j < height-1) //[x+1,y+1]
                {
                    // --- 1/16 PIXEL START
                    //int error = data[(i * 4) + (j * 4 * width)] % 16;
                    data[((i + 1) * 4) + ((j + 1) * 4 * width)] = clamp(data[((i + 1) * 4) + ((j + 1) * 4 * width)] + (error * 1 / 16)); //quantized color, closest multiplication of 16 smaller than n
                    //data[(i * 4) + (j * 4 * width)] =  + data[(i * 4) + (j * 4 * width)] % 16;
                    data[(((i + 1) * 4) + 1) + ((j + 1) * 4 * width)] = data[((i + 1) * 4) + ((j + 1) * 4 * width)];
                    data[(((i + 1) * 4) + 2) + ((j + 1) * 4 * width)] = data[((i + 1) * 4) + ((j + 1) * 4 * width)];
                    data[(((i + 1) * 4) + 3) + ((j + 1) * 4 * width)] = 255;
                    // 1/16 PIXEL END ---
                }
            }
        }


        //printing our results
        std::ofstream myfile;
        myfile.open("floyd_steinberg.txt");
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++) //[x,y]
            {
                myfile << int(data[(i * 4) + (j * 4 * width)]) << " "; //I'm always printing out the R value of the RGBA, they should be the same though
            }
            myfile << "\n";
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else if (filter_num == 2) //half-tone
    {
        //enlarging the image, making it twice the size, with each pixel repeating 4 times
        int new_width = width * 2;
        int new_height = height * 2;
        unsigned char* new_data = (unsigned char*)calloc(new_width * new_height * 4,sizeof(unsigned char)); //this will store our new image, it probably shouldn't be done that way in cpp tho lol
        for (int i = 0; i < (new_width); i = i + 2) //replicating each pixel 4 times in a square configuration
        {
            for (int j = 0; j < (new_height); j = j + 2) 
            {
                //pixel 1 [x,y]
                new_data[(i * 4) + (j * 4 * new_width)] = data[((i/2) * 4) + ((j / 2) * 4 * width)]; //R
                new_data[(i * 4) + 1 + (j * 4 * new_width)] = data[((i / 2) * 4) + ((j/2) * 4 * width)]; //G
                new_data[(i * 4) + 2 + (j * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //B
                new_data[(i * 4) + 3 + (j * 4 * new_width)] = 255; //alpha

                //pixel 2 [x+1,y]
                new_data[((i + 1) * 4) + (j * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //R
                new_data[(((i + 1) * 4) + 1) + (j * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //G
                new_data[(((i + 1) * 4) + 2) + (j * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //B
                new_data[(((i + 1) * 4) + 3) + (j * 4 * new_width)] = 255; //alpha

                //pixel 3 [x,y+1]
                new_data[(i * 4) + ((j + 1) * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //R
                new_data[((i * 4) + 1) + ((j + 1) * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //G
                new_data[((i * 4) + 2) + ((j + 1) * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //B
                new_data[((i * 4) + 3) + ((j + 1) * 4 * new_width)] = 255; //alpha

                //pixel 4 [x+1,y+1]
                new_data[((i + 1) * 4) + ((j + 1) * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //R
                new_data[(((i + 1) * 4) + 1) + ((j + 1) * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //G
                new_data[(((i + 1) * 4) + 2) + ((j + 1) * 4 * new_width)] = data[((i / 2) * 4) + ((j / 2) * 4 * width)]; //B
                new_data[(((i + 1) * 4) + 3) + ((j + 1) * 4 * new_width)] = 255; //alpha
            }

            for (int i = 0; i < (new_width); i = i + 2) //painting in half tone
            {
                for (int j = 0; j < (new_height); j = j + 2)
                {
                    //pixel 1 [x,y] - threshold 4 - 255 * 0.8 and higher 
                    if (data[(( i / 2) * 4) + (( j / 2 ) * 4 * width)] >= 0.8 * 255)
                    {
                        
                        new_data[(i * 4) + (j * 4 * new_width)] = 255; //R
                        new_data[(i * 4) + 1 + (j * 4 * new_width)] = 255; //G
                        new_data[(i * 4) + 2 + (j * 4 * new_width)] = 255; //B
                        new_data[(i * 4) + 3 + (j * 4 * new_width)] = 255; //alpha
                    }
                    else 
                    {
                        new_data[(i * 4) + (j * 4 * new_width)] = 0; //R
                        new_data[(i * 4) + 1 + (j * 4 * new_width)] = 0; //G
                        new_data[(i * 4) + 2 + (j * 4 * new_width)] = 0; //B
                        new_data[(i * 4) + 3 + (j * 4 * new_width)] = 0; //alpha
                    }

                    //pixel 2 [x+1,y] - threshold 2 - 255 * 0.4 < x < 255 * 0.6
                    if ((data[((i / 2) * 4) + ((j / 2) * 4 * width)] >= 255 * 0.4))
                    {
                        new_data[((i + 1) * 4) + (j * 4 * new_width)] = 255; //R
                        new_data[(((i + 1) * 4) + 1) + (j * 4 * new_width)] = 255; //G
                        new_data[(((i + 1) * 4) + 2) + (j * 4 * new_width)] = 255; //B
                        new_data[(((i + 1) * 4) + 3) + (j * 4 * new_width)] = 255; //alpha
                    }
                    else 
                    {
                        new_data[((i + 1) * 4) + (j * 4 * new_width)] = 0; //R
                        new_data[(((i + 1) * 4) + 1) + (j * 4 * new_width)] = 0; //G
                        new_data[(((i + 1) * 4) + 2) + (j * 4 * new_width)] = 0; //B
                        new_data[(((i + 1) * 4) + 3) + (j * 4 * new_width)] = 0; //alpha
                    }

                    //pixel 3 [x,y+1] - threshold 1 - 255 * 0.2 < x < 255 * 0.4
                    if ((data[((i / 2) * 4) + ((j / 2) * 4 * width)] >= 255 * 0.2))
                    {
                        new_data[(i * 4) + ((j + 1) * 4 * new_width)] = 0; //R
                        new_data[((i * 4) + 1) + ((j + 1) * 4 * new_width)] = 0; //G
                        new_data[((i * 4) + 2) + ((j + 1) * 4 * new_width)] = 0; //B
                        new_data[((i * 4) + 3) + ((j + 1) * 4 * new_width)] = 255; //alpha
                    }
                    else 
                    {
                        new_data[(i * 4) + ((j + 1) * 4 * new_width)] = 0; //R
                        new_data[((i * 4) + 1) + ((j + 1) * 4 * new_width)] = 0; //G
                        new_data[((i * 4) + 2) + ((j + 1) * 4 * new_width)] = 0; //B
                        new_data[((i * 4) + 3) + ((j + 1) * 4 * new_width)] = 255; //alpha
                    }

                    //pixel 4 [x+1,y+1] - threshold 3 - 255 * 0.6 < x < 255 * 0.8
                    if ((data[((i / 2) * 4) + ((j / 2) * 4 * width)] >= 255 * 0.6))
                    {
                        new_data[((i + 1) * 4) + ((j + 1) * 4 * new_width)] = 255; //R
                        new_data[(((i + 1) * 4) + 1) + ((j + 1) * 4 * new_width)] = 255; //G
                        new_data[(((i + 1) * 4) + 2) + ((j + 1) * 4 * new_width)] = 255; //B
                        new_data[(((i + 1) * 4) + 3) + ((j + 1) * 4 * new_width)] = 255; //alpha
                    }
                    else 
                    {
                        new_data[((i + 1) * 4) + ((j + 1) * 4 * new_width)] = 0; //R
                        new_data[(((i + 1) * 4) + 1) + ((j + 1) * 4 * new_width)] = 0; //G
                        new_data[(((i + 1) * 4) + 2) + ((j + 1) * 4 * new_width)] = 0; //B
                        new_data[(((i + 1) * 4) + 3) + ((j + 1) * 4 * new_width)] = 255; //alpha
                    }
                }
            }
        }

        //printing our results
        std::ofstream myfile;
        myfile.open("half-tone.txt");
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++) //[x,y]
            {
                myfile << int(new_data[(i * 4) + (j * 4 * new_width)]) << " "; //I'm always printing out the R value of the RGBA, they should be the same though
            }
            myfile << "\n";
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new_width, new_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, new_data);
        stbi_image_free(new_data);
    }

    else if (filter_num == 3) // canny_edge, im only printing the results here since all of the work is done in the fragment shader
    {
        //printing our results
        std::ofstream myfile;
        myfile.open("canny_edge.txt");
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++) //[x,y]
            {
                myfile << int(data[(i * 4) + (j * 4 * width)]) << " "; //I'm always printing out the R value of the RGBA, they should be the same though
            }
            myfile << "\n";
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else 
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    
    stbi_image_free(data);
}

Texture::Texture(int width,int height,unsigned char *data)
{
    glGenTextures(1, &m_texture);
    Bind(m_texture);
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

void Texture::Bind(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

