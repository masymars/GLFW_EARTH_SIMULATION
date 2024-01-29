#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>
#include "shader/shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include "model/objload.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"




using namespace glm;

std::vector<vec3> posion;
std::vector<vec2> texture;
std::vector<vec3> normals;

std::vector<vec3> moonPosition;
std::vector<vec2> moonTextureCoords;
std::vector<vec3> moonNormals;

struct STRVertex {
    vec3 position;
    vec3 couleur;
    vec2 texture;
};


void CreateSphere(int segments, float radius) {
    posion.clear();
    texture.clear();
    normals.clear();

    for (int i = 0; i <= segments; ++i) {
        float phi = pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
        for (int j = 0; j <= segments; ++j) {
            float theta = two_pi<float>() * static_cast<float>(j) / static_cast<float>(segments);

            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            float u = static_cast<float>(j) / static_cast<float>(segments);
            float v = static_cast<float>(i) / static_cast<float>(segments);

            posion.push_back(vec3(x, y, z));
            texture.push_back(vec2(u, v));
            normals.push_back(normalize(vec3(x, y, z)));
        }
    }

    // Generating triangle vertices for each quad on the sphere
    std::vector<vec3> tempVertices;
    std::vector<vec2> tempTextures;
    std::vector<vec3> tempNormals;

    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            int first = (i * (segments + 1)) + j;
            int second = first + segments + 1;

            tempVertices.push_back(posion[first]);
            tempVertices.push_back(posion[second]);
            tempVertices.push_back(posion[first + 1]);

            tempTextures.push_back(texture[first]);
            tempTextures.push_back(texture[second]);
            tempTextures.push_back(texture[first + 1]);

            tempNormals.push_back(normals[first]);
            tempNormals.push_back(normals[second]);
            tempNormals.push_back(normals[first + 1]);

            tempVertices.push_back(posion[second]);
            tempVertices.push_back(posion[second + 1]);
            tempVertices.push_back(posion[first + 1]);

            tempTextures.push_back(texture[second]);
            tempTextures.push_back(texture[second + 1]);
            tempTextures.push_back(texture[first + 1]);

            tempNormals.push_back(normals[second]);
            tempNormals.push_back(normals[second + 1]);
            tempNormals.push_back(normals[first + 1]);
        }
    }

    posion = tempVertices;
    texture = tempTextures;
    normals = tempNormals;
}



void CreateSphere2(int segments, float radius) {
    moonPosition.clear();
    moonTextureCoords.clear();
    moonNormals.clear();

    for (int i = 0; i <= segments; ++i) {
        float phi = pi<float>() * static_cast<float>(i) / static_cast<float>(segments);
        for (int j = 0; j <= segments; ++j) {
            float theta = two_pi<float>() * static_cast<float>(j) / static_cast<float>(segments);

            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            float u = static_cast<float>(j) / static_cast<float>(segments);
            float v = static_cast<float>(i) / static_cast<float>(segments);

            moonPosition.push_back(vec3(x, y, z));
            moonTextureCoords.push_back(vec2(u, v));
            moonNormals.push_back(normalize(vec3(x, y, z)));
        }
    }

    // Generating triangle vertices for each quad on the sphere
    std::vector<vec3> tempVertices2;
    std::vector<vec2> tempTextures2;
    std::vector<vec3> tempNormals2;

    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            int first = (i * (segments + 1)) + j;
            int second = first + segments + 1;

            tempVertices2.push_back(moonPosition[first]);
            tempVertices2.push_back(moonPosition[second]);
            tempVertices2.push_back(moonPosition[first + 1]);

            tempTextures2.push_back(moonTextureCoords[first]);
            tempTextures2.push_back(moonTextureCoords[second]);
            tempTextures2.push_back(moonTextureCoords[first + 1]);

            tempNormals2.push_back(moonNormals[first]);
            tempNormals2.push_back(moonNormals[second]);
            tempNormals2.push_back(moonNormals[first + 1]);

            tempVertices2.push_back(moonPosition[second]);
            tempVertices2.push_back(moonPosition[second + 1]);
            tempVertices2.push_back(moonPosition[first + 1]);

            tempTextures2.push_back(moonTextureCoords[second]);
            tempTextures2.push_back(moonTextureCoords[second + 1]);
            tempTextures2.push_back(moonTextureCoords[first + 1]);

            tempNormals2.push_back(moonNormals[second]);
            tempNormals2.push_back(moonNormals[second + 1]);
            tempNormals2.push_back(moonNormals[first + 1]);
        }
    }

    moonPosition = tempVertices2;
    moonTextureCoords = tempTextures2;
    moonNormals = tempNormals2;
}


