#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h> // Graphics lib

#define FPS 240
#define NUMBER_OF_PARTICLES 10 
#define GRAVITATIONAL_ACCELERATION 9.81f // acceleration due to gravity (earths gravitational constant)
#define RADIUS 10 // radius of each particle in pixels
#define MIN_XY 0
#define MAX_XY 800

/**
* struct: Particle
* 
* Defines kinematic attributes of the particle
*/
typedef struct {
    float displacement[2];
    float velocity[2];
} Particle;

/**
* Function: new_particle
*
* Generates a pointer to a distinct particle with random displacement and velocity
*
* @param void 
* @return pointer to the generated particle || null if malloc failed
*/
Particle* new_particle(void) 
{
    Particle* ptr = malloc(sizeof(Particle));
    if (ptr == NULL) 
    {
        printf("MALLOC FAILED\n");
        return NULL;
    }

    ptr->displacement[0] = rand() % MAX_XY; /* x displacement in range of the window size */
    ptr->displacement[1] = rand() % MAX_XY; /* y displacement in range of the window size */
    ptr->velocity[0] = ((rand() % 200) - 100) / 100.0f; /* x velocity -1f < x < 0.99f */
    ptr->velocity[1] = ((rand() % 200) - 100) / 100.0f; /* x velocity -1f < x < 0.99f */

    return ptr;
}

/**
* Function: update_position
*
* Updates the position and velocity of the particle based on the time step and applies gravity.
* The particle's position is updated using its current velocity, and the velocity is updated
* with the gravitational acceleration over the given time step.
* The function also handles boundary conditions by inverting the velocity if the particle hits
* the edges of the window. 
*
* @param particle Pointer to the particle to be updated
* @param delta_time Time step (in seconds or frames) to ensure proper velocity/acceleration calculation
* @return void
*/
void update_position(Particle* particle, float delta_time) 
{
    /* Apply gravity to vertical component of velocity */
    particle->velocity[1] += GRAVITATIONAL_ACCELERATION * delta_time;

    /* Update position  of particle*/
    particle->displacement[0] += particle->velocity[0] * delta_time;
    particle->displacement[1] += particle->velocity[1] * delta_time;

    /* Ensure particle cannot go beyond the bounds in the x position*/
    if (particle->displacement[0] < MIN_XY + RADIUS) {
        particle->displacement[0] = MIN_XY + RADIUS;
        particle->velocity[0] = -particle->velocity[0];
    } else if (particle->displacement[0] > MAX_XY - RADIUS) {
        particle->displacement[0] = MAX_XY - RADIUS;
        particle->velocity[0] = -particle->velocity[0];
    }

    /* Ensure the particle cannot go beyond the bounds in the y position */
    if (particle->displacement[1] < MIN_XY + RADIUS) {
        particle->displacement[1] = MIN_XY + RADIUS;
        particle->velocity[1] = -particle->velocity[1];
    } else if (particle->displacement[1] > MAX_XY - RADIUS) {
        particle->displacement[1] = MAX_XY - RADIUS;
        particle->velocity[1] = -particle->velocity[1];
    }
}

/** 
* Function: DrawFilledCircle
*
* Draws a circle to the window using midpoint circle algorithm of radius passed as a paramter
*
* @param renderer rendering tool part of the SDL2 lib
* @param centreX xDisplacement of the circle
* @param centreY yDisplacement of the circle
* @param radius width of the circle / 2
* @return void
**/
void draw_particle(SDL_Renderer* renderer, int centreX, int centreY, int radius)
{
    int x = radius;
    int y = 0;
    int tx = 1;
    int ty = 1;
    int error = tx - radius * 2;

    while (x >= y)
    {
        SDL_RenderDrawLine(renderer, centreX - x, centreY + y, centreX + x, centreY + y);
        SDL_RenderDrawLine(renderer, centreX - x, centreY - y, centreX + x, centreY - y);
        SDL_RenderDrawLine(renderer, centreX - y, centreY + x, centreX + y, centreY + x);
        SDL_RenderDrawLine(renderer, centreX - y, centreY - x, centreX + y, centreY - x);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }
        if (error > 0)
        {
            --x;
            tx += 2;
            error += tx - radius * 2;
        }
    }
}

/**
 * Function: create_particles
 *
 * Allocates an array of `count` Particle pointers, initializes each via
 * new_particle(), and on any failure frees everything and returns NULL.
 *
 * @param void
 * @return       Pointer to newly allocated Particle* array, or NULL on error
 */
Particle** create_particles(void) {
    /* Allocate the pointer array */
    Particle** particles = malloc(NUMBER_OF_PARTICLES * sizeof(Particle*));
        if (particles == NULL) {
            printf(stderr, "create_particles: memory allocation for pointer array failed\n");
            return NULL;
        }
        /* Init each particle */
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++) // generates number of particles defined in global macro
        {
            particles[i] = new_particle();
            /* for if allocation fails partway through */
            if (particles[i] == NULL) {
                printf(stderr, "create_particles: new_particle() failed at index %d", i);

                for (int j = 0; j < i; ++j) {
                    free(particles[j]);
                }
                free(particles);
                return NULL;    
            }
        }
}

/**
* Function: main
*
* Initializes SDL2, creates the window and renderer, spawns particles,
* updates and draws them every frame until the window is closed
* 
* @param void
* @return exit status code
*/
int main(void) 
{
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1; // exit if SDL initialisation fails
    }

    /* Create SDL window */
    SDL_Window* win = SDL_CreateWindow("Particles", 100, 100, MAX_XY, MAX_XY, SDL_WINDOW_SHOWN);
    if (win == NULL) 
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1; // exit if SDL window creation fails
    }

    /* Create SDL renderer */
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) 
    {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1; // exit if SDL renderer creation fails
    }

    /* Allocate particle array with a predefined number of particles */
    allocate_particle_array();

    int quit = 0; // exit flag
    SDL_Event e; // event handeler
    uint previous_ticks = SDL_GetTicks(); //  

    /* Main simulation loop */
    while (!quit)
    {
        /* Handle quit event */
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT) 
            {
                quit = 1;
            }
        }

        /* calculates the time between frames */
        uint32_t current_ticks = SDL_GetTicks();
        float delta_time = (current_ticks - previous_ticks) / 1000.0f; // in seconds
        previous_ticks = current_ticks;

        /* Update all particles */
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
        {
            update_position(particles[i], delta_time);
        }

        // Clear screen
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        /* Draw all particles */
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
        {
            draw_particle(ren,
                            (int)particles[i]->displacement[0],
                            (int)particles[i]->displacement[1],
                            RADIUS);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(1000 / FPS); // delay to maintain frame rate
    }

    /* MALLOC deallocation */
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
    {
        free(particles[i]);
    }
    free(particles);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
