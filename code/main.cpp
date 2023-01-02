#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <ctime>

#define main SDL_main
#include <SDL.h>

#include "mt.cpp"

#define UNUSED(x) ((void)(x))
#define UNPACK_COL(color_bits) ((color_bits) >> 8 * 3) & 0xFF, ((color_bits) >> 8 * 2) & 0xFF, ((color_bits) >> 8 * 1) & 0xFF, ((color_bits) >> 8 * 0) & 0xFF
#define internal static
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define CLEAR_COLOR 18, 18, 18, 255
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CENTER_X (WINDOW_WIDTH * 0.5f)
#define CENTER_Y (WINDOW_HEIGHT * 0.5f)
#define POINTS_CAPACITY 10000
#define SCALE_FACTOR 0.03f

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
    return((static_cast<float> (mt_rand_u32()) / UINT32_MAX));
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

internal void generate_points(Colored_Point *points)
{        
    for (int i = 0; i < POINTS_CAPACITY; ++i) {
        float rand_x = rand_float32();
        float rand_y = rand_float32();

        Colored_Point point = {};
        point.rect.x = static_cast<int> (rand_x * WINDOW_WIDTH);
        point.rect.y = static_cast<int> (rand_y * WINDOW_HEIGHT);
        point.rect.w = 3;
        point.rect.h = 3;
        
        points[i] = point;

        // NOTE(Aiden): This is useful for debugging and just cool to look at.
        // printf("X :: %d, Y :: %d, point.rect.x, point.rect.y);
    }
}

internal int count_points_in_center(Colored_Point *points, float bullseye_radius)
{
    int points_bullseye = 0;
    
    for (int i = 0; i < POINTS_CAPACITY; ++i) {
        float dx = CENTER_X - points[i].rect.x;
        float dy = CENTER_Y - points[i].rect.y;

        if ((dx * dx) + (dy * dy) <= bullseye_radius * bullseye_radius) {
            points[i].color = 0xFF0000FF;
            points_bullseye += 1;
        } else  {
            points[i].color = 0x00FF00FF;
        }
    }

    return(points_bullseye);
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);
    
    mt_seed(static_cast<uint32_t> (time(0)));
    
    Render_Context render_context = create_window("Hello, Sailor!", WINDOW_WIDTH, WINDOW_HEIGHT);
    Colored_Point *points = static_cast<Colored_Point *> (malloc(sizeof(Colored_Point) * POINTS_CAPACITY));

    float bullseye_factor = 0.08f;
    float bullseye_radius = MIN(WINDOW_HEIGHT, WINDOW_WIDTH) * bullseye_factor;

    generate_points(points);
    printf("Probability :: %.2f%%\n",
           (static_cast<float> (count_points_in_center(points, bullseye_radius)) / POINTS_CAPACITY) * 100.0f);
    
    bool should_quit = false;
    while (!should_quit) {
        SDL_Event event = {};
        
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    should_quit = true;
                } break;

                case SDL_KEYDOWN: {
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE: {
                            should_quit = true;                            
                        } break;
                        
                        case SDLK_r: {
                            generate_points(points);
                            printf("Probability :: %.2f%%\n",
                                   (static_cast<float> (count_points_in_center(points, bullseye_radius)) / POINTS_CAPACITY) * 100.0f);
                        } break;
                    }
                } break;

                case SDL_MOUSEWHEEL: {
                    if (event.wheel.preciseY > 0) {
                        if (bullseye_factor + SCALE_FACTOR >= 0.5f) bullseye_factor = 0.5f;
                        else bullseye_factor += SCALE_FACTOR;
                    } else {
                        if (bullseye_factor - SCALE_FACTOR <= 0.03f) bullseye_factor = 0.03f;
                        else bullseye_factor -= SCALE_FACTOR;
                    }

                    bullseye_radius = MIN(WINDOW_HEIGHT, WINDOW_WIDTH) * bullseye_factor;
                    printf("Probability :: %.2f%%\n",
                           (static_cast<float> (count_points_in_center(points, bullseye_radius)) / POINTS_CAPACITY) * 100.0f);
                } break;
            }
        }

        SDL_SetRenderDrawColor(render_context.renderer, CLEAR_COLOR);
        SDL_RenderClear(render_context.renderer);

        for (int i = 0; i < POINTS_CAPACITY; ++i) {
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
