#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/mat4x4.hpp"

GLuint VBO;
GLuint gWorldLocation;

using namespace glm;

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(clamp(Scale, 0.0, 1.0), 1.0);                                           \n\
}";


static void RenderSceneCB()
{
    static float Scale = 0.0f;

    Scale += 0.001f;

    mat4 edmat;

    edmat[0][0] = 1.0f; edmat[0][1] = 0.0f; edmat[0][2] = 0.0f; edmat[0][3] = 0.0f;

    edmat[1][0] = 0.0f; edmat[1][1] = 1.0f; edmat[1][2] = 0.0f; edmat[1][3] = 0.0f;

    edmat[2][0] = 0.0f; edmat[2][1] = 0.0f; edmat[2][2] = 1.0f; edmat[2][3] = 0.0f;

    edmat[3][0] = 0.0f; edmat[3][1] = 0.0f; edmat[3][2] = 0.0f; edmat[3][3] = 1.0f;

    mat4 rotZ;
        
    rotZ[0][0] = 1.0f; rotZ[0][1] = 0.0f; rotZ[0][2] = 0.0f; rotZ[0][3] = -sinf(Scale) / 2;

    rotZ[1][0] = 0.0f; rotZ[1][1] = 1.0f; rotZ[1][2] = 0.0f; rotZ[1][3] = cosf(Scale) / 2;
    
    rotZ[2][0] = 0.0f; rotZ[2][1] = 0.0f; rotZ[2][2] = 1.0f; rotZ[2][3] = 0.0f;
    
    rotZ[3][0] = 0.0f; rotZ[3][1] = 0.0f; rotZ[3][2] = 0.0f; rotZ[3][3] = 1.0f;

    mat4 rotX;

    rotX[0][0] = 1.0f; rotX[0][1] = 0.0f; rotX[0][2] = 0.0f; rotX[0][3] = 0.0f;

    rotX[1][0] = 0.0f; rotX[1][1] = 1.0f; rotX[1][2] = 0.0f; rotX[1][3] = cosf(Scale) / 2;

    rotX[2][0] = 0.0f; rotX[2][1] = 0.0f; rotX[2][2] = 1.0f; rotX[2][3] = -sinf(Scale) / 2;

    rotX[3][0] = 0.0f; rotX[3][1] = 0.0f; rotX[3][2] = 0.0f; rotX[3][3] = 1.0f;

    mat4 vrash;

    vrash[0][0] = sinf(Scale) ; vrash[0][1] = 0.0f; vrash[0][2] = 0.0f; vrash[0][3] = 0.0f;

    vrash[1][0] = 0.0f; vrash[1][1] = sinf(Scale) ; vrash[1][2] = 0.0f; vrash[1][3] = 0.0f;

    vrash[2][0] = 0.0f; vrash[2][1] = 0.0f; vrash[2][2] = 1.0f; vrash[2][3] = 0.0f;

    vrash[3][0] = 0.0f; vrash[3][1] = 0.0f; vrash[3][2] = 0.0f; vrash[3][3] = 1.0f;

    mat4 rez = edmat * rotZ * rotX * vrash;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &rez[0][0]);

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 1.0f);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_POLYGON, 0, 8);
    glDisableVertexAttribArray(0);
    glutSwapBuffers();
}


static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
    double r = 0.4;
    double t = 1.0;
    const double PI = 3.141592653589793;

    vec3 Vertices[8];

    for (int i = 0; i < 8; i++) {
        Vertices[i] = vec3(r * cos(t + i * PI * 0.25), r * sin(t + i * PI * 0.25), 0.0f);
    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(832, 624);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("rewrewregf");

    InitializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    CreateVertexBuffer();

    CompileShaders();

    glutMainLoop();

    return 0;
}