#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader.h>
#include <stb_image.h>
#include <learnopengl/camera.h>
#include <time.h>

// me
#include <me/shape_factory.h>
#include <me/physics.h>

// glm
#include <cglm/cglm.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// ----- Shader -----
Shader shader;

// ----- Camera -----
Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;

// ----- Shapes ----
SolarSystem solarSystem;
Shape sun;
Shape planet;
Shape moon;

// float GRAVITY = -6.67408 * pow(10, -11);
// float GRAVITY = -0.0001;

// ----- Time -----
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // initialize window
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }    

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback); 

    // setup shaders
    Shader_init(&shader, "../src/shader_v.txt", "../src/shader_f.txt");

    // setup camera
    camera = Camera_init(STATIC, 300.0f, 400.0f, 60.0f, 0.1f);

    // setup solar system
    InitializeSolarSystem(&solarSystem);

    sun = CreateCircle(50.0f, 36, (vec3){ 0.0f, 0.0f, 0.0f}, (vec3){ 0.0f, 0.0f, 0.0f}, 10000000);
    AddBody(&solarSystem, sun);

    planet = CreateCircle(5.0f, 36, (vec3){ 150.0f, 0.0f, 0.0f}, (vec3){ 0.0f, 25.0f, 0.0f}, 1000);
    AddBody(&solarSystem, planet);

    moon = CreateCircle(1.0f, 36, (vec3){ 140.0f, 0.0f, 0.0f}, (vec3){ -10.0f, 25.0f, 0.0f}, 50);
    AddBody(&solarSystem, moon);
    
    srand(time(0));

    // vertex handling
    unsigned int VBO, VBO2, VBO3, VAO, VAO2, VAO3;
    // glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &VBO3);
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VAO2);
    glGenVertexArrays(1, &VAO3);

    // bind data
    //sun
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * 5 * 36 * sizeof(float), sun.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // planet
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, 3 * 5 * 36 * sizeof(float), planet.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // moon
    glBindVertexArray(VAO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, 3 * 5 * 36 * sizeof(float), moon.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // enable z-buffer
    glEnable(GL_DEPTH_TEST); 

    // enable mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // setup textures
    unsigned int texture, cheeseballTexture;
    unsigned char *data;
    int width, height, nrChannels;

    double lastTime = glfwGetTime();
    float timeScale = 1;

    // main loop
    while(!glfwWindowShouldClose(window))
    {   
        // update delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        processInput(window);

        // clear
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // shader config
        useShader(&shader);
        // setInt(&shader, "texture1", 0);

        // transforms
        mat4 model, view, projection;

        // create view
        glm_look(camera.position, camera.front, camera.up, view);
        glm_ortho(-camera.width, camera.width, -camera.height, camera.height, 0.1f, 100.0f, projection);
        
        setMat4(&shader, "view", view);
        setMat4(&shader, "projection", projection);

        // calculates all gravitational forces and updates velocities and positions
        SimulateGravity(&solarSystem, deltaTime);

        // draw sun
        glBindVertexArray(VAO);
        glm_mat4_identity(model);
        glm_translate(model, solarSystem.bodies[0].position);
        setMat4(&shader, "model", model);
        glDrawArrays(GL_TRIANGLES, 0, 3 * 5 * 36);

        // draw planet
        glBindVertexArray(VAO2);
        glm_mat4_identity(model);
        glm_translate(model, solarSystem.bodies[1].position);
        setMat4(&shader, "model", model);
        glDrawArrays(GL_TRIANGLES, 0, 3 * 5 * 36);

        // draw moon
        glBindVertexArray(VAO3);
        glm_mat4_identity(model);
        glm_translate(model, solarSystem.bodies[2].position);
        setMat4(&shader, "model", model);
        glDrawArrays(GL_TRIANGLES, 0, 3 * 5 * 36);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    HandleScroll(&camera, yoffset);
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

    HandleMouse(&camera, xoffset, yoffset);
}

void processInput(GLFWwindow* window)
{
    // close window on escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // camera movement
    float cameraSpeed = camera.height * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { 
        HandleMovement(&camera, CAM_UP, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        HandleMovement(&camera, CAM_DOWN, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        HandleMovement(&camera, CAM_LEFT, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        HandleMovement(&camera, CAM_RIGHT, cameraSpeed);
    }
}
