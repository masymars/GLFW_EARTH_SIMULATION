#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>
#include "shader/shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include "model/objload.hpp"

using namespace glm;

std::vector<vec3> posion;
std::vector<vec2> texture;
std::vector<vec3> normals;

struct STRVertex {
    vec3 position;
    vec3 couleur;
};

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

static void cursor_position(GLFWwindow* window, double x, double y) {
    double diffX = Xpos - x;
    double diffY = y - Ypos;
    Model = rotate(Model, radians(float(diffX)), vec3(0.0f, 0.0f, 1.0f));
    Model = rotate(Model, radians(float(diffY)), vec3(1.0f, 0.0f, 0.0f));
}

void mouse_button(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            // Enlarge the object
            Model = scale(Model, vec3(1.5f, 1.5f, 1.5f));
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            // Shrink the object
            Model = scale(Model, vec3(0.5f, 0.5f, 0.5f));
        }
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_R:
                glClearColor(1.0f, 0.0f, 0.0f, 0.5f);
                break;
            case GLFW_KEY_V:
                glClearColor(0.0f, 1.0f, 0.0f, 0.5f);
                break;
            case GLFW_KEY_B:
                glClearColor(0.0f, 0.0f, 1.0f, 0.5f);
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

int main(void) {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Could not initialize GLAD");
        return -1;
    }

    // Load OBJ model
    if (!loadOBJ("C:/Users/masy/Desktop/model/model/torus.obj", posion, texture, normals)) {
        std::cerr << "Error loading OBJ file." << std::endl;
        return -1;
    }

    // Print the number of vertices loaded
    std::cout << "Number of vertices: " << posion.size() << std::endl;

    GLuint VAO;
    GLuint VBO;
    GLuint VBO_normals; // New buffer for normals

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, posion.size() * sizeof(vec3), &posion[0], GL_STATIC_DRAW);

    glGenBuffers(1, &VBO_normals);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    // Normal attribute
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    GLuint ShaderProgram;
    ShaderProgram = LoadShaders("C:/shader/SimpleVertexShader.vertexshader",
                                "C:/shader/SimpleFragmentShader.fragmentshader");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position);
    glfwSetMouseButtonCallback(window, mouse_button);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    ///////////////////////
    glGenBuffers(1, &VBONormal);
    glBindBuffer(GL_ARRAY_BUFFER, VBONormal);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);



    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetCursorPos(window, &Xpos, &Ypos);

        // Update the View matrix with zoom factor
        View = lookAt(vec3(0, 0, 5 * zoomFactor), vec3(0, 0, 0), vec3(0, 1, 0));

        mat4 MVP = Projection * View * Model;
        GLuint MatrixID = glGetUniformLocation(ShaderProgram, "MVP");

        glUseProgram(ShaderProgram);

        glBindVertexArray(VAO);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, posion.size());
       glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,3 * sizeof(GLfloat), (void*)0 );
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL Error: %d\n", error);
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_normals);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
