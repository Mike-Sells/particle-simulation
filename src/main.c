#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/**
* Structure: Particle
*   stores the values for each particle 
*
* float arrays will hold directional x, y information of the two particles
* float radius determins the size of the shown particle
*/

typedef struct {
    float[2] displacement;
    float[2] velocity;
    float radius;
} Particle;

/**
* Function: new_particle
*   creates and instance of the structure particle
*
*
*
*/

Particle* new_particle(int particle_ptr) {
    Particle* particle_ptr = malloc(sizeof(Particle));
    if (p1 == NULL) {
        printf("malloc error...");
    } else {
        p1->displacement = {0,0};
        p1->velocity = {1,1};
        p1->radius = 1;
    }
    return particle_ptr;
}

int main(void) {
   
    return 0;
}