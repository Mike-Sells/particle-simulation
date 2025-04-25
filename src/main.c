#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <SDL2/SDL.h>

#define TIME_STEP 1000 /**< milliseconds */
#define FPS 60 /**< frames per second */
#define NUMBER_OF_PARTICLES 10 /**< number of particles */
#define GRAVITATIONAL_ACCELERATION -9.81f
#define RADIUS 10/**< radius of the particles */
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
        printf("MALLOC FAILED\n");
        return NULL;
    }
    
    ptr->displacement[0] = rand() % MAX_XY;
    ptr->displacement[1] = rand() % MAX_XY;
    ptr->velocity[0] = ((rand() % 200) - 100) / 100.0f; /**< random velocity between -1.0 and 1.0 */
    ptr->velocity[1] = ((rand() % 200) - 100) / 100.0f;

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
    particle->velocity[1] += GRAVITATIONAL_ACCELERATION;
    particle->displacement[0] += particle->velocity[0];
    particle->displacement[1] += particle->velocity[1];

    if (particle->displacement[0] <= MIN_XY || particle->displacement[0] >= MAX_XY)
        particle->velocity[0] = -particle->velocity[0];
    if (particle->displacement[1] <= MIN_XY || particle->displacement[1] >= MAX_XY)
        particle->velocity[1] = -particle->velocity[1];
}

/**
 * Function: DrawFilledCircle
 * 
 * Draws a circle on the SDL renderer using the midpoint circle algorithm.
 * SDL2 does not provide a built-in circle drawing function.
 * 
 * @param renderer SDL_Renderer to draw on.
 * @param centreX X coordinate of circle center.
 * @param centreY Y coordinate of circle center.
 * @param radius radius of the circle to be drawn
 */
 void DrawFilledCircle(SDL_Renderer* renderer, int centreX, int centreY, int radius)
 {
     int x = radius;
     int y = 0;
     int tx = 1;
     int ty = 1;
     int error = (tx - radius * 2);
 
     while (x >= y)
     {
         // Draw horizontal lines for every y position between the top and bottom of the circle
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
             error += (tx - radius * 2);
         }
     }
 }
 
/**
 * Function: main
 * 
 * Initializes SDL2, creates the window and renderer, spawns particles,
 * updates and draws them every frame until the window is closed.
 * 
 * @return Exit status code
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
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); /* white background */
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); /* red particles */
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
        {
            DrawFilledCircle(ren,
                       (int)particles[i]->displacement[0],
                       (int)particles[i]->displacement[1],
                       RADIUS);
        }

        SDL_RenderPresent(ren);

        SDL_Delay(TIME_STEP / FPS);
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
