#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_FLAGS SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_TRANSPARENT | SDL_WINDOW_OPENGL
// #define WINDOW_FLAGS SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_TRANSPARENT
// #define WINDOW_FLAGS SDL_WINDOW_HIGH_PIXEL_DENSITY
// #define WINDOW_FLAGS | SDL_WINDOW_OPENGL

typedef struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

typedef struct Vector2
{
    float x;
    float y;
} Vector2;

const int WIN_WIDTH = 1000;
const int WIN_HEIGHT = 1000;
Color text_color = {.r = 110, .g = 120, .b = 100, .a = 255};
Vector2 *poligon_vertex = NULL;
int faces;
Vector2 window_center = {.x = WIN_WIDTH / 2.0, .y = WIN_HEIGHT / 2.0};

float multiplier = .5;
bool paused = false;
bool is_running = true;

char str_sides[64];
char str_multiplier[64];
Vector2 intermediate_point = {};
Vector2 last_intermediate = {.x = 23, .y = 520};

SDL_Window *window = {};
SDL_Renderer *renderer;

void setup();
void input();
void update();
void render();
void cleanup();

void calc_intermediate(Vector2);
Vector2 *generate_poligon_vertices(int faces);
void clear_screen();

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        puts("Usage : chaos_game NUM (NUM = Number of faces)");
        return 0;
    }
    // set faces from argument 1
    faces = atoi(argv[1]);
    setup();
    // LOOP
    while (is_running)
    {
        input();
        update();
        render();
    }
    cleanup();
    return 0;
}
void setup()
{
    // Inizializzo sottosistemi eventi e video
    if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Errore di inizializzazione SDL_InitSubSystem : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // inizializzo finestra
    window = SDL_CreateWindow("Chaos game in SDL3", WIN_WIDTH, WIN_HEIGHT, 0);

    if (window == NULL)
    {
        fprintf(stderr, "Errore di inizializzazione SDL_CreateWindow : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // inizializzo renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL)
    {
        fprintf(stderr, "Errore di inizializzazione SDL_CreateRenderer : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // IMPOSTO VSYNC
    int ret_vsync = SDL_SetRenderVSync(renderer, 1);
    if (ret_vsync != 0)
    {
        SDL_Log("Errore in settaggio VSYNC del renderer a 1 : %s\n", SDL_GetError());
    }

    // generate poligon vertices
    poligon_vertex = generate_poligon_vertices(faces);
    clear_screen();
}
void cleanup()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    // SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Quit();
}
void input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            is_running = false;
            break;
        case SDL_EVENT_KEY_DOWN: {
            switch (event.key.key)
            {
            case SDLK_SPACE:
                // PAUSE on SPACE
                if (paused == true)
                {
                    paused = false;
                }
                else
                {
                    paused = true;
                }
                break;
                // SET MULTIPLIER
            case SDLK_KP_1:
                multiplier = .1;
                clear_screen();
                break;
            case SDLK_KP_2:
                multiplier = .2;
                clear_screen();
                break;
            case SDLK_KP_3:
                multiplier = .3;
                clear_screen();
                break;
            case SDLK_KP_4:
                multiplier = .4;
                clear_screen();
                break;
            case SDLK_KP_5:
                multiplier = .5;
                clear_screen();
                break;
            case SDLK_KP_6:
                multiplier = .6;
                clear_screen();
                break;
            case SDLK_KP_7:
                multiplier = .7;
                clear_screen();
                break;
            case SDLK_KP_8:
                multiplier = .8;
                clear_screen();
                break;
            case SDLK_KP_9:
                multiplier = .9;
                clear_screen();
                break;
                // ADJUST MULTIPLIER
            case SDLK_KP_PLUS:
                multiplier += .01;
                clear_screen();
                break;
            case SDLK_KP_MINUS:
                multiplier -= .01;
                clear_screen();
                break;
            case SDLK_ESCAPE:
            case SDLK_q:
                SDL_Log("ESCO !!");
                is_running = false;
                break;
            }
        }
        }
    }
}
void update()
{
    if (!paused && is_running)
    {
        // SELECT RANDOM POLIGON VERTEX
        int ref_vert = SDL_rand(faces); // GetRandomValue(0, faces - 1);
        // FIND INTERMEDIATE
        calc_intermediate(poligon_vertex[ref_vert]);
    }
}
void render()
{
    if (is_running)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 10, 255);
        // // Display calculated pixel
        SDL_RenderPoint(renderer, intermediate_point.x, intermediate_point.y);
        for (int i = 0; i < faces; ++i)
        {
            SDL_RenderPoint(renderer, poligon_vertex[i].x, poligon_vertex[i].y);
        }
        SDL_RenderPresent(renderer);
    }
}
void clear_screen()
{
    // ripulisco schermo col nero
    SDL_SetRenderDrawColor(renderer, 5, 5, 5, 255);
    SDL_RenderClear(renderer);
    // draw a little circle at every poligon vertex
    // for (int i = 0; i < faces; ++i)
    // {
    //     SDL_RenderPoint(renderer, poligon_vertex[i].x, poligon_vertex[i].y);
    // }
}
Vector2 *generate_poligon_vertices(int faces)
{
    poligon_vertex = calloc(faces, sizeof(Vector2));
    float two_pi_radians = 2 * 3.14159;
    float x = 0.0;
    float y = 0.0;
    float radius = 400.0;
    // find poligon vertices coordinates
    for (int i = 0; i < faces; ++i)
    {
        // Slice 2PI on sin and cos and multiply by the radius
        y = cos(two_pi_radians * ((i) / (float)faces)) * radius;
        x = sin(two_pi_radians * ((i) / (float)faces)) * radius;
        // add poligon center offset
        x += window_center.x;
        y += window_center.y;
        // save coordinate
        poligon_vertex[i] = (Vector2){.x = x, .y = y};
    }
    return poligon_vertex;
}
void calc_intermediate(Vector2 sample_vertex)
{
    // Calculate intermediate point at multiplier distance between random vertex and last calculated
    intermediate_point.x = sample_vertex.x + (last_intermediate.x - sample_vertex.x) * multiplier;
    intermediate_point.y = sample_vertex.y + (last_intermediate.y - sample_vertex.y) * multiplier;
    // save in last intermediate
    last_intermediate = intermediate_point;
    // SDL_Log("Intermediate x : %.f\ty : %.f\n", intermediate_point.x, intermediate_point.y);
}