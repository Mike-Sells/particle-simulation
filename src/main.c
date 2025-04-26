#include <stdio.h>
#include <stdlib.h>
#include <math.h>   
#include <SDL2/SDL.h> // Graphics lib

#define LOG_ERR(msg) fprintf(stderr, "ERROR: %s | SDL: %s\n", msg, SDL_GetError())
#define SQUARE(n) n*n
#define FPS 60
#define NUMBER_OF_PARTICLES 150 
#define PIXELS_PER_METER 100.0f // 100 pixels == 1 meter
#define GRAVITATIONAL_ACCELERATION (9.81f * PIXELS_PER_METER) // acceleration due to gravity (earths gravitational constant)
#define DAMPENING 0.9f // causes collisions to be inelastic
#define MAX_ITERATIONS 5 // max iterations for the continuous collision detection
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
    particle->displacement[0] = rand() % WINDOW_WIDTH - RADIUS; /* x displacement in range of the window size */
    particle->displacement[1] = rand() % WINDOW_HEIGHT - RADIUS; /* y displacement in range of the window size */
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
    // @TODO apply gravity continuously
    particle->velocity[1] += GRAVITATIONAL_ACCELERATION * delta_time;

    float remaining_time = delta_time;

    for (int i = 0; i < MAX_ITERATIONS && remaining_time > 0; i++) 
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
            } else {
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
        } else {
            // Move to collision point
            particle->displacement[0] += particle->velocity[0] * t;
            particle->displacement[1] += particle->velocity[1] * t;
            remaining_time -= t;

            // Reflect velocity at collision with dampening
            if (tx < ty) {
                particle->velocity[0] *= -DAMPENING; // X collision
            } else { 
                particle->velocity[1] *= -DAMPENING; // Y collision
            } 
        }
    }
}

/**
* Function: resolve_collision
*
* Resolves the collision between two particles if the distance between their centres is less than the sum of their radii
* 
* @param  a the first partcile 
* @param  b the second particle
* @return void
*/
void resolve_collision(Particle* a, Particle* b) {
    float delta_x = b->displacement[0] - a->displacement[0];
    float delta_y = b->displacement[1] - a->displacement[1];
    float distance_sq = delta_x*delta_x + delta_y*delta_y;
    float min_distance = 2 * RADIUS;
    float min_distance_sq = min_distance * min_distance;

    // Only process collision if particles are overlapping or very close
    if (distance_sq >= min_distance_sq) return;

    float distance = sqrtf(distance_sq);
    
    // Handle potential division by zero for perfectly aligned particles
    if (distance < 0.0001f) {
        delta_x = 0.1f; // Arbitrary small push
        delta_y = 0.0f;
        distance = 0.1f;
    }

    // Calculate normalized collision vector
    float nx = delta_x / distance;
    float ny = delta_y / distance;

    // Calculate relative velocity
    float dvx = b->velocity[0] - a->velocity[0];
    float dvy = b->velocity[1] - a->velocity[1];
    
    // Calculate relative velocity in terms of the normal vector
    float velocity_along_normal = dvx * nx + dvy * ny;

    // Do not resolve if particles are separating
    if (velocity_along_normal > 0) return;

    // Calculate impulse scalar
    float restitution = DAMPENING;
    float impulse = -(1.0f + restitution) * velocity_along_normal;
    impulse /= 2.0f; // Equal mass assumption

    // Apply impulse
    a->velocity[0] -= impulse * nx;
    a->velocity[1] -= impulse * ny;
    b->velocity[0] += impulse * nx;
    b->velocity[1] += impulse * ny;

    // Positional correction to prevent sticking
    float penetration = min_distance - distance;
    const float percent = 0.2f; // Percentage of penetration to correct
    const float slop = 0.01f; // Small allowed penetration
    float correction = fmaxf(penetration - slop, 0.0f) / 2.0f * percent;

    a->displacement[0] -= correction * nx;
    a->displacement[1] -= correction * ny;
    b->displacement[0] += correction * nx;
    b->displacement[1] += correction * ny;
}

/** 
* Function: draw_particle
*
* Draws a circle to the window using midpoint circle algorithm of radius passed as a paramter
*
* @param  renderer rendering tool part of the SDL2 lib
* @param  centreX X displacement of the circle
* @param  centreY Y displacement of the circle
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
Particle* create_particles(int count) {
    Particle* particles = malloc(count * sizeof(Particle));
    if (!particles) {
        LOG_ERR("Particle allocation failed");
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
SDL_Window* create_window(void) {
    SDL_Window* win = SDL_CreateWindow("Particles", 100, 100, 
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) LOG_ERR("Window creation failed");
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
SDL_Renderer* create_renderer(SDL_Window* win) {
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) LOG_ERR("Renderer creation failed");
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
    const uint32_t frame_delay = 1000 / FPS; // Delay per frame in ms

    while (!quit) 
    {
        uint32_t frame_start = SDL_GetTicks();

        // Handle events
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT)
                quit = 1;
        }

        // Calculate delta time
        uint32_t current_ticks = SDL_GetTicks();
        float delta_time = (current_ticks - previous_ticks) / 1000.0f;
        previous_ticks = current_ticks;

        // Update physics
        for (int i = 0; i < NUMBER_OF_PARTICLES; i++)
            update_position(&particles[i], delta_time);

        // Resolve collisions
        for (int i = 0; i < NUMBER_OF_PARTICLES; ++i) {
            for (int j = i + 1; j < NUMBER_OF_PARTICLES; ++j) {
                resolve_collision(&particles[i], &particles[j]);
            }
        }

        // Rendering
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

        // Frame rate control
        uint32_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_delay > frame_time) {
            SDL_Delay(frame_delay - frame_time);
        }
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