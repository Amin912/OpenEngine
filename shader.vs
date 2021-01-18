#version 330 core
        layout (location = 0) in vec3 aPos;
        //layout (location = 1) in vec3 aCol;
        layout (location = 1) in vec2 aTex;
       // out vec3 MyColor;
	//out vec3 position;
	out vec2 MyTexture;
        uniform float offset;
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
        void main()
        {
           
           gl_Position = projection*view*model* vec4(aPos.x,aPos.y+offset, aPos.z, 1.0);
           //MyColor = aCol;
	MyTexture=aTex;
	//position=aPos;
        }