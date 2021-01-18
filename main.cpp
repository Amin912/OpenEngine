#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

#include <iostream>
#include "shader.h"
#include "stb_image.h"
#include <cmath>
using namespace std;
//using namespace glm;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool firstMouse=true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float Zoom   =  45.0f;



float amount=0;
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
Camera cam;
//vec3 direction;
//direction.x = cos(glm::radians(yaw)); // Note that we convert the angle to radians first
//direction.z = sin(glm::radians(yaw));
//direction.y = sin(glm::radians(pitch));
//yaw = -90.0f;
//const char *vertexShaderSource = "#version 330 core\n"
//        "layout (location = 0) in vec3 aPos;\n"
//        "layout (location = 1) in vec3 aCol;\n"
//        "out vec3 MyColor;\n"
//
//        "void main()\n"
//        "{\n"
//        "   gl_Position = vec4(aPos, 1.0);\n"
//        "   MyColor = aCol;\n"
//        "}\0";
//const char *fragmentShaderSource =
//        "#version 330 core\n"
//        "out vec4 FragColor;\n"
//        "in vec3 MyColor;\n"
//        "void main()\n"
//        "{\n"
//        "   FragColor = vec4(MyColor, 1.0f);\n"
//        "}\0";
//const char *fragmentShaderSource1 =
//        "#version 330 core\n"
//        "out vec4 FragColor1;\n"
//        "uniform vec4 MyColor;\n"
//        "void main()\n"
//        "{\n"
//        "   FragColor1 = MyColor;\n"
//        "}\0";

void ball(float arr[], double x, double y, float r)
{
    int i=0;
    arr[i++]=x;
    arr[i++]=y;
    arr[i++]=0.0f;
//    arr[i++]=1.0f;
//    arr[i++]=1.0f;
//    arr[i++]=1.0f;
    arr[i++]=0.5f;
    arr[i++]=0.5f;
    for (int a=0; a<360;a++){
        float ar=a*3.1415926535897932384626433832795028841971/180;
        arr[i++]=cos(ar)*r+x;
        arr[i++]=sin(ar)*r+y;
        arr[i++]=0.0f;
//        arr[i++]=1.0f;
//        arr[i++]=1.0f;
//        arr[i++]=1.0f;
//        arr[i++]=arr[i-6]/2+0.5;
//        arr[i++]=arr[i-5]/2+0.5;
        arr[i++]=(cos(ar)+x)/2+0.5;
        arr[i++]=(sin(ar)+y)/2+0.5;

        //arr[i++]=(cos(ar)*r+y);
    }
}

void fall(double& y, double weight)
{
    double acc=0.001*weight;
    y-=acc;
    acc-=0.01;
    if (y<=-0.93){acc=0.0f;y=-0.95;}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    cam.ProcessMouseMovement(xoffset, yoffset);
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.ProcessMouseScroll(yoffset);
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void processInput(GLFWwindow *window)
{

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        amount += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if(amount >= 1.0f)
            amount = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        amount -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (amount <= 0.0f)
            amount = 0.0f;
    }
    const float cameraSpeed=2.0f*deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(FORWARD, deltaTime);
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(BACKWARD, deltaTime);
        cameraPos -= cameraSpeed * cameraFront;
    }
//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
////        cameraPos += cameraSpeed * cameraUp;
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
////        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(LEFT, deltaTime);
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;//cameraRright*speed
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(RIGHT, deltaTime);
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;//same
    }
}

