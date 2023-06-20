#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> 

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <camera.h> 

using namespace std;

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace {

    const char* const WINDOW_TITLE = "5-5 Texturing Objects in a 3D Scene";

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh {
        GLuint vao;
        GLuint vbo;
        GLuint nFloorVertices;
        GLuint nOvenBodyVertices;
        GLuint nStoveTopVertices;
        GLuint nOvenWindowVertices;
        GLuint nOvenPanelVertices;
        GLuint nClockVertices;
        GLuint nCounterTopVertices;
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    bool perspective = true;

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;

    // Texture
    GLuint gTextureId[7];
    glm::vec2 gUVScale(5.0f, 5.0f);

    // Shader program
    GLuint gShapeProgramId;
    GLuint gLightProgramId;

    // camera
    Camera gCamera(glm::vec3(0.0f, -5.0f, 10.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Subject position and scale
    glm::vec3 gShapePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gShapeScale(2.0f);

    // Cube and light color
    glm::vec3 gObjectColor(0.0f, 0.0f, 0.0f);
    //glm::vec3 gLightColor(0.94f, 0.75f, 0.44f);
    glm::vec3 gLightColor(1.0f, 1.0f, 1.0f);

    // Light position and scale
    glm::vec3 gLightPosition(0.0f, -2.0f, 0.0f);
    glm::vec3 gLightScale(0.2f);

}


/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void ULoadTextures();
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UDraw(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void UInitializeVAOs(GLMesh& mesh);
void URender();
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint ProgramId);

//Floor
GLfloat vert1[] = {
    // Vertex Positions    //Normals            // Texture
    -5.0f, -5.0f, -5.0f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
     5.0f, -5.0f, -5.0f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
     5.0f, -5.0f,  5.0f,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
     5.0f, -5.0f,  5.0f,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
    -5.0f, -5.0f,  5.0f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
    -5.0f, -5.0f, -5.0f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f
};

//Oven Body
GLfloat vert2[] = {
    // Vertex Positions     //Normals             // Texture
     -0.5f, -5.0f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f, //back surface
      0.5f, -5.0f, -0.5f,   0.0f,  0.0f, -1.0f,   0.2f, 0.0f,
      0.5f, -4.0f, -0.5f,   0.0f,  0.0f, -1.0f,   0.2f, 0.2f,
      0.5f, -4.0f, -0.5f,   0.0f,  0.0f, -1.0f,   0.2f, 0.2f,
     -0.5f, -4.0f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.2f,
     -0.5f, -5.0f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

     -0.5f, -5.0f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f, //front surface
      0.5f, -5.0f,  0.5f,   0.0f,  0.0f,  1.0f,   0.2f, 0.0f,
      0.5f, -4.0f,  0.5f,   0.0f,  0.0f,  1.0f,   0.2f, 0.2f,
      0.5f, -4.0f,  0.5f,   0.0f,  0.0f,  1.0f,   0.2f, 0.2f,
     -0.5f, -4.0f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.2f,
     -0.5f, -5.0f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

     -0.5f, -4.0f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, //left surface
     -0.5f, -4.0f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.2f, 0.0f,
     -0.5f, -5.0f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.2f, 0.2f,
     -0.5f, -5.0f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.2f, 0.2f,
     -0.5f, -5.0f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.2f,
     -0.5f, -4.0f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

      0.5f, -4.0f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, //right surface
      0.5f, -4.0f, -0.5f,   1.0f,  0.0f,  0.0f,   0.2f, 0.0f,
      0.5f, -5.0f, -0.5f,   1.0f,  0.0f,  0.0f,   0.2f, 0.2f,
      0.5f, -5.0f, -0.5f,   1.0f,  0.0f,  0.0f,   0.2f, 0.2f,
      0.5f, -5.0f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.2f,
      0.5f, -4.0f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

     -0.5f, -5.0f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.2f, //bottom surface
      0.5f, -5.0f, -0.5f,   0.0f, -1.0f,  0.0f,   0.2f, 0.2f,
      0.5f, -5.0f,  0.5f,   0.0f, -1.0f,  0.0f,   0.2f, 0.0f,
      0.5f, -5.0f,  0.5f,   0.0f, -1.0f,  0.0f,   0.2f, 0.0f,
     -0.5f, -5.0f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
     -0.5f, -5.0f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.2f,

     -0.5f, -4.0f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f, //top surface
      0.5f, -4.0f, -0.5f,   0.0f,  1.0f,  0.0f,   0.2f, 0.0f,
      0.5f, -4.0f,  0.5f,   0.0f,  1.0f,  0.0f,   0.2f, 0.2f,
      0.5f, -4.0f,  0.5f,   0.0f,  1.0f,  0.0f,   0.2f, 0.2f,
     -0.5f, -4.0f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.2f,
     -0.5f, -4.0f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f
};

//Oven Top
GLfloat vert3[] = {
    // Vertex Positions         //Normals             // Texture
    -0.45f, -3.9999f, -0.40f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
     0.45f, -3.9999f, -0.40f,   0.0f,  1.0f,  0.0f,   0.2f, 0.0f,
     0.45f, -3.9999f,  0.45f,   0.0f,  1.0f,  0.0f,   0.2f, 0.2f,
     0.45f, -3.9999f,  0.45f,   0.0f,  1.0f,  0.0f,   0.2f, 0.2f,
    -0.45f, -3.9999f,  0.45f,   0.0f,  1.0f,  0.0f,   0.0f, 0.2f,
    -0.45f, -3.9999f, -0.40f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,

};

//Oven Window
GLfloat vert4[] = {
    // Vertex Positions         //Normals             // Texture
    -0.5f, -4.85f,  0.5001f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
     0.5f, -4.85f,  0.5001f,   0.0f,  0.0f,  1.0f,   0.2f, 0.0f,
     0.5f, -4.15f,  0.5001f,   0.0f,  0.0f,  1.0f,   0.2f, 0.2f,
     0.5f, -4.15f,  0.5001f,   0.0f,  0.0f,  1.0f,   0.2f, 0.2f,
    -0.5f, -4.15f,  0.5001f,   0.0f,  0.0f,  1.0f,   0.0f, 0.2f,
    -0.5f, -4.85f,  0.5001f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f
};

//Oven Panel
GLfloat vert5[] = {
    // Vertex Positions      //Normals              // Texture
     0.5f, -3.65f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //Back Surface
     0.5f, -4.0f,  -0.5f,    0.0f,  0.0f, -1.0f,    0.2f, 0.0f,
    -0.5f, -4.0f,  -0.5f,    0.0f,  0.0f, -1.0f,    0.2f, 0.2f,
    -0.5f, -4.0f,  -0.5f,    0.0f,  0.0f, -1.0f,    0.2f, 0.2f,
    -0.5f, -3.65f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.2f,
     0.5f, -3.65f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,

     0.5f, -3.65f, -0.45f,   0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //Front Surface
     0.5f, -4.0f,  -0.45f,   0.0f,  0.0f,  1.0f,    0.2f, 0.0f,
    -0.5f, -4.0f,  -0.45f,   0.0f,  0.0f,  1.0f,    0.2f, 0.2f,
    -0.5f, -4.0f,  -0.45f,   0.0f,  0.0f,  1.0f,    0.2f, 0.2f,
    -0.5f, -3.65f, -0.45f,   0.0f,  0.0f,  1.0f,    0.0f, 0.2f,
     0.5f, -3.65f, -0.45f,   0.0f,  0.0f,  1.0f,    0.0f, 0.0f,

    -0.5f, -3.65f, -0.45f,  -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //Left Side
    -0.5f, -4.0f,  -0.45f,  -1.0f,  0.0f,  0.0f,    0.2f, 0.0f,
    -0.5f, -4.0f,  -0.5f,   -1.0f,  0.0f,  0.0f,    0.2f, 0.2f,
    -0.5f, -4.0f,  -0.5f,   -1.0f,  0.0f,  0.0f,    0.2f, 0.2f,
    -0.5f, -3.65f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.2f,
    -0.5f, -3.65f, -0.45f,  -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,

     0.5f, -3.65f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //Right Side
     0.5f, -4.0f,  -0.5f,    1.0f,  0.0f,  0.0f,    0.2f, 0.0f,
     0.5f, -4.0f,  -0.45f,   1.0f,  0.0f,  0.0f,    0.2f, 0.2f,
     0.5f, -4.0f,  -0.45f,   1.0f,  0.0f,  0.0f,    0.2f, 0.2f,
     0.5f, -3.65f, -0.45f,   1.0f,  0.0f,  0.0f,    0.0f, 0.2f,
     0.5f, -3.65f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,

     0.5f, -4.0f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 0.0f, //Bottom Surface
     0.5f, -4.0f, -0.45f,    0.0f, -1.0f,  0.0f,    0.2f, 0.0f,
    -0.5f, -4.0f, -0.45f,    0.0f, -1.0f,  0.0f,    0.2f, 0.2f,
    -0.5f, -4.0f, -0.45f,    0.0f, -1.0f,  0.0f,    0.2f, 0.2f,
    -0.5f, -4.0f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 0.2f,
     0.5f, -4.0f, -0.5f,     0.0f, -1.0f,  0.0f,    0.0f, 0.0f,

     0.5f, -3.65f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f, //Top Surface
     0.5f, -3.65f, -0.45f,   0.0f,  1.0f,  0.0f,    0.2f, 0.0f,
    -0.5f, -3.65f, -0.45f,   0.0f,  1.0f,  0.0f,    0.2f, 0.2f,
    -0.5f, -3.65f, -0.45f,   0.0f,  1.0f,  0.0f,    0.2f, 0.2f,
    -0.5f, -3.65f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.2f,
     0.5f, -3.65f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f
};

//Oven Panel Screen
GLfloat vert6[] = {
    // Vertex Positions        //Normals             // Texture
    -0.1f, -3.95f, -0.4499f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
     0.1f, -3.95f, -0.4499f,   0.0f,  0.0f,  1.0f,   0.2f, 0.0f,
     0.1f, -3.75f, -0.4499f,   0.0f,  0.0f,  1.0f,   0.2f, 0.2f,
     0.1f, -3.75f, -0.4499f,   0.0f,  0.0f,  1.0f,   0.2f, 0.2f,
    -0.1f, -3.75f, -0.4499f,   0.0f,  0.0f,  1.0f,   0.0f, 0.2f,
    -0.1f, -3.95f, -0.4499f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f
};

//Counter Tops
GLfloat vert7[] = {
    // Vertex Positions      //Normals              // Texture
     0.0f,   0.05f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f, //Back Surface
     0.0f,   0.0f,  -0.5f,    0.0f,  0.0f, -1.0f,    0.2f, 0.0f,
    -0.35f,  0.0f,  -0.5f,    0.0f,  0.0f, -1.0f,    0.2f, 0.2f,
    -0.35f,  0.0f,  -0.5f,    0.0f,  0.0f, -1.0f,    0.2f, 0.2f,
    -0.35f,  0.05f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.2f,
     0.0f,   0.05f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,

     0.0f,   0.05f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f, //Front Surface
     0.0f,   0.0f,   0.5f,    0.0f,  0.0f,  1.0f,    0.2f, 0.0f,
    -0.35f,  0.0f,   0.5f,    0.0f,  0.0f,  1.0f,    0.2f, 0.2f,
    -0.35f,  0.0f,   0.5f,    0.0f,  0.0f,  1.0f,    0.2f, 0.2f,
    -0.35f,  0.05f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.2f,
     0.0f,   0.05f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,

    -0.35f,  0.05f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //Left Side
    -0.35f,  0.0f,   0.5f,   -1.0f,  0.0f,  0.0f,    0.2f, 0.0f,
    -0.35f,  0.0f,  -0.5f,   -1.0f,  0.0f,  0.0f,    0.2f, 0.2f,
    -0.35f,  0.0f,  -0.5f,   -1.0f,  0.0f,  0.0f,    0.2f, 0.2f,
    -0.35f,  0.05f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.2f,
    -0.35f,  0.05f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,

     0.0f,   0.05f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f, //Right Side
     0.0f,   0.0f,  -0.5f,    1.0f,  0.0f,  0.0f,    0.2f, 0.0f,
     0.0f,   0.0f,   0.5f,    1.0f,  0.0f,  0.0f,    0.2f, 0.2f,
     0.0f,   0.0f,   0.5f,    1.0f,  0.0f,  0.0f,    0.2f, 0.2f,
     0.0f,   0.05f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.2f,
     0.0f,   0.05f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,

     0.0f,   0.0f,  -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f, //Bottom Surface
     0.0f,   0.0f,   0.5f,    0.0f, -1.0f,  0.0f,    0.2f, 0.0f,
    -0.35f,  0.0f,   0.5f,    0.0f, -1.0f,  0.0f,    0.2f, 0.2f,
    -0.35f,  0.0f,   0.5f,    0.0f, -1.0f,  0.0f,    0.2f, 0.2f,
    -0.35f,  0.0f,  -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.2f,
     0.0f,   0.0f,  -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,

     0.0f,   0.05f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f, //Top Surface
     0.0f,   0.05f,  0.5f,    0.0f,  1.0f,  0.0f,    0.2f, 0.0f,
    -0.35f,  0.05f,  0.5f,    0.0f,  1.0f,  0.0f,    0.2f, 0.2f,
    -0.35f,  0.05f,  0.5f,    0.0f,  1.0f,  0.0f,    0.2f, 0.2f,
    -0.35f,  0.05f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.2f,
     0.0f,   0.05f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f
};


// Vertex data
GLfloat vert8[] = {
    // Vertex Positions   //Normals             // Texture
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f, //Bottom Face
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.2f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.2f, 0.2f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.2f, 0.2f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.2f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f, //Front Face
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.2f, 0.0f,
     0.0f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,   0.1f, 0.2f,

     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f, //Right Face
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.2f, 0.0f,
     0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,   0.1f, 0.2f,

     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f, //Back Face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.2f, 0.0f,
     0.0f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,   0.1f, 0.2f,

    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, //Left Face
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.2f, 0.0f,
     0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,   0.1f, 0.2f

};


/* Cube Vertex Shader Source Code*/
const GLchar* shapeVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormal;
    out vec3 vertexFragmentPos;
    out vec2 vertexTextureCoordinate;

    //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * view * model * vec4(position, 1.0f);

        vertexFragmentPos = vec3(model * vec4(position, 1.0f));
        vertexNormal = mat3(transpose(inverse(model))) * normal;
        vertexTextureCoordinate = textureCoordinate;
    }
);


/* Shape Fragment Shader Source Code*/
const GLchar* shapeFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal;
    in vec3 vertexFragmentPos;
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor;

    // Uniform / Global variables for object color, light color, light position, and camera/view position
    uniform vec3 objectColor;
    uniform vec3 lightColor;
    uniform vec3 lightPos;
    uniform vec3 viewPosition;
    uniform sampler2D uTexture;
    uniform vec2 uvScale;

    void main() {

        vec3 norm = normalize(vertexNormal);
        vec3 lightDirection = normalize(lightPos - vertexFragmentPos);

        //Calculate Ambient lighting*/
        float ambientStrength = 0.3f;
        vec3 ambient = ambientStrength * lightColor;

        //Calculate Diffuse lighting*/
        float impact = max(dot(norm, lightDirection), 0.0);
        vec3 diffuse = impact * lightColor;

        //Calculate Specular lighting*/
        float specularIntensity = 0.4f;
        float highlightSize = 8.0f;
        vec3 viewDir = normalize(viewPosition - vertexFragmentPos);
        vec3 reflectDir = reflect(-lightDirection, norm);

        //Calculate specular component
        float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
        vec3 specular = specularIntensity * specularComponent * lightColor;

        // Texture holds the color to be used for all three components
        vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

        float distance = length(lightDirection - vertexFragmentPos);
        float attenuation = 1.0 / (1.0 + 0.7 * distance + 1.8 * (distance * distance));

        // Calculate phong result
        vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

        fragmentColor = vec4(phong, 1.0);
    }
);


