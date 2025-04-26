#include <stdio.h>
#include <stdlib.h>
#include <math.h>   
#include <SDL2/SDL.h> // Graphics lib

#define FPS 240
#define NUMBER_OF_PARTICLES 10 
#define PIXELS_PER_METER 100.0f // 100 pixels == 1 meter
#define GRAVITATIONAL_ACCELERATION 9.81f * PIXELS_PER_METER // acceleration due to gravity (earths gravitational constant)
#define RADIUS 10 // radius of each particle in pixels
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 1200

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
* Function: init_particle
*
* Initializes a particle with random displacement and velocity
*
* @param  particle - Pointer to the particle to initialize
* @return void
*/
void init_particle(Particle* particle) 
{
    particle->displacement[0] = rand() % WINDOW_WIDTH; /* x displacement in range of the window size */
    particle->displacement[1] = rand() % WINDOW_HEIGHT; /* y displacement in range of the window size */
    particle->velocity[0] = ((rand() % 200) - 100) / 10.0f * PIXELS_PER_METER; /* x velocity -1f < x < 0.99f */
    particle->velocity[1] = ((rand() % 200) - 100) / 10.0f * PIXELS_PER_METER; /* x velocity -1f < x < 0.99f */
}

/**
* Function: update_position
*
* Updates the position and velocity of a particle based on its velocity,
* gravitational acceleration, and possible collisions with window boundaries.
* 
* The function handles continuous collision detection by estimating when during
* the frame a collision would occur and updating accordingly, reflecting the 
* particle's velocity upon collision.
* 
* Limits maximum iterations to prevent infinite loops in complex cases.
*
* @param particle Pointer to the particle to be updated
* @param delta_time Elapsed time in seconds since the last frame
* @return void
*/
void update_position(Particle* particle, float delta_time) 
{
    // Apply gravity once per frame (simplification)
    particle->velocity[1] += GRAVITATIONAL_ACCELERATION * delta_time;

    float remaining_time = delta_time;
    const int max_iterations = 5; // Prevent infinite loops

    for (int iter = 0; iter < max_iterations && remaining_time > 0; iter++) 
    {
        float tentative_x = particle->displacement[0] + particle->velocity[0] * remaining_time;
        float tentative_y = particle->displacement[1] + particle->velocity[1] * remaining_time;

        // Calculate time to collision for X and Y axes
        float tx = INFINITY, ty = INFINITY;

        // X-axis collision check
        if (particle->velocity[0] != 0) 
        {
            if (particle->velocity[0] > 0) 
            {
                float boundary = WINDOW_WIDTH - RADIUS;
                if (tentative_x > boundary) 
                {
                    tx = (boundary - particle->displacement[0]) / particle->velocity[0];
                }
            } else {
                float boundary = RADIUS;
                if (tentative_x < boundary) 
                {
                    tx = (boundary - particle->displacement[0]) / particle->velocity[0];
                }
            }
        }

        // Y-axis collision check
        if (particle->velocity[1] != 0) 
        {
            if (particle->velocity[1] > 0) 
            {
                float boundary = WINDOW_HEIGHT - RADIUS;
                if (tentative_y > boundary) 
                {
                    ty = (boundary - particle->displacement[1]) / particle->velocity[1];
                }
            } 
            else 
            {
                float boundary = RADIUS;
                if (tentative_y < boundary) 
                {
                    ty = (boundary - particle->displacement[1]) / particle->velocity[1];
                }
            }
        }

        // Find earliest collision time
        float t = fminf(tx, ty);

        if (t > remaining_time) 
        {
            // No collision: move full remaining time
            particle->displacement[0] += particle->velocity[0] * remaining_time;
            particle->displacement[1] += particle->velocity[1] * remaining_time;
            remaining_time = 0;
        } 
        else 
        {
            // Move to collision point
            particle->displacement[0] += particle->velocity[0] * t;
            particle->displacement[1] += particle->velocity[1] * t;
            remaining_time -= t;

            // Reflect velocity at collision
            if (tx < ty) 
                particle->velocity[0] *= -1; // X collision
            else 
                particle->velocity[1] *= -1; // Y collision
        }
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
* Allocates a contiguous block of particles and initializes them
*
* @param  count - Number of particles to create
* @return Pointer to particle array, NULL on failure
*/
Particle* create_particles(int count)
{
    Particle* particles = malloc(count * sizeof(Particle));
    if (particles == NULL) {
        printf("create_particles: memory allocation failed\n");
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        init_particle(&particles[i]);
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
    SDL_Window* win = SDL_CreateWindow("Particles", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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
* Function: cleanup
*
* Deallocates particles and destroys SDL resources.
*
* @param  particles - Pointer to particle array
* @param  ren - Pointer to SDL_Renderer
* @param  win - Pointer to SDL_Window
* @return void
*/
void cleanup(Particle* particles, SDL_Renderer* ren, SDL_Window* win) 
{
    if (particles) free(particles);
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
* @param  particles - Pointer to particle array
* @param  ren - Pointer to SDL_Renderer
* @return void
*/
void game_loop(Particle* particles, SDL_Renderer* ren) 
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
            update_position(&particles[i], delta_time);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++) 
        {
            draw_particle(ren,
                          (int)particles[i].displacement[0],
                          (int)particles[i].displacement[1],
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

    Particle* particles = create_particles(NUMBER_OF_PARTICLES);
    if (!particles) 
    {
        cleanup(NULL, ren, win);
        return 1;
    }

    game_loop(particles, ren);

    cleanup(particles, ren, win);
    return 0;
}