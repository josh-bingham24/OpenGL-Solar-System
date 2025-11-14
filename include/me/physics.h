#ifndef PHYSICS_H
#define PHYSICS_H

#include <me/shape_factory.h>

typedef struct {
    Shape *bodies;
    int count;
} SolarSystem;

void InitializeSolarSystem(SolarSystem *solarSystem);
void AddBody(SolarSystem *solarSystem, Shape body);
void SimulateGravity(SolarSystem *solarSystem, float deltaTime);


#endif