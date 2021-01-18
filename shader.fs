#version 330 core
        	out vec4 FragColor;
        	//in vec3 MyColor;
	in vec2 MyTexture;
	uniform sampler2D texData;
	uniform sampler2D texData1;
	uniform float amount;
	//in vec3 position;
        	void main()
        	{
           		FragColor = mix(texture(texData,MyTexture),texture(texData1,MyTexture),amount);
		//FragColor = texture(texData,MyTexture)*vec4(MyColor,1.0);
		//FragColor = vec4(MyColor,1.0);
        	}