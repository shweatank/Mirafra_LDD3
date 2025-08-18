#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#define NUM_IMAGES 8
#define DISPLAY_TIME_MS 50  // Duration per frame in ms

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("PNG Animation",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    const char *image_files[NUM_IMAGES] = {
        "run0.png", "run1.png", "run2.png", "run3.png",
        "run4.png", "run5.png", "run6.png", "run7.png"
    };

    SDL_Texture *textures[NUM_IMAGES];
    for (int i = 0; i < NUM_IMAGES; i++) {
        SDL_Surface *surface = IMG_Load(image_files[i]);
        if (!surface) {
            printf("Failed to load %s: %s\n", image_files[i], IMG_GetError());
            // Cleanup previously loaded textures
            for (int j = 0; j < i; j++) SDL_DestroyTexture(textures[j]);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            IMG_Quit();
            SDL_Quit();
            return 1;
        }
        textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!textures[i]) {
            printf("Failed to create texture for %s: %s\n", image_files[i], SDL_GetError());
            for (int j = 0; j < i; j++) SDL_DestroyTexture(textures[j]);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            IMG_Quit();
            SDL_Quit();
            return 1;
        }
    }

    int quit = 0;
    SDL_Event e;
    int current_frame = 0;
    Uint32 last_update = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;
        }

        Uint32 now = SDL_GetTicks();
        if (now - last_update > DISPLAY_TIME_MS) {
            current_frame = (current_frame + 1) % NUM_IMAGES;
            last_update = now;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, textures[current_frame], NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    for (int i = 0; i < NUM_IMAGES; i++) {
        SDL_DestroyTexture(textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

