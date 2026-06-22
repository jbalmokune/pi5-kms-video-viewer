#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int WIDTH  = 0;
static int HEIGHT = 0;

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s --width <w> --height <h>\n"
        "Reads raw BGRA frames of size WxH from stdin and displays them fullscreen.\n",
        prog);
}

static int parse_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--width") && i + 1 < argc) {
            WIDTH = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--height") && i + 1 < argc) {
            HEIGHT = atoi(argv[++i]);
        } else {
            usage(argv[0]);
            return -1;
        }
    }

    if (WIDTH <= 0 || HEIGHT <= 0) {
        fprintf(stderr, "Error: width and height must be specified and > 0.\n");
        usage(argv[0]);
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (parse_args(argc, argv) < 0) {
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "kmsview",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_FULLSCREEN
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_BGRA32,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );
    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const size_t frame_size = (size_t)WIDTH * (size_t)HEIGHT * 4;
    uint8_t *buffer = (uint8_t *)malloc(frame_size);
    if (!buffer) {
        fprintf(stderr, "malloc failed for %zu bytes\n", frame_size);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int running = 1;
    while (running) {
        size_t n = fread(buffer, 1, frame_size, stdin);
        if (n < frame_size) {
            break; // EOF or short read
        }

        if (SDL_UpdateTexture(texture, NULL, buffer, WIDTH * 4) < 0) {
            fprintf(stderr, "SDL_UpdateTexture failed: %s\n", SDL_GetError());
            break;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                break;
            }
        }
    }

    free(buffer);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
