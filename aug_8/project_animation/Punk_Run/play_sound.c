#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#define NUM_IMAGES 8
#define DISPLAY_TIME_MS 150  // milliseconds per frame

int main(int argc, char *argv[]) {
    // Initialize SDL video and audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image for PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_mixer for audio playback
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load background music (make sure this file exists in your folder)
    Mix_Music *music = Mix_LoadMUS("song.mp3");
    if (!music) {
        printf("Failed to load music: %s\n", Mix_GetError());
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Play the music in a loop (-1)
    Mix_PlayMusic(music, -1);

    // Create SDL window and renderer
    SDL_Window *window = SDL_CreateWindow("PNG Animation with Sound",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // List of PNG image files
    const char *image_files[NUM_IMAGES] = {
        "run0.png", "run1.png", "run2.png", "run3.png",
        "run4.png", "run5.png", "run6.png", "run7.png"
    };

    // Load all textures upfront
    SDL_Texture *textures[NUM_IMAGES];
    for (int i = 0; i < NUM_IMAGES; i++) {
        SDL_Surface *surface = IMG_Load(image_files[i]);
        if (!surface) {
            printf("Failed to load %s: %s\n", image_files[i], IMG_GetError());
            // Cleanup before quitting
            for (int j = 0; j < i; j++) SDL_DestroyTexture(textures[j]);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            Mix_FreeMusic(music);
            Mix_CloseAudio();
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
            Mix_FreeMusic(music);
            Mix_CloseAudio();
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

    // Cleanup all resources
    for (int i = 0; i < NUM_IMAGES; i++) {
        SDL_DestroyTexture(textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_FreeMusic(music);
    Mix_CloseAudio();

    IMG_Quit();
    SDL_Quit();

    return 0;
}

