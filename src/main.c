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
Shape sun;
Shape planet;

// float GRAVITY = -6.67408 * pow(10, -11);
float GRAVITY = -0.0001;

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


    // setup shaders
    Shader_init(&shader, "../src/shader_v.txt", "../src/shader_f.txt");

    // setup camera
    camera = Camera_init(STATIC, 60.0f, 0.1f);

    // setup sun
    sun = CreateCircle(0.5f, 36, (vec3){ 0.0f, 0.0f, 0.0f}, (vec3){ 0.0f, 0.0f, 0.0f}, 100);
    planet = CreateCircle(0.1f, 36, (vec3){ 3.0f, 0.0f, 0.0f}, (vec3){ 0.0f, 1.0f, 0.0f}, 1);

    float dist = glm_vec3_distance(sun.position, planet.position);
    float test = -GRAVITY * (sun.mass + planet.mass);
    float test2 = test / dist;
    float planetVelo = sqrt(test2) * 14;
    glm_vec3_copy((vec3){ 0.0f, planetVelo, 0.0f}, planet.velocity);
    printf("dist: %f\n", dist);
    printf("test: %f\n", test);
    printf("test2: %f\n", test2);
    printf("test2: %f\n", planetVelo);
    
    srand(time(0));

    // vertex handling
    unsigned int VBO, VBO2, VAO, VAO2;
    // glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO2);
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VAO2);

    // bind data
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * 5 * 36 * sizeof(float), sun.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, 3 * 5 * 36 * sizeof(float), planet.vertices, GL_STATIC_DRAW);
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


    // setup textures
    unsigned int texture, cheeseballTexture;
    unsigned char *data;
    int width, height, nrChannels;

    double lastTime = glfwGetTime();

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
        glm_ortho(-8.0f, 8.0f, -6.0f, 6.0f, 0.1f, 100.0f, projection);
        
        setMat4(&shader, "view", view);
        setMat4(&shader, "projection", projection);

        // ----- MOVE THE PLANET -----
        // calculate force vector of gravity
        vec3 gravityVec;
        glm_vec3_sub(planet.position, sun.position, gravityVec);

        float gravityForce = GRAVITY * sun.mass * planet.mass / glm_vec3_distance2(sun.position, planet.position);
        glm_vec3_scale(gravityVec, gravityForce, gravityVec);

        // Update position
        vec3 frameVelo;
        glm_vec3_add(planet.velocity, gravityVec, planet.velocity);
        glm_vec3_scale(planet.velocity, deltaTime, frameVelo);
        glm_vec3_add(planet.position, frameVelo, planet.position);

        // draw planet
        glBindVertexArray(VAO2);
        glm_mat4_identity(model);
        glm_translate(model, planet.position);
        setMat4(&shader, "model", model);
        glDrawArrays(GL_TRIANGLES, 0, 3 * 5 * 36);

        // draw sun
        glBindVertexArray(VAO);
        glm_mat4_identity(model);
        glm_translate(model, sun.position);
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
    float cameraSpeed = 2.5f * deltaTime;
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