// global variables
mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
mat4 View = lookAt(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0));
mat4 Model = mat4(1.0f);
double Xpos, Ypos;

float zoomFactor = 1.0f;

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    zoomFactor += static_cast<float>(yoffset) * 0.1f;
    zoomFactor = glm::clamp(zoomFactor, 0.1f, 10.0f);
}
bool leftMouseButtonDown = false;
vec2 lastMousePos = vec2(0, 0);

float userRotationOffsetX = 0.0f;
float userRotationOffsetY = 0.0f;
float automaticRotationAngle = 0.0f; // in degrees


void mouse_button(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMouseButtonDown = true;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastMousePos = vec2(xpos, ypos);
        } else if (action == GLFW_RELEASE) {
            leftMouseButtonDown = false;
        }
    }
}
bool isNight = false;
////cameras///////////

// Global variables
vec3 earthCameraPosition = vec3(0, 0, 5); // Base position for Earth
vec3 moonCameraPosition = vec3(2.5, 0, 2.5); // Base position for Moon
vec3 cameraTargetEarth = vec3(0, 0, 0);
vec3 cameraTargetMoon = vec3(2.0, 0, 0); // Adjust based on Moon's position
vec3 cameraOffsetFromMoon = vec3(0, 0, 1.5); 

// Declare global variables for camera control
vec3 cameraPosition = vec3(0.0f, 0.0f, 5.0f);  // Initial camera position
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);     // Initial camera front direction
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);          // Initial camera up vector

float cameraSpeed = 0.05f;  // Adjust as needed
float mouseSensitivity = 0.1f;  // Adjust as needed

double lastMouseX, lastMouseY;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;

bool focusOnMoon = false; // Toggle between Earth and Moon
bool focusOnspace = false;
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_1:
                focusOnMoon = true;
                focusOnspace = false;
                break;
            case GLFW_KEY_2:
                focusOnMoon = false;
                focusOnspace = false;
                break;
             case GLFW_KEY_3:
                focusOnMoon = false;
                focusOnspace = true;
                break;
            case GLFW_KEY_N:
                isNight = !isNight; // Toggle between day and night
                break;
            case GLFW_KEY_W:
            if(focusOnspace){
                                 cameraPosition += cameraSpeed * cameraFront;

            }
                
                break;
            case GLFW_KEY_S:
                 if(focusOnspace){
                                cameraPosition -= cameraSpeed * cameraFront;

            }
                break;
            case GLFW_KEY_A:
                 if(focusOnspace){
                                  cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;


            }
                break;
            case GLFW_KEY_D:
                 if(focusOnspace){
                              cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

            }
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            default:
                glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
        }
    } else if (action == GLFW_RELEASE) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
}
static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(focusOnspace){
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;  // Reversed since y-coordinates range from bottom to top

    lastMouseX = xpos;
    lastMouseY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch to avoid camera flipping
    if (pitch > 89.0f){
        pitch = 89.0f;}
    if (pitch < -89.0f){
        pitch = -89.0f;}

     glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
     cameraFront = glm::normalize(front);

     }else{
         if (leftMouseButtonDown) {
        vec2 currentMousePos = vec2(xpos, ypos);
        vec2 delta = (currentMousePos - lastMousePos) * 0.05f; // Sensitivity factor
        lastMousePos = currentMousePos;
        
        userRotationOffsetX += radians(delta.x);
        userRotationOffsetY += radians(delta.y);

        // Reset automatic rotation when the user is dragging
        automaticRotationAngle = 0.0f;
    }}
}
GLuint text; // une seule texture
GLuint text2; // une seule texture
GLuint normalMapTexture;
GLuint moonTexture;
void InitTexture(void)
{
char data[128*128*3];
FILE *f = fopen("C:/textures/herbe.raw", "rb");
if(f)
{
fread(data, 128*128*3, 1, f);
fclose(f);
glGenTextures(1, &text);
glBindTexture(GL_TEXTURE_2D, text);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

}
}

// Load and bind the skybox shader program
GLuint skyboxShaderProgram;

// Load and bind the main shader program
GLuint mainShaderProgram;

