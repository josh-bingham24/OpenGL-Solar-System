#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <learnopengl/camera.h>


void HandleMovement(Camera *camera, Direction direction, float speed) {
    vec3 temp;

    switch (direction) {
        case CAM_UP:
            glm_vec3_scale(camera->up, speed, temp);
            glm_vec3_add(camera->position, temp, camera->position); 
            break;
        case CAM_DOWN:
            glm_vec3_scale(camera->up, speed, temp);
            glm_vec3_sub(camera->position, temp, camera->position);
            break;
        case CAM_LEFT:
            glm_vec3_cross(camera->front, camera->up, temp);
            glm_vec3_normalize(temp);
            glm_vec3_scale(temp, speed, temp);
            glm_vec3_sub(camera->position, temp, camera->position);
            break;
        case CAM_RIGHT:
            glm_vec3_cross(camera->front, camera->up, temp);
            glm_vec3_normalize(temp);
            glm_vec3_scale(temp, speed, temp);
            glm_vec3_add(camera->position, temp, camera->position);
            break;
        default:
            printf("Invalid direction");
            break;
    }
}

Camera Camera_init(CameraType type, float height, float width, float fov, float sensitivity) {
    Camera cam;
    switch (type) {
        case STATIC:
            cam = (Camera){
                .position       = { 0.0f, 0.0f, 3.0f },
                .front          = { 0.0f, 0.0f, -1.0f },
                .up             = { 0.0f, 1.0f, 0.0f },
                .yaw            = -90.0f,
                .pitch          = 0.0f,
                .height         = height,
                .width          = width,
                .aspectx        = 4,
                .aspecty        = 3,
                .fov            = fov,
                .Move           = NULL
            };
            break;
        case FLY:
            cam = (Camera){
                .position       = { 0.0f, 0.0f, 3.0f },
                .front          = { 0.0f, 0.0f, -1.0f },
                .up             = { 0.0f, 1.0f, 0.0f },
                .yaw            = -90.0f,
                .pitch          = 0.0f,
                .fov            = fov,
                .sensitivity    = sensitivity,
                .Move           = HandleMovement
            };
            break;
        case PLAYER:
            break;
        default:
            printf("Unsupported camera type");
            break;
    }

    return cam;
}

void HandleMouse(Camera *camera, double xoffset, double yoffset) {
    xoffset *= camera->sensitivity;
    yoffset *= camera->sensitivity;

    camera->yaw   += xoffset;
    camera->pitch += yoffset;

    if(camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if(camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    vec3 direction = { 
        cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
        sin(glm_rad(camera->pitch)),
        sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch))
    };

    glm_vec3_normalize(direction);
    glm_vec3_copy(direction, camera->front);
}

void HandleScroll(Camera *camera, double yoffset) {
    yoffset *= 3;

    if (yoffset + camera->height <= 0)
        return;

    double xoffset = yoffset * camera->aspectx / camera->aspecty;
    camera->height += yoffset;
    camera->width += xoffset;
}
