#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#define TIME_STEP 1000 //ms
#define MIN_XY 0
#define MAX_XY 100

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
* @return: pointer to the particle in memory
*/
Particle* new_particle(void) 
{
    Particle* ptr = malloc(sizeof(Particle));
    if (ptr == NULL) //check if malloc has failed
    {
        printf("malloc error...\n");
        return NULL;
    }
    
    ptr->displacement[0] = 1.0f;
    ptr->displacement[1] = 1.0f;
    ptr->velocity[0] = 1.0f;
    ptr->velocity[1] = 1.0f;
    ptr->radius = 1.0f;

    return ptr;
}

/**
* Function: update_position
*   updates the x and y positions of the particle with respect to time
*   if the particle leaves the valid region velocity is negated

* @param particle: pointer to the particle being manipulated

* @return void 
 */
void update_position(Particle* particle) 
{
    particle->displacement[0] += (particle->velocity[0] / TIME_STEP);
    particle->displacement[1] += (particle->velocity[1] / TIME_STEP);

    if (particle->displacement[0] < MIN_XY || particle->dispalcement[0] < MIN_XY) 
        particle->velocity[0] = -particle->velocity;

    if (particle->displacement[1] < MIN_XY || particle->dispalcement[1] < MIN_XY) 
        particle->velocity[1] = -particle->velocity[1];
}

/**
* Function: main
*   contains logic for the main flow of the program
* 
* @param void 

* @return EXIT_SUCCESS if the program executes as expected
*/
int main(void) 
{
    Particle* p1 = new_particle();
 
    free(p1);
    return EXIT_SUCCESS;
}
