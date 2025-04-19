#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/**
* Structure: Particle
*   stores the values for each particle 
*
* float arrays will hold directional x, y information of the two particles
* float radius determines the size of the shown particle
*/

typedef struct {
    float displacement[2];
    float velocity[2];
    float radius;
} Particle;

/**
* Function: new_particle
*   creates an instance of the structure Particle
*
* return: pointer to the particle in memory
*/
Particle* new_particle(void) {
    Particle* particle_ptr = malloc(sizeof(Particle));
    if (particle_ptr == NULL) {
        printf("malloc error...\n");
        return NULL;
    }
    
    particle_ptr->displacement[0] = 0.0f;
    particle_ptr->displacement[1] = 0.0f;
    particle_ptr->velocity[0] = 1.0f;
    particle_ptr->velocity[1] = 1.0f;
    particle_ptr->radius = 1.0f;

    return particle_ptr;
}

int main(void) {
    Particle* p1 = new_particle();
    if (p1 != NULL) {
        printf("Particle created!\n");
        printf("Displacement: (%f, %f)\n", p1->displacement[0], p1->displacement[1]);
        printf("Velocity: (%f, %f)\n", p1->velocity[0], p1->velocity[1]);
        printf("Radius: %f\n", p1->radius);
    }

    free(p1);
    return 0;
}
