#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h> // Graphics lib

#define FPS 240
#define NUMBER_OF_PARTICLES 10 
#define PIXELS_PER_METER 100.0f // 100 pixels == 1 meter
#define GRAVITATIONAL_ACCELERATION 9.81f * PIXELS_PER_METER // acceleration due to gravity (earths gravitational constant)
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
* @param  void 
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
    ptr->velocity[0] = ((rand() % 200) - 100) / 10.0f * PIXELS_PER_METER; /* x velocity -1f < x < 0.99f */
    ptr->velocity[1] = ((rand() % 200) - 100) / 10.0f * PIXELS_PER_METER; /* x velocity -1f < x < 0.99f */

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
* @param  particle Pointer to the particle to be updated
* @param  delta_time Time step (in seconds or frames) to ensure proper velocity/acceleration calculation
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
* @param  renderer rendering tool part of the SDL2 lib
* @param  centreX xDisplacement of the circle
* @param  centreY yDisplacement of the circle
* @param  radius width of the circle / 2
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
 * @param  count  Number of particles to be created
 * @return Pointer to newly allocated Particle* array, or NULL on error
 */
Particle** create_particles(int count)
 {
    /* Allocate the pointer array */
    Particle** particles = malloc(count * sizeof(Particle*));
        if (particles == NULL) {
            printf("create_particles: memory allocation for pointer array failed\n");
            return NULL;
        }
        /* Init each particle */
        for (int i = 0; i < count; i++) // generates number of particles defined in global macro
        {
            particles[i] = new_particle();
            /* for if allocation fails partway through */
            if (particles[i] == NULL) {
                printf("create_particles: new_particle() failed at index %d", i);

                for (int j = 0; j < i; ++j) 
                {
                    free(particles[j]);
                }
                free(particles);
                return NULL;    
            }
        }
    return particles;
}

/**
* Function: init_SDL
*
* Initializes SDL video subsystem.
*
* @param  void
* @return 0 on success, 1 on failure
*/
int init_SDL(void) 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
}

/**
* Function: create_window
*
* Creates an SDL window with predefined size and title.
*
* @param  void
* @return Pointer to created SDL_Window, NULL on failure
*/
SDL_Window* create_window(void) 
{
    SDL_Window* win = SDL_CreateWindow("Particles", 100, 100, MAX_XY, MAX_XY, SDL_WINDOW_SHOWN);
    if (!win)
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    return win;
}

/**
* Function: create_renderer
*
* Creates an SDL renderer linked to the provided window.
*
* @param  win - Pointer to SDL_Window
* @return Pointer to created SDL_Renderer, NULL on failure
*/
SDL_Renderer* create_renderer(SDL_Window* win) 
{
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren)
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return ren;
}

/**
* Function: init_particles
*
* Allocates and initializes an array of particle pointers.
*
* @param  count - Number of particles to create
* @return Pointer to Particle* array, NULL on failure
*/
Particle** init_particles(int count) 
{
    Particle** particles = malloc(sizeof(Particle*) * count);
    if (!particles) 
    {
        printf("Failed to allocate particle array.\n");
        return NULL;
    }
    for (int i = 0; i < count; i++) 
    {
        particles[i] = new_particle();
        if (!particles[i]) 
        {
            printf("Failed to create particle.\n");
            for (int j = 0; j < i; j++) 
                free(particles[j]);
            free(particles);
            return NULL;
        }
    }
    return particles;
}

/**
* Function: cleanup
*
* Deallocates all particles and destroys SDL resources.
*
* @param  particles - Array of particle pointers
* @param  ren - Pointer to SDL_Renderer
* @param  win - Pointer to SDL_Window
* @return void
*/
void cleanup(Particle** particles, SDL_Renderer* ren, SDL_Window* win) 
{
    if (particles) 
    {
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++)
            free(particles[i]);
        free(particles);
    }
    if (ren) SDL_DestroyRenderer(ren);
    if (win) SDL_DestroyWindow(win);
    SDL_Quit();
}

/**
* Function: game_loop
*
* Main simulation loop: handles events, updates physics,
* renders particles, and maintains frame rate.
*
* @param  particles - Array of particle pointers
* @param  ren - Pointer to SDL_Renderer
* @return void
*/
void game_loop(Particle** particles, SDL_Renderer* ren) 
{
    int quit = 0;
    SDL_Event e;
    uint32_t previous_ticks = SDL_GetTicks();

    while (!quit) 
    {
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT)
                quit = 1;
        }

        uint32_t current_ticks = SDL_GetTicks();
        float delta_time = (current_ticks - previous_ticks) / 1000.0f;
        previous_ticks = current_ticks;

        for (int i = 0; i < NUMBER_OF_PARTICLES; i++)
            update_position(particles[i], delta_time);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
        {
            draw_particle(ren,
                          (int)particles[i]->displacement[0],
                          (int)particles[i]->displacement[1],
                          RADIUS);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(1000 / FPS);
    }
}

/**
* Function: main
*
* Entry point of the application.
* Initializes SDL, window, renderer, and particles,
* runs the main loop, and performs cleanup.
*
* @param  void
* @return 0 on normal exit, 1 on failure
*/
int main(void) 
{
    if (init_SDL() != 0)
        return 1;

    SDL_Window* win = create_window();
    if (!win) 
    {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = create_renderer(win);
    if (!ren) 
    {
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    Particle** particles = init_particles(NUMBER_OF_PARTICLES);
    if (!particles) 
    {
        cleanup(NULL, ren, win);
        return 1;
    }

    game_loop(particles, ren);

    cleanup(particles, ren, win);
    return 0;
}