// Skybox vertices for a cube
// Skybox vertices for a cube (scaled by 10)
float skyboxVertices[] = {
    // Positions
    -50.0f,  50.0f, -50.0f,
    -50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,

    -50.0f, -50.0f,  50.0f,
    -50.0f, -50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f,  50.0f,
    -50.0f, -50.0f,  50.0f,

     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,

    -50.0f, -50.0f,  50.0f,
    -50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
    -50.0f, -50.0f,  50.0f,
     50.0f, -50.0f,  50.0f,

    -50.0f,  50.0f, -50.0f,
     50.0f,  50.0f, -50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
    -50.0f,  50.0f,  50.0f,
    -50.0f,  50.0f, -50.0f,

    -50.0f, -50.0f, -50.0f,
    -50.0f, -50.0f,  50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
    -50.0f, -50.0f,  50.0f,
     50.0f, -50.0f,  50.0f
};

GLuint loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}



float earthRotationAngle = 0.0f; // in degrees

float moonOrbitRadius = 15.0f;
float moonOrbitSpeed = 0.01f;
float moonOrbitAngle = 0.0f; 




double sensitivity = 0.5f;
int main(void) {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
GLFWmonitor* monitor = glfwGetPrimaryMonitor();
const GLFWvidmode* mode = glfwGetVideoMode(monitor);

window = glfwCreateWindow(mode->width, mode->height, "Earth", monitor, NULL);


    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Could not initialize GLAD");
        return -1;
    }

   /* // Load OBJ model
    if (!loadOBJ("C:/Users/masy/Desktop/model/model/Earth3.obj", posion, texture, normals)) {
        std::cerr << "Error loading OBJ file." << std::endl;
        return -1;
    }*/
  /////////////////////////////////moon///////////////
float moonRadius = 0.27f; // Moon's radius is about 27% of Earth's

  CreateSphere2(100, moonRadius);



glGenTextures(1, &moonTexture);
    glBindTexture(GL_TEXTURE_2D, moonTexture);





 int texWidthmoon, texHeightmoon, texChannelsmoon;
    unsigned char* texDatamoon = stbi_load("C:/Users/masy/Desktop/model/model/moon.jpg", &texWidthmoon, &texHeightmoon, &texChannelsmoon, 0);
  if (texDatamoon) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidthmoon, texHeightmoon, 0, GL_RGB, GL_UNSIGNED_BYTE, texDatamoon);

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(texDatamoon);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }



GLuint moonVAO, moonVBO, moonTexCoordVBO, moonNormalVBO;

// Generate and bind the VAO for the Moon
glGenVertexArrays(1, &moonVAO);
glBindVertexArray(moonVAO);