int main()
{

    //double mouseX,mouseY;

//    long long nr;
//    glGetInteger64v(GL_MAX_VERTEX_ATTRIBS, &nr);
    //std::cout<<nr<<std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(700,700,"Connect 4",NULL,NULL);


    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //glViewport(0,0,400,400);
    glfwSetFramebufferSizeCallback(window ,framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
{
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
}
    //shaders building and compiling
    //vertex shader
    Shader my("shader.vs","shader.fs");
    float offset=0.0f;



//    unsigned int vertexShader;
//    vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//    glCompileShader(vertexShader);
//    int  success;
//    char infoLog[512];
//    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//    if(!success)
//    {
//        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
    //frame shader
//    unsigned int fragmentShader1;
//    fragmentShader1=glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
//    glCompileShader(fragmentShader1);
//    glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
//    if(!success)
//    {
//        glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::FRAGMENT1::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
//    unsigned int fragmentShader;
//    fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//    glCompileShader(fragmentShader);
//    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//    if(!success)
//    {
//        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
    //linking shaders
//    unsigned int shaderProgram,shaderProgram1;
//    shaderProgram=glCreateProgram();
//    shaderProgram1=glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glAttachShader(shaderProgram1, vertexShader);
//    glAttachShader(shaderProgram1, fragmentShader1);
//    glLinkProgram(shaderProgram);
//    glLinkProgram(shaderProgram1);
//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//    if(!success)
//    {
//        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//        std::cout << "ERROR::LINK_FAILED\n" << infoLog << std::endl;
//    }
//    //glUseProgram(shaderProgram);
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//    glDeleteShader(fragmentShader1);

    //vertex data and buffers
    float third[] = {
        0.0f,  0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   // center right
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };
float first[361*8];

int i=0;
float cl=0.1;
double cx=0,cy=0;
//      //i=0;
        float r=0.5f;
        ball(first, cx, cy, r);

//glfwGetCursorPos(window,&cx,&cy);
//double cy=mouseY;
//float r=0.07f;
//first[i++]=cx;
//first[i++]=cy;
//first[i++]=0.0f;
//for (int a=0; a<360;a++){
//    float ar=a*3.1415926535897932384626433832795028841971/180;
//    first[i++]=cos(ar)*r+cx;
//    first[i++]=sin(ar)*r+cy;
//    first[i++]=0.0f;
//    //std::cout<<cos(ar)*r<<std::endl;
//    }

    //i=0;
    //for (int y=0;y<360;y++){std::cout<<first[i]<<first[i+1]<<first[i+2]<<std::endl;i+=3;}
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

    float second[] = {
        cx, -0.5f, 0.0f, 0.5, 0.0f, 1.0f,  // left
        0.9f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// right
        0.45f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f// top
    };
    stbi_set_flip_vertically_on_load(true);
        my.use();
        my.setInt("texData", 0);
        my.setInt("texData1", 1);
        unsigned int texture,texture1;
        glGenTextures(1,&texture);
        glGenTextures(1,&texture1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_REPEAT);
        int width, height, nrChannels;
        unsigned char *data = stbi_load("C:\\Users\\medam\\Desktop\\OPENGL\\container.jpg", &width, &height, &nrChannels, 0);
        if(data){
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else std::cout<<"failed to load texture"<<std::endl;
        stbi_image_free(data);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,texture1);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_REPEAT);
        data = stbi_load("C:\\Users\\medam\\Desktop\\scr.png", &width, &height, &nrChannels, 0);
        if(data){
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else std::cout<<"failed to load texture"<<std::endl;
        stbi_image_free(data);
        i=0;
        unsigned int VBO[3],VAO[3], indices[361*3],EBO;
        for (int b=1;b<361;b++){indices[i++]=0;indices[i++]=b;indices[i++]=b+1;}
        indices[1079]=1;
        glGenBuffers(1,&EBO);
        glGenBuffers(3, VBO);
        glGenVertexArrays(3, VAO);
        glBindVertexArray(VAO[0]);
        glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(first),first,GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) (3*sizeof(float)));
        glEnableVertexAttribArray(1);
       // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (6*sizeof(float)));
        //glEnableVertexAttribArray(2);