/* Lamp Shader Source Code*/
const GLchar* lightVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position;

    //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * view * model * vec4(position, 1.0f);
    }
);


/* Fragment Shader Source Code*/
const GLchar* lightFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor;

    void main() {
        fragmentColor = vec4(1.0f);
    }
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[]) {

    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the shader program
    if (!UCreateShaderProgram(shapeVertexShaderSource, shapeFragmentShaderSource, gShapeProgramId))
        return EXIT_FAILURE;
    if (!UCreateShaderProgram(lightVertexShaderSource, lightFragmentShaderSource, gLightProgramId))
        return EXIT_FAILURE;

    ULoadTextures();

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gShapeProgramId);

    glUniform1i(glGetUniformLocation(gShapeProgramId, "uTextureFloor"), 0);
    glUniform1i(glGetUniformLocation(gShapeProgramId, "uTextureStainless"), 1);
    glUniform1i(glGetUniformLocation(gShapeProgramId, "uTextureStoveTop"), 2);
    glUniform1i(glGetUniformLocation(gShapeProgramId, "uTextureOvenWindow"), 3);
    glUniform1i(glGetUniformLocation(gShapeProgramId, "uTextureClock"), 4);

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    while (!glfwWindowShouldClose(gWindow)) {
        // per-frame timing
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);

    // Release texture
    UDestroyTexture(gTextureId[0]);
    UDestroyTexture(gTextureId[1]);
    UDestroyTexture(gTextureId[2]);
    UDestroyTexture(gTextureId[3]);
    UDestroyTexture(gTextureId[4]);

    // Release shader program
    UDestroyShaderProgram(gShapeProgramId);
    UDestroyShaderProgram(gLightProgramId);

    exit(EXIT_SUCCESS);
}