// Generate and bind VBO for Moon vertices
glGenBuffers(1, &moonVBO);
glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
glBufferData(GL_ARRAY_BUFFER, moonPosition.size() * sizeof(vec3), &moonPosition[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(0);

// Generate and bind VBO for Moon texture coordinates
glGenBuffers(1, &moonTexCoordVBO);
glBindBuffer(GL_ARRAY_BUFFER, moonTexCoordVBO);
glBufferData(GL_ARRAY_BUFFER, moonTextureCoords.size() * sizeof(vec2), &moonTextureCoords[0], GL_STATIC_DRAW);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(1);

// Generate and bind VBO for Moon normals
glGenBuffers(1, &moonNormalVBO);
glBindBuffer(GL_ARRAY_BUFFER, moonNormalVBO);
glBufferData(GL_ARRAY_BUFFER, moonNormals.size() * sizeof(vec3), &moonNormals[0], GL_STATIC_DRAW);
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);

// Unbind the VAO to prevent accidental modification
glBindVertexArray(0);
    GLuint moonShaderProgram;
    moonShaderProgram = LoadShaders("C:/shader/moontextureVertexShader.vertexshader",
                                "C:/shader/moontextureFragmentShader.fragmentshader");

  ///////////////////////earth///////////////////////
  
    CreateSphere(100, 1.0f);
///////////////////////////////////////////////////

glGenTextures(1, &text2);
    glBindTexture(GL_TEXTURE_2D, text2);





 int texWidth2, texHeight2, texChannels2;
    unsigned char* texData2 = stbi_load("C:/Users/masy/Desktop/model/model/2k_earth_nightmap.jpg", &texWidth2, &texHeight2, &texChannels2, 0);
  if (texData2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth2, texHeight2, 0, GL_RGB, GL_UNSIGNED_BYTE, texData2);

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(texData2);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

/////////////////////////////////
    glGenTextures(1, &text);
    glBindTexture(GL_TEXTURE_2D, text);

   




 int texWidth, texHeight, texChannels;
    unsigned char* texData = stbi_load("C:/Users/masy/Desktop/model/model/earth2048.bmp", &texWidth, &texHeight, &texChannels, 0);
  if (texData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(texData);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
   /////////////////////////normal map///////////////////
      
  ////////////////////////////

    // Print the number of vertices loaded
    std::cout << "Number of vertices: " << posion.size() << std::endl;
    
     GLuint VAO;
    GLuint VBO;
    //GLuint TBO;
    GLuint NBO;
GLuint texCoordBuffer;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, posion.size() * sizeof(vec3), &posion[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

   

    glGenBuffers(1, &texCoordBuffer);
glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
glBufferData(GL_ARRAY_BUFFER, texture.size() * sizeof(vec2), &texture[0], GL_STATIC_DRAW);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(1);

    glGenBuffers(1, &NBO);
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);


    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(STRVertex), (void*)offsetof(STRVertex, texture));

    glBindVertexArray(0);


    GLuint ShaderProgram;
    ShaderProgram = LoadShaders("C:/shader/textureVertexShader.vertexshader",
                                "C:/shader/textureFragmentShader.fragmentshader");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwSetKeyCallback(window, key_callback);

    glfwSetMouseButtonCallback(window, mouse_button);

        glfwSetScrollCallback(window, scroll_callback);
glfwSetCursorPosCallback(window, mouse_callback);
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  

//GLuint LightClrID = glGetUniformLocation(ShaderProgram, "lightColor");
glBindVertexArray(VAO);
glDrawElements(GL_TRIANGLES, posion.size(), GL_UNSIGNED_INT, 0);

//InitTexture();

////////////////////////skybox ////////////////////
GLuint skyboxShaderProgram = LoadShaders("C:/shader/skyboxVertexShader.vertexshader", "C:/shader/skyboxFragmentShader.fragmentshader");
// Create and set up the skybox VAO and VBO
    GLuint skyboxVAO, skyboxVBO;
 

    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);

       glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(skyboxVertices) * sizeof(vec3),  &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

   std::vector<std::string> faces = {
        "C:/Users/masy/Desktop/model/model/right.png", "C:/Users/masy/Desktop/model/model/left.png",
        "C:/Users/masy/Desktop/model/model/top.png", "C:/Users/masy/Desktop/model/model/bottom.png",
        "C:/Users/masy/Desktop/model/model/front.png", "C:/Users/masy/Desktop/model/model/back.png"
    };

    GLuint cubemapTexture = loadCubemap(faces);
    glGenBuffers(1, &cubemapTexture);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapTexture);
    glBufferData(GL_ARRAY_BUFFER, texture.size() * sizeof(vec2), &texture[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
     ///////////////////////
     vec3 lightPos(10.0f, 10.0f, 0.5f);
     GLuint LightPosID = glGetUniformLocation(ShaderProgram, "lightPos");
    

     vec3 lightColor(1.0f, 0.6f, 1.0f) ;
     GLuint LightClrID = glGetUniformLocation(ShaderProgram, "lightColor");
     glUniform3fv(LightClrID, 1, &lightColor[0]);


 
     GLuint LightClrID2 = glGetUniformLocation(moonShaderProgram, "lightColor");
     glUniform3fv(LightClrID2, 1, &lightColor[0]);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         if (!leftMouseButtonDown) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastMousePos = vec2(xpos, ypos);
         // Update the automatic rotation angle
   
    }
    //////////////////moon rotation /////////

         moonOrbitAngle += moonOrbitSpeed;
    if (moonOrbitAngle > 360.0f){moonOrbitAngle -= 360.0f;} 

    
    glm::mat4 ModelMoon = glm::rotate(glm::mat4(1.0f), glm::radians(moonOrbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    ModelMoon = glm::translate(ModelMoon, glm::vec3(moonOrbitRadius, 0.0f, 0.0f));
    ModelMoon = glm::scale(ModelMoon, glm::vec3(moonRadius)); // Scale down the Moon
    glm::mat4 MVPMoon = Projection * View * ModelMoon;
     glm::vec3 wheremoonPosition = glm::vec3(ModelMoon[3]);

           //////camera////

    if (focusOnMoon) {
        // Calculate camera position relative to Moon
        vec3 cameraPositionRelativeToMoon = wheremoonPosition + cameraOffsetFromMoon;
        View = lookAt(cameraPositionRelativeToMoon  * zoomFactor, wheremoonPosition, vec3(0, 1, 0));
    } else {
         if (focusOnspace) {
       View = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

    }else{
        // Camera position focused on Earth
        vec3 adjustedEarthCameraPosition = earthCameraPosition * zoomFactor;
        View = lookAt(adjustedEarthCameraPosition, cameraTargetEarth, vec3(0, 1, 0));}
    }


           ///////////rotation of the earth///////////////
 automaticRotationAngle += 0.1f;
    if (automaticRotationAngle > 360.0f) {
        automaticRotationAngle -= 360.0f;
    }

    // Combine user rotation offset and automatic rotation
    mat4 userRotation = glm::rotate(glm::rotate(mat4(1.0f), userRotationOffsetY, vec3(1.0f, 0.0f, 0.0f)), userRotationOffsetX, vec3(0.0f, 1.0f, 0.0f));
    mat4 autoRotation = glm::rotate(mat4(1.0f), radians(automaticRotationAngle), vec3(0.0f, 1.0f, 0.0f));
    Model = userRotation * autoRotation;
        
        // Update the View matrix with zoom factor
        //View = lookAt(vec3(0, 0, 5 * zoomFactor), vec3(0, 0, 0), vec3(0, 1, 0));

        mat4 MVP = Projection * View * Model;
        GLuint MatrixID = glGetUniformLocation(ShaderProgram, "MVP");

GLuint ModelID = glGetUniformLocation(ShaderProgram, "Model");
///////////////////////skybox////////////////
    mat4 view = glm::mat4(glm::mat3(View)); // Remove translation from the view matrix
    mat4 skyboxMVP = Projection * view;
       glDepthFunc(GL_LEQUAL);  
       glUseProgram(skyboxShaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "projection"), 1, GL_FALSE, &Projection[0][0]);
        
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glUniform1i(glGetUniformLocation(skyboxShaderProgram, "skybox"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
  ////////////////////////moon/////////////////////

   


    // Use Moon shader program (if different) and set uniforms
    glUseProgram(moonShaderProgram); // If you have a separate shader for the Moon
    glUniformMatrix4fv(glGetUniformLocation(moonShaderProgram, "MVP"), 1, GL_FALSE, &MVPMoon[0][0]);
    
    // Bind Moon texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, moonTexture);
    glUniform1i(glGetUniformLocation(moonShaderProgram, "mytexture"), 0);

    glBindVertexArray(moonVAO);

 glEnableVertexAttribArray(0); // Udostepnienie zmiennej Vertex Shadera => vertexPosition_modelspace
      glBindBuffer(GL_ARRAY_BUFFER,moonVBO);
      glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER,moonTexCoordVBO);
      glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);
      // Normals attribute (if used in your shader)
glEnableVertexAttribArray(2);
glBindBuffer(GL_ARRAY_BUFFER, moonNormalVBO);
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        
        
        glUniformMatrix4fv(glGetUniformLocation(moonShaderProgram, "MVP"), 1, GL_FALSE, &MVPMoon[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(moonShaderProgram, "Model"), 1, GL_FALSE, &ModelMoon[0][0]);
        // Bind Moon VAO and draw
   
    glDrawArrays(GL_TRIANGLES, 0, moonPosition.size());

        glUniform3fv(glGetUniformLocation(moonShaderProgram, "lightPos"), 1, &lightPos[0]); 
        glBindVertexArray(0); // Unbind VAO

  /////////////////////earth//////////////////
        glUseProgram(ShaderProgram);


    GLuint currentTexture = isNight ? text2 : text; // text is day, text2 is night

         //texture
       glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, currentTexture);
glUniform1i(glGetUniformLocation(ShaderProgram, "mytexture"), 0);



      glEnableVertexAttribArray(0); // Udostepnienie zmiennej Vertex Shadera => vertexPosition_modelspace
      glBindBuffer(GL_ARRAY_BUFFER,VBO);
      glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER,text);
      glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);
        
        glBindVertexArray(VAO);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, posion.size());

        glUniform3fv(LightPosID, 1, &lightPos[0]); 

        //////glUniform3fv(LightClrID, 1, &lightColor[0]);
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL Error: %d\n", error);
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &NBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