//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*) 0);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*) (3*sizeof(float)));
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) 0);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (3*sizeof(float)));
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (6*sizeof(float)));
//        glEnableVertexAttribArray(2);
        glBindVertexArray(VAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(third),third,GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (6*sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(VAO[2]);
        glBindBuffer(GL_ARRAY_BUFFER,VBO[2]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) (3*sizeof(float)));
        glEnableVertexAttribArray(1);
        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

        glm::mat4 model=glm::mat4(1.0f);
        model=glm::rotate(model, glm::radians(-40.0f), glm::vec3(1.0f,0.0f,0.0f));
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection=glm::mat4(1.0f);
        projection=glm::perspective(glm::radians(60.0f), 700.0f / 700.0f, 0.1f,100.0f);
        my.setMat4("model",model);
        unsigned int viewloc=glGetUniformLocation(my.ID,"view");
        glUniformMatrix4fv(viewloc, 1, false, value_ptr(view));
        unsigned int projloc=glGetUniformLocation(my.ID,"projection");
        glUniformMatrix4fv(projloc, 1, false, value_ptr(projection));

        glEnable(GL_DEPTH_TEST);

        glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //rendering starts
        glClearColor(0.7,0.3,0.2,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//        cx=((cx-700)/700+0.5)*2;
//            cy=(-(cy-700)/700-0.5)*2;
my.setFloat("amount", amount);
//        cout<<cx<<endl<<cy<<endl;C
        //glUseProgram(shaderProgram);

       // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0, 0.0, 0.0));



//        mat4 model=mat4(1.0f);
//        model=rotate(model, (float)glfwGetTime(), vec3(0.5f,1.0f,0.0f));
//        unsigned int modelloc=glGetUniformLocation(my.ID,"model");
//        glUniformMatrix4fv(modelloc, 1, false, value_ptr(model));
        glm::mat4 view;
//       view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        view = glm::lookAt(cam.Position, cam.Position + cam.Front, cam.Up);
        my.setMat4("view",view);
        my.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,texture1);
//        float timev=glfwGetTime();
//        float green=sin(timev) / 2.0f + 0.5f;
//        float red=cos(timev)  + 0.2f;
//        int color=glGetUniformLocation(shaderProgram1,"MyColor");
//        glUniform4f(color, red, 0.1f, 0.5f, 1.0f);

//        if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS){
//            glfwGetCursorPos(window,&cx,&cy);
//           my.use();
//            cx=mouseX;
//            cy=mouseY;
//
//        }
        glBindVertexArray(VAO[2]);
        for(unsigned int i = 0; i < 10; i++)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[i]);
    float angle = 20.0f * i;
    model = glm::rotate(model, glm::radians(angle)*glm::radians(angle)+(float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
    my.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}
//        glBindVertexArray(VAO[0]);
//        glDrawElements(GL_TRIANGLES, 360*3,GL_UNSIGNED_INT,0);
        //fall(cy,1);




//        glBindVertexArray(VAO[1]);
//        glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(third),third,GL_STATIC_DRAW);
//        glDrawArrays(GL_TRIANGLES,0,3);
        //second[0]+=second[0]<-1?0.9:-0.01;


        //cl+=0.01;
       // glUseProgram(shaderProgram1);
        my.setFloat("offset",offset);
        //glBindVertexArray(VAO[1]);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //finished
        //offset-=0.0005;
        glfwSwapBuffers(window);
        glfwPollEvents();
        ball(first,cx, cy, r);
        i=0;
        for (int b=1;b<361;b++){indices[i++]=0;indices[i++]=b;indices[i++]=b+1;}
        indices[1079]=1;
//        glBindVertexArray(VAO[0]);
//         cout<<amount;
//        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(first),first,GL_DYNAMIC_DRAW);
        //cout<<second[0]<<endl;
//        glBindVertexArray(VAO[1]);
//        glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
//        glBufferData(GL_ARRAY_BUFFER,sizeof(third),third,GL_STATIC_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*) (3*sizeof(float)));
//        glEnableVertexAttribArray(1);
    }
    glfwTerminate();
    return 0;
}
