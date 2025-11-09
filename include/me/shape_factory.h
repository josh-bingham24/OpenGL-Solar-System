#ifndef SHAPE_FACTORY
#define SHAPE_FACTORY

#include <cglm/cglm.h>

typedef struct {
    float *vertices;
    float rotation;
    vec3 position;
    vec3 velocity;
    float mass;
} Shape;


Shape CreateCircle(float radius, unsigned int edges, vec3 initialPosition, vec3 initialVelocity, float mass);
Shape CreateRectangle(float width, float height, vec3 initialPosition);
void DestroyShape(Shape *shape);

#endif