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
* @param void
*
* return: pointer to the particle in memory
*/
Particle* new_particle(void) {
    Particle* ptr = malloc(sizeof(Particle));
    if (ptr == NULL) {
        printf("malloc error...\n");
        return NULL;
    }
    
    ptr->displacement[0] = 0.0f;
    ptr->displacement[1] = 0.0f;
    ptr->velocity[0] = 1.0f;
    ptr->velocity[1] = 1.0f;
    ptr->radius = 1.0f;

    return ptr;
}

/**
* Function: main
*   contains logic for the main flow of the program
* 
* @param void 
*/
int main(void) {
    Particle* p1 = new_particle();
 
    free(p1);
    return EXIT_SUCCESS;
}