void ULoadTextures() {

    const char* texFilename;

    texFilename = "C:/Users/Screa/source/repos/CS 330 6-5 Lighting Complex Objects/resources/Floor.jpg";
    if (!UCreateTexture(texFilename, gTextureId[0])) {
        cout << "Failed to load texture " << texFilename << endl;
    }
    texFilename = "C:/Users/Screa/source/repos/CS 330 6-5 Lighting Complex Objects/resources/stainless steel.png";
    if (!UCreateTexture(texFilename, gTextureId[1])) {
        cout << "Failed to load texture " << texFilename << endl;
    }
    texFilename = "C:/Users/Screa/source/repos/CS 330 6-5 Lighting Complex Objects/resources/stove top.png";
    if (!UCreateTexture(texFilename, gTextureId[2])) {
        cout << "Failed to load texture " << texFilename << endl;
    }
    texFilename = "C:/Users/Screa/source/repos/CS 330 6-5 Lighting Complex Objects/resources/Oven Window.png";
    if (!UCreateTexture(texFilename, gTextureId[3])) {
        cout << "Failed to load texture " << texFilename << endl;
    }
    texFilename = "C:/Users/Screa/source/repos/CS 330 6-5 Lighting Complex Objects/resources/Clock.png";
    if (!UCreateTexture(texFilename, gTextureId[4])) {
        cout << "Failed to load texture " << texFilename << endl;
    }
    texFilename = "C:/Users/Screa/source/repos/CS 330 6-5 Lighting Complex Objects/resources/Cabinet.png";
    if (!UCreateTexture(texFilename, gTextureId[5])) {
        cout << "Failed to load texture " << texFilename << endl;
    }
    texFilename = "C:/Users/Screa/source/repos/CS 330 6-5 Lighting Complex Objects/resources/CounterTop.png";
    if (!UCreateTexture(texFilename, gTextureId[6])) {
        cout << "Failed to load texture " << texFilename << endl;
    }
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window) {

    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    if (*window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult) {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
}

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window) {

    static const float cameraSpeed = 3.0f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        perspective = true;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        perspective = false;

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);

}


