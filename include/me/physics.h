#ifndef PHYSICS_H
#define PHYSICS_H

#include <me/shape_factory.h>

typedef struct {
    Shape *bodies;
    int count;
} SolarSystem;

void InitializeSolarSystem(SolarSystem *solarSystem);
void AddBodyWithOrbit(SolarSystem *solarSystem, Shape bodyToOrbit, float radius, vec3 position, float mass);
void AddBody(SolarSystem *solarSystem, float radius, vec3 position, float mass);
void SimulateGravity(SolarSystem *solarSystem, float deltaTime);


#endif