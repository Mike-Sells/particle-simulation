#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <SDL2/SDL.h>

#define TIME_STEP 1000 /**< milliseconds */
#define FPS 60 /**< frames per second */
#define NUMBER_OF_PARTICLES 10 /**< number of particles */
#define MIN_XY 0 /**< minimum screen coordinate */
#define MAX_XY 800 /**< maximum screen coordinate (screen width/height) */

/**
 * Structure: Particle
 * 
 * Represents a moving particle with displacement, velocity, and radius attributes.
 */
typedef struct {
    float displacement[2];
    float velocity[2];
    float radius;
} Particle;

/**
 * Function: new_particle
 * 
 * Allocates and initializes a new Particle instance with random position and velocity.
 * 
 * @return Pointer to the newly created Particle structure.
 */
Particle* new_particle(void) 
{
    Particle* ptr = malloc(sizeof(Particle));
    if (ptr == NULL) 
    {
        printf("malloc error...\n");
        return NULL;
    }
    
    ptr->displacement[0] = rand() % MAX_XY;
    ptr->displacement[1] = rand() % MAX_XY;
    ptr->velocity[0] = ((rand() % 200) - 100) / 100.0f; /**< random velocity between -1.0 and 1.0 */
    ptr->velocity[1] = ((rand() % 200) - 100) / 100.0f;
    ptr->radius = 5.0f + (rand() % 5); /**< random radius between 5 and 10 */

    return ptr;
}

/**
 * Function: update_position
 * 
 * Updates the particle's position based on its velocity.
 * Bounces the particle off window boundaries if necessary.
 * 
 * @param particle Pointer to the Particle being updated.
 */
void update_position(Particle* particle) 
{
    particle->displacement[0] += particle->velocity[0];
    particle->displacement[1] += particle->velocity[1];

    if (particle->displacement[0] <= MIN_XY || particle->displacement[0] >= MAX_XY)
        particle->velocity[0] = -particle->velocity[0];
    if (particle->displacement[1] <= MIN_XY || particle->displacement[1] >= MAX_XY)
        particle->velocity[1] = -particle->velocity[1];
}

/**
 * Function: DrawCircle
 * 
 * Draws a circle on the SDL renderer using the midpoint circle algorithm.
 * SDL2 does not provide a built-in circle drawing function.
 * 
 * @param renderer SDL_Renderer to draw on.
 * @param centreX X coordinate of circle center.
 * @param centreY Y coordinate of circle center.
 * @param radius Radius of the circle.
 */
void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y)
    {
        /* Draw the 8 octants */
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

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
            error += (tx - diameter);
        }
    }
}

/**
 * Function: main
 * 
 * Initializes SDL2, creates the window and renderer, spawns particles,
 * updates and draws them every frame until the window is closed.
 * 
 * @return Exit status code (0 if successful).
 */
int main(void) 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Particles", 100, 100, MAX_XY, MAX_XY, SDL_WINDOW_SHOWN);
    if (win == NULL) 
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) 
    {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Particle** particles = malloc(NUMBER_OF_PARTICLES * sizeof(Particle*));
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
    {
        particles[i] = new_particle();
    }

    int quit = 0;
    SDL_Event e;

    while (!quit)
    {
        /* Event handling */
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT) 
            {
                quit = 1;
            }
        }

        /* Update all particle positions */
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
        {
            update_position(particles[i]);
        }

        /* Render frame */
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); /**< white background */
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); /**< red particles */
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
        {
            DrawCircle(ren,
                       (int)particles[i]->displacement[0],
                       (int)particles[i]->displacement[1],
                       (int)particles[i]->radius);
        }

        SDL_RenderPresent(ren);

        SDL_Delay(1000 / FPS);
    }

    /* Cleanup memory */
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