// glfw: whenever the mouse moves, this callback is called
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos) {

    if (gFirstMouse) {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos;

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

    gCamera.ProcessMouseScroll(yoffset);

}


void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Function called to render a frame
void URender() {

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the shape VAO (used by shape and lamp)
    glBindVertexArray(gMesh.vao);

    // SHAPE: draw Light Source
    //-------------------------
    glUseProgram(gShapeProgramId);

    // Creates a perspective or ortho projection
    if (perspective == true) {

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = glm::translate(gShapePosition) * glm::scale(gShapeScale);
        glm::mat4 view = gCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gShapeProgramId, "model");
        GLint viewLoc = glGetUniformLocation(gShapeProgramId, "view");
        GLint projLoc = glGetUniformLocation(gShapeProgramId, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
        GLint objectColorLoc = glGetUniformLocation(gShapeProgramId, "objectColor");
        GLint lightColorLoc = glGetUniformLocation(gShapeProgramId, "lightColor");
        GLint lightPositionLoc = glGetUniformLocation(gShapeProgramId, "lightPos");
        GLint viewPositionLoc = glGetUniformLocation(gShapeProgramId, "viewPosition");

        // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
        glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
        glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
        glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
        const glm::vec3 cameraPosition = gCamera.Position;
        glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        GLint UVScaleLoc = glGetUniformLocation(gShapeProgramId, "uvScale");
        glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gTextureId[0]);

        // Create the mesh
        UDraw(gMesh);

        // Draws the triangles
        glDrawArrays(GL_TRIANGLES, 0, gMesh.nFloorVertices);

        // LIGHT: draw light
        //------------------
        glUseProgram(gLightProgramId);

        //Transform the smaller cube used as a visual que for the light source
        model = glm::translate(gLightPosition) * glm::scale(gLightScale);

        // Reference matrix uniforms from the Lamp Shader program
        modelLoc = glGetUniformLocation(gLightProgramId, "model");
        viewLoc = glGetUniformLocation(gLightProgramId, "view");
        projLoc = glGetUniformLocation(gLightProgramId, "projection");

        // Pass matrix data to the Lamp Shader program's matrix uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, gMesh.nFloorVertices);
    }
    if (perspective == false) {

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = glm::translate(gShapePosition) * glm::scale(gShapeScale);
        glm::mat4 view = gCamera.GetViewMatrix();
        glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gShapeProgramId, "model");
        GLint viewLoc = glGetUniformLocation(gShapeProgramId, "view");
        GLint projLoc = glGetUniformLocation(gShapeProgramId, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
        GLint objectColorLoc = glGetUniformLocation(gShapeProgramId, "objectColor");
        GLint lightColorLoc = glGetUniformLocation(gShapeProgramId, "lightColor");
        GLint lightPositionLoc = glGetUniformLocation(gShapeProgramId, "lightPos");
        GLint viewPositionLoc = glGetUniformLocation(gShapeProgramId, "viewPosition");

        // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
        glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
        glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
        glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
        const glm::vec3 cameraPosition = gCamera.Position;
        glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        GLint UVScaleLoc = glGetUniformLocation(gShapeProgramId, "uvScale");
        glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gTextureId[0]);

        // Create the mesh
        UDraw(gMesh);

        // Draws the triangles
        glDrawArrays(GL_TRIANGLES, 0, gMesh.nFloorVertices);

        // LIGHT: draw light
        //------------------
        glUseProgram(gLightProgramId);

        //Transform the smaller cube used as a visual que for the light source
        model = glm::translate(gLightPosition) * glm::scale(gLightScale);

        // Reference matrix uniforms from the Lamp Shader program
        modelLoc = glGetUniformLocation(gLightProgramId, "model");
        viewLoc = glGetUniformLocation(gLightProgramId, "view");
        projLoc = glGetUniformLocation(gLightProgramId, "projection");

        // Pass matrix data to the Lamp Shader program's matrix uniforms
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, gMesh.nFloorVertices);
    }
    
    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);
}


