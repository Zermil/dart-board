#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <ctime>

#define main SDL_main
#include <SDL.h>

#include "mt.h"

#define UNUSED(x) ((void)(x))
#define UNPACK_COL(color_bits) ((color_bits) >> 8 * 3) & 0xFF, ((color_bits) >> 8 * 2) & 0xFF, ((color_bits) >> 8 * 1) & 0xFF, ((color_bits) >> 8 * 0) & 0xFF
#define internal static

#define CLEAR_COLOR 18, 18, 18, 255
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define POINTS_SIZE 10000
#define BULLSEYE_RADIUS 0.1

typedef uint32_t Color32;

struct Render_Context
{
    SDL_Window *window;
    SDL_Renderer *renderer;
};

struct Colored_Point
{
    SDL_Rect rect;
    Color32 color;
};

internal inline float rand_float32()
{
    return((static_cast<float> (rand_u32()) / UINT32_MAX));
}

internal Render_Context create_window(const char *title, int width, int height)
{
    Render_Context render_context = {};
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[SDL2 ERROR]: %s\n", SDL_GetError());
        exit(1);
    }

    render_context.window = SDL_CreateWindow(title,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          width,
                                          height,
                                          SDL_WINDOW_SHOWN);
    
    if (render_context.window == NULL) {
        fprintf(stderr, "[SDL2 ERROR]: Window was NULL\n");
        exit(1);
    }

    render_context.renderer = SDL_CreateRenderer(render_context.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (render_context.renderer == NULL) {
        fprintf(stderr, "[SDL2 ERROR]: Renderer was NULL\n");
        exit(1);
    }

    return(render_context);
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);
    seed(static_cast<uint32_t> (time(0)));
    
    Render_Context render_context = create_window("Hello, Sailor!", WINDOW_WIDTH, WINDOW_HEIGHT);
    Colored_Point *points = static_cast<Colored_Point *> (malloc(sizeof(Colored_Point) * POINTS_SIZE));

    int points_bullseye = 0;
    
    for (int i = 0; i < POINTS_SIZE; ++i) {
        float rand_x = rand_float32();
        float rand_y = rand_float32();
        
        Colored_Point point = {};
        point.rect.x = static_cast<int> (rand_x * WINDOW_WIDTH);
        point.rect.y = static_cast<int> (rand_y * WINDOW_HEIGHT);
        point.rect.w = 5;
        point.rect.h = 5;

        float dx = 0.5f - rand_x;
        float dy = 0.5f - rand_y;
        
        if ((dx * dx) + (dy * dy) <= BULLSEYE_RADIUS * BULLSEYE_RADIUS) {
            point.color = 0xFF0000FF;
            points_bullseye += 1;
        } else  {
            point.color = 0x00FF00FF;
        }
                
        points[i] = point;

        // NOTE(Aiden): This is useful for debugging and just cool to look at.
        // printf("X :: %d, Y :: %d\n", point.rect.x, point.rect.y);
    }

    printf("Probability :: %.2f%%\n", (static_cast<float> (points_bullseye) / POINTS_SIZE) * 100.0f);
    
    bool should_quit = false;
    while (!should_quit) {
        SDL_Event event = {};
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    should_quit = true;
                } break;

                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_ESCAPE) should_quit = true;
                } break;
            }
        }

        SDL_SetRenderDrawColor(render_context.renderer, CLEAR_COLOR);
        SDL_RenderClear(render_context.renderer);

        for (int i = 0; i < POINTS_SIZE; ++i) {
            SDL_SetRenderDrawColor(render_context.renderer, UNPACK_COL(points[i].color));
            SDL_RenderFillRect(render_context.renderer, &points[i].rect);
        }
        
        SDL_RenderPresent(render_context.renderer);
    }

    SDL_DestroyRenderer(render_context.renderer);
    SDL_DestroyWindow(render_context.window);
    SDL_Quit();

    free(points);
    
    return 0;
}
