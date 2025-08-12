#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_IMAGES 100

int main(int argc, char *argv[]) {
    const char *image_folder = "./images";
    const char *audio_file = "./mp3/explosion-42132.mp3";
    int frame_delay_ms = 130;  // ~10 FPS
    int num_images = 90;
    int width = 640, height = 480;

    // Initialize SDL video and audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio Error: %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    // Load audio file
    Mix_Music *music = Mix_LoadMUS(audio_file);
    if (!music) {
        fprintf(stderr, "Failed to load audio %s: %s\n", audio_file, Mix_GetError());
        Mix_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window and renderer
    SDL_Window *win = SDL_CreateWindow("Image Sequence Player",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN);

    if (!win) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        Mix_FreeMusic(music);
        Mix_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        Mix_FreeMusic(music);
        Mix_Quit();
        SDL_Quit();
        return 1;
    }

    // Load images
    char path[512];
    SDL_Texture *textures[MAX_IMAGES] = {0};

    for (int i = 0; i < num_images; ++i) {
        snprintf(path, sizeof(path), "%s/explosion1_%04d.png", image_folder, i + 1);
        int img_w, img_h, channels;
        unsigned char *img = stbi_load(path, &img_w, &img_h, &channels, 4);
        if (!img) {
            fprintf(stderr, "Failed to load image %s\n", path);
            continue;
        }

        SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
            img, img_w, img_h, 32, img_w * 4,
            0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

        if (!surface) {
            fprintf(stderr, "Failed to create surface from image %s: %s\n", path, SDL_GetError());
            stbi_image_free(img);
            continue;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            fprintf(stderr, "Failed to create texture from image %s: %s\n", path, SDL_GetError());
        }
        textures[i] = texture;

        SDL_FreeSurface(surface);
        stbi_image_free(img);
    }

    // Play music
    if (Mix_PlayMusic(music, 1) == -1) {
        fprintf(stderr, "Failed to play music: %s\n", Mix_GetError());
    }

    SDL_Event e;
    int quit = 0;
    int loop = 1;

    while (!quit && loop) {
        for (int i = 0; i < num_images; ++i) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) quit = 1;
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) quit = 1;
            }

            if (textures[i]) {
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, textures[i], NULL, NULL);
                SDL_RenderPresent(renderer);
            }

            SDL_Delay(frame_delay_ms);
        }

        loop = 0;  // Set to 1 to loop the animation
    }

    // Wait until music finishes
    while (Mix_PlayingMusic()) {
        SDL_Delay(10);
    }

    // Cleanup
    for (int i = 0; i < num_images; ++i)
        if (textures[i]) SDL_DestroyTexture(textures[i]);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();

    return 0;
}