void UDraw(GLMesh& mesh) {

    glm::mat4 scale;
    glm::mat4 translation;
    glm::mat4 model;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gShapeProgramId, "model");

    //Floor
    //-----------------------------------------------------------------------------------------------------------
    mesh.nFloorVertices = sizeof(vert1) / (sizeof(vert1[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert1), vert1, GL_STATIC_DRAW);

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[0]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nFloorVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Oven Body
    //--------------------------------------------------------------------------------------------------------------
    mesh.nOvenBodyVertices = sizeof(vert2) / (sizeof(vert2[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert2), vert2, GL_STATIC_DRAW);

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[1]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    
    //Stove Top
    //--------------------------------------------------------------------------------------------------------------
    mesh.nStoveTopVertices = sizeof(vert3) / (sizeof(vert3[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert3), vert3, GL_STATIC_DRAW);

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[2]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nStoveTopVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Oven Window
    //----------------------------------------------------------------------------------------------------------------
    mesh.nOvenWindowVertices = sizeof(vert4) / (sizeof(vert4[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert4), vert4, GL_STATIC_DRAW);

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[3]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenWindowVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Oven Panel
    //---------------------------------------------------------------------------------------------------------------
    mesh.nOvenPanelVertices = sizeof(vert5) / (sizeof(vert5[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert5), vert5, GL_STATIC_DRAW);

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[1]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenPanelVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Panel Screen
    //-----------------------------------------------------------------------------------------------------------
    mesh.nClockVertices = sizeof(vert6) / (sizeof(vert6[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert6), vert6, GL_STATIC_DRAW);
    
    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[4]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nClockVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Left Counter
    //--------------------------------------------------------------------------------------------------------------
    mesh.nOvenBodyVertices = sizeof(vert2) / (sizeof(vert2[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert2), vert2, GL_STATIC_DRAW);

    // Scales the shape from its original size in all 3 dimensions
    scale = glm::scale(glm::vec3(1.0f, 1.9f, 2.0f));
    translation = glm::translate(glm::vec3(-1.5f, -0.28f, 0.0f));
    // Transformations are applied right-to-left order
    model = scale * translation;

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gShapeProgramId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[5]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    //Second cabinet
    //--------------
    mesh.nOvenBodyVertices = sizeof(vert2) / (sizeof(vert2[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert2), vert2, GL_STATIC_DRAW);

    // Scales the shape from its original size in all 3 dimensions
    scale = glm::scale(glm::vec3(1.0f, 1.9f, 2.0f));
    translation = glm::translate(glm::vec3(-2.5f, -0.28f, 0.0f));
    // Transformations are applied right-to-left order
    model = scale * translation;

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gShapeProgramId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[5]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Left Counter Top
    //--------------------------------------------------------------------------------------------------------------
    mesh.nCounterTopVertices = sizeof(vert7) / (sizeof(vert7[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert7), vert7, GL_STATIC_DRAW);

    // Scales the shape from its original size in all 3 dimensions
    scale = glm::scale(glm::vec3(5.8f, 2.5f, 2.0f));
    translation = glm::translate(glm::vec3(-1.0f, -8.13f, 0.0f));
    // Transformations are applied right-to-left order
    model = translation * scale;

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gShapeProgramId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[6]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Right Counter
    //--------------------------------------------------------------------------------------------------------------
    mesh.nOvenBodyVertices = sizeof(vert2) / (sizeof(vert2[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert2), vert2, GL_STATIC_DRAW);

    // Scales the shape from its original size in all 3 dimensions
    scale = glm::scale(glm::vec3(1.0f, 1.9f, 2.0f));
    translation = glm::translate(glm::vec3(1.5f, -0.28f, 0.0f));
    // Transformations are applied right-to-left order
    model = scale * translation;

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gShapeProgramId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[5]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    //second cabinet
    //--------------
    mesh.nOvenBodyVertices = sizeof(vert2) / (sizeof(vert2[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert2), vert2, GL_STATIC_DRAW);

    // Scales the shape from its original size in all 3 dimensions
    scale = glm::scale(glm::vec3(1.0f, 1.9f, 2.0f));
    translation = glm::translate(glm::vec3(2.5f, -0.28f, 0.0f));
    // Transformations are applied right-to-left order
    model = scale * translation;

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gShapeProgramId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[5]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Right Counter Top
    //--------------------------------------------------------------------------------------------------------------
    mesh.nCounterTopVertices = sizeof(vert7) / (sizeof(vert7[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert7), vert7, GL_STATIC_DRAW);

    // Scales the shape from its original size in all 3 dimensions
    scale = glm::scale(glm::vec3(5.8f, 2.5f, 2.0f));
    translation = glm::translate(glm::vec3(3.0f, -8.13f, 0.0f));
    // Transformations are applied right-to-left order
    model = translation * scale;

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gShapeProgramId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[6]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);


    //Range Light
    //--------------------------------------------------------------------------------------------------------------
    mesh.nOvenBodyVertices = sizeof(vert2) / (sizeof(vert2[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert2), vert2, GL_STATIC_DRAW);

    // Scales the shape from its original size in all 3 dimensions
    scale = glm::scale(glm::vec3(0.75f, 3.0f, 0.75f));
    translation = glm::translate(glm::vec3(0.0f, 3.25f, 0.0f));
    // Transformations are applied right-to-left order
    model = scale * translation;

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gShapeProgramId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[1]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    //Range Flair
    //-----------
    mesh.nOvenBodyVertices = sizeof(vert8) / (sizeof(vert8[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vert8), vert8, GL_STATIC_DRAW);

    // Scales the shape from its original size in all 3 dimensions
    scale = glm::scale(glm::vec3(2.0f, 0.5f, 2.0f));
    translation = glm::translate(glm::vec3(0.0f, -10.0f, 0.0f));
    // Transformations are applied right-to-left order
    model = scale * translation;

    // Retrieves and passes transform matrices to the Shader program
    modelLoc = glGetUniformLocation(gShapeProgramId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    UInitializeVAOs(gMesh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId[1]);

    // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nOvenBodyVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}


void UDestroyMesh(GLMesh& mesh) {

    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);

}


void UInitializeVAOs(GLMesh &mesh) {

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    // Create VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMesh.vao);
}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId) {

    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);

    if (image) {

        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        }
        
        else if (channels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        else {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
        
    }

    // Error loading the image
    return false;
    
}


void UDestroyTexture(GLuint textureId) {

    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId) {

    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId);
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId);
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);

    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);

    return true;
}


void UDestroyShaderProgram(GLuint programId) {

    glDeleteProgram(programId);

}