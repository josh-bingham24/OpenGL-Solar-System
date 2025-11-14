#include <me/physics.h>
#include <stdlib.h>

// float GRAVITY = -6.67408 * pow(10, -11);
float GRAVITY = -0.01;

void CalculateAccelerationVectors(SolarSystem *solarSystem, vec3 *accelerationVecs);
void UpdatePositions(SolarSystem *solarSystem, vec3 *accelerationVecs, float deltaTime);


void InitializeSolarSystem(SolarSystem *solarSystem) {
    solarSystem->bodies = NULL;
    solarSystem->count = 0;
}

void AddBody(SolarSystem *solarSystem, Shape body) {
    solarSystem->count++;

    solarSystem->bodies = realloc(solarSystem->bodies, sizeof(Shape) * solarSystem->count);
    solarSystem->bodies[solarSystem->count - 1] = body;
}


void SimulateGravity(SolarSystem *solarSystem, float deltaTime) {
    // get new acceleration for each object
    vec3 accelerationVecs[solarSystem->count];
    CalculateAccelerationVectors(solarSystem, accelerationVecs);

    // update object velocities and positions
    UpdatePositions(solarSystem, accelerationVecs, deltaTime);
}

void CalculateAccelerationVectors(SolarSystem *solarSystem, vec3 *accelerationVecs) {
    // naive implementation for now :(
    for (unsigned int i = 0; i < solarSystem->count; i++) {
        Shape currentBody = solarSystem->bodies[i];
        vec3 netGravityForce;
        glm_vec3_zero(netGravityForce);

        for (unsigned int j = 0; j < solarSystem->count; j++) {
            if (j == i) continue;

            Shape affectingBody = solarSystem->bodies[j];

            // calculate the gravitational force on the two bodies
            vec3 r, gravityVec;
            float gravityForce;
            glm_vec3_sub(currentBody.position, affectingBody.position, r);
            gravityForce = GRAVITY * affectingBody.mass * currentBody.mass / pow(glm_vec3_norm(r), 3);
            glm_vec3_scale(r, gravityForce, gravityVec);

            // add to net force
            glm_vec3_add(netGravityForce, gravityVec, netGravityForce);
        }

        // get acceleration
        vec3 acceleration;
        glm_vec3_scale(netGravityForce, 1.0f / currentBody.mass, acceleration);
        glm_vec3_copy(acceleration, accelerationVecs[i]);
    }
}

void UpdatePositions(SolarSystem *solarSystem, vec3 *accelerationVecs, float deltaTime) {
    for (unsigned int i = 0; i < solarSystem->count; i++) {
        // update velocity
        vec3 scaledAcceleration;
        glm_vec3_scale(accelerationVecs[i], deltaTime, scaledAcceleration);
        glm_vec3_add(solarSystem->bodies[i].velocity, scaledAcceleration, solarSystem->bodies[i].velocity);

        // update position
        vec3 scaledVelocity;
        glm_vec3_scale(solarSystem->bodies[i].velocity, deltaTime, scaledVelocity);
        glm_vec3_add(solarSystem->bodies[i].position, scaledVelocity, solarSystem->bodies[i].position);
    }
}

