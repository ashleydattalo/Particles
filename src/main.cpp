// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define SHADER_PATH "/Users/ashleydattalo/graphics/LearnOpenGL/src/Shaders/"
#define RESOURCE_PATH "/Users/ashleydattalo/graphics/LearnOpenGL/resources/"

#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Particles/ParticleSystem.hpp"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

const GLuint WIDTH = 1200, HEIGHT = 1000;

bool keys[1024]; 
bool firstMouse = true;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

ParticleSystem *particles;

std::vector<float> data;
int numParticles = 100000;
int numVertices = 0;
int numFaces = 0;

float randFloat(float min, float max) {
    float r = rand() / (float)RAND_MAX;
    return (1.0f - r) * min + r * max;
}

void addLine(glm::vec3 vert1, glm::vec3 vert2) {

    // std::cout << std::endl <<  "vert1: ";
    // std::cout << vert1.x << " ";
    // std::cout << vert1.y << " ";
    // std::cout << vert1.z << " ";

    // std::cout << "vert2: ";
    // std::cout << vert2.x << " ";
    // std::cout << vert2.y << " ";
    // std::cout << vert2.z << " ";

    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;

    float step = 5.0f;
    float incX, incY, incZ;

    minX = fmin(vert1.x, vert2.x);
    minY = fmin(vert1.y, vert2.y);
    minZ = fmin(vert1.z, vert2.z);
    // std::cout << minX << " ";
    // std::cout << minY << " ";
    // std::cout << minZ << " ";

    maxX = fmax(vert1.x, vert2.x);
    maxY = fmax(vert1.y, vert2.y);
    maxZ = fmax(vert1.z, vert2.z);

    // std::cout << ", ";
    // std::cout << maxX << " ";
    // std::cout << maxY << " ";
    // std::cout << maxZ << " ";

    incX = (maxX-minX)/step;
    incY = (maxY-minY)/step;
    incZ = (maxZ-minZ)/step;
    
    // std::cout << ", ";
    // std::cout << incX << " ";
    // std::cout << incY << " ";
    // std::cout << incZ << " , ";


    for (int i = 1; i < step; i++) {
        if (vert1.x < vert2.x) {
            data.push_back(minX + i * incX);
        }
        else {
            data.push_back(maxX - i * incX);   
        }
        if (vert1.y < vert2.y) {
            data.push_back(minY + i * incY);
        }
        else {
            data.push_back(maxY - i * incY);   
        }
        if (vert1.z < vert2.z) {
            data.push_back(minZ + i * incZ);
        }
        else {
            data.push_back(maxZ - i * incZ);   
        }
        
        // std::cout << std::endl;

        // std::cout << "   " << minX + i * incX << " ";
        // std::cout << "   " << minY + i * incY << " ";
        // std::cout << "   " << minZ + i * incZ << " ";

        numVertices++;
    }

}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);    
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }    

    //transform feedback
    const GLchar* vertexShaderSrc = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "layout (location = 1) in vec3 force;\n"

        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        
        "out vec3 outPos;\n"

        "void main()\n"
        "{\n"
            "outPos = pos;\n"
            // "outPos.y = sin(pos.x);\n"

            "gl_Position = projection * view * model * vec4(outPos, 1.0f);\n"
            "gl_PointSize = 1.0f;\n"

        "}\n";

        // Fragment shader
    const GLchar* fragShaderSrc =
        "#version 330 core\n"

        "in vec3 outPos;\n"
        "in float col;\n"
        "out vec4 outColor;\n"

        "void main()\n"
        "{\n"
            // "outColor = vec4(0.0f, 0.0f, col, 1.0);\n"
            "outColor = vec4(1.0f);\n"
        "}\n";


    GLint success;
    GLchar infoLog[512];
    // Compile shaderVert
    GLuint shaderVert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaderVert, 1, &vertexShaderSrc, NULL);
    glCompileShader(shaderVert);

    glGetShaderiv(shaderVert, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderVert, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile shaderFrag
    GLuint shaderFrag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shaderFrag, 1, &fragShaderSrc, NULL);
    glCompileShader(shaderFrag);

    glGetShaderiv(shaderFrag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderFrag, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create program and specify transform feedback variables
    GLuint program = glCreateProgram();
    glAttachShader(program, shaderVert);
    glAttachShader(program, shaderFrag);

    const GLchar* feedbackVaryings[] = { "outPos" };
    glTransformFeedbackVaryings(program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(program);
    glUseProgram(program);

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create input VBO and vertex format
    // GLfloat data[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };


    for (int i = 0; i < numParticles; i++) {
        data.push_back(randFloat(-30, 30));
        data.push_back(randFloat(-30, 30));
        data.push_back(randFloat(-300, 300));
        
        // data.push_back(0.2f);
    }

    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< float > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    const char *path = "../resources/simpleCube.obj";

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }
    // while( 1 ){
    //     char lineHeader[128];
    //     // read the first word of the line
    //     int res = fscanf(file, "%s", lineHeader);
    //     if (res == EOF)
    //         break; // EOF = End Of File. Quit the loop.

    //     if ( strcmp( lineHeader, "v" ) == 0 ){
    //         glm::vec3 vertex;
    //         fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
    //         // vertex *=20;
    //         data.push_back(vertex.x);
    //         data.push_back(vertex.y);
    //         data.push_back(vertex.z);

    //         //forces
    //         data.push_back(vertex.x);
    //         data.push_back(vertex.y);
    //         data.push_back(vertex.z);
    //         numVertices++;
    //     }

    //     if ( strcmp( lineHeader, "f" ) == 0 ){
    //         glm::vec3 triangle;
    //         fscanf(file, "%f %f %f\n", &triangle.x, &triangle.y, &triangle.z );

    //         std::cout << std::endl << triangle.x << " ";
    //         std::cout << triangle.y << " ";
    //         std::cout << triangle.z << " ";

    //         glm::vec3 vert1, vert2, vert3;

    //         float offset;
    //         offset = (triangle.x -1) * 3;
    //         vert1.x = data[offset + 0];
    //         vert1.y = data[offset + 1];
    //         vert1.z = data[offset + 2];

    //         offset = (triangle.y -1) * 3;
    //         vert2.x = data[offset + 0];
    //         vert2.y = data[offset + 1];
    //         vert2.z = data[offset + 2];

    //         offset = (triangle.z -1) * 3;
    //         vert3.x = data[offset + 0];
    //         vert3.y = data[offset + 1];
    //         vert3.z = data[offset + 2];


    //         // addLine(vert1, vert2);
    //         // addLine(vert1, vert3);
    //         // addLine(vert2, vert3);

    //         numFaces++;
    //     }

    // }
    // numParticles = numVertices;
    for (int i = 0; i < data.size(); i+=3) {
        // printf("%f %f %f\n", data[i], data[i+1], data[i + 2]);
    }
    std::cout << "numVertices: " << numVertices << std::endl;
    std::cout << "numFaces: " << numFaces << std::endl;



    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), &data[0], GL_STREAM_DRAW);

    GLint inputAttrib = glGetAttribLocation(program, "pos");
    glEnableVertexAttribArray(inputAttrib);
    glVertexAttribPointer(inputAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);


    // GLint forceAttrib = glGetAttribLocation(program, "force");
    // glEnableVertexAttribArray(forceAttrib);
    // glVertexAttribPointer(forceAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    // GLint colAttrib = glGetAttribLocation(program, "color");
    // glEnableVertexAttribArray(colAttrib);
    // glVertexAttribPointer(colAttrib, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));


    // Create transform feedback buffer
    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), nullptr, GL_STATIC_READ);


    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
    std::vector<float> feedback;
    feedback.resize(data.size());
    glBindBuffer(GL_ARRAY_BUFFER, vbo);


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create camera transformations
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom + 20.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 200.0f);
        // Get the uniform locations
        
        GLint modelLoc = glGetUniformLocation(program, "model");
        GLint viewLoc  = glGetUniformLocation(program,  "view");
        GLint projLoc  = glGetUniformLocation(program,  "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        static int i = 0;
        // std::cout << std::endl << i++ << ": ";
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Perform feedback transform
            // glEnable(GL_RASTERIZER_DISCARD);

            glBeginTransformFeedback(GL_POINTS);
                glDrawArrays(GL_POINTS, 0, numParticles);
            glEndTransformFeedback();

            // glDisable(GL_RASTERIZER_DISCARD);
            // glFlush();
            glfwSwapBuffers(window);

            // Fetch and print results
            glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedback.size()*sizeof(float), &feedback[0]);
            for (int j = 0; j < data.size(); j+=3) {
                // printf("%d ~ %d: %f %f %f\n", i, j, feedback[j], feedback[j+1], feedback[j+2]);
                data[j] = feedback[j];
            }
            for (int j = 0; j < data.size(); j++) {
                // printf("%f ", data[j]);
                data[j] = feedback[j];
            }
            glBufferSubData(GL_ARRAY_BUFFER, 0, data.size()*sizeof(float), &data[0]);



        // Swap the screen buffers
    }

    glDeleteProgram(program);
    glDeleteShader(shaderVert);
    glDeleteShader(shaderFrag);

    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);

    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }    
    if (key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false; 
}

// Moves/alters the camera positions based on user input
void do_movement() {
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(keys[GLFW_KEY_J])
        camera.ProcessKeyboard(SPEEDUP, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}  