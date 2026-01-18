#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <vector>

void drawTeto();

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static SDL_Texture *texture2 = NULL;
static SDL_Texture *tetotext_texture = NULL;


/* Stuff for audio*/
static SDL_AudioStream *stream = NULL;
static Uint8 *wav_data = NULL;
static Uint32 wav_data_len = 0;

static int texture_width = 0;
static int texture_height = 0;

static int tetotext_width = 0;
static int tetotext_height = 0;

float mouseCoordX = 0;
float mouseCoordY = 0;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480



struct Sprite {
    float x;
    float y;
};
std::vector<Sprite> sprites;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_AudioSpec spec;
    char *wav_path = NULL;
    
    /* I'm aware this isnt smart. it's just for funsies */
    SDL_Surface *surface = NULL;
    char *png_path = NULL;

    SDL_Surface *surface2 = NULL;
    char *png_path2 = NULL;

    SDL_Surface *tetotext_surface = NULL;
    char *tetotext_path = NULL;


    SDL_SetAppMetadata("Jamie Paige - Machine Love", "1.0", "I love Kasane Teto");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Jamie Paige - Machine Love", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    /* Textures are pixel data that we upload to the video hardware for fast drawing. Lots of 2D
       engines refer to these as "sprites." We'll do a static texture (upload once, draw many
       times) with data from a png file. */

    /* SDL_Surface is pixel data the CPU can access. SDL_Texture is pixel data the GPU can access.
       Load a .png into a surface, move it to a texture from there. */
    SDL_asprintf(&png_path, "%sassets/tetohead.png", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_free(png_path);  /* file is loaded. */

    SDL_asprintf(&png_path2, "%sassets/tetohead2.png", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface2 = SDL_LoadPNG(png_path2);
    if (!surface2) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_free(png_path2);  /* file is loaded. */

    SDL_asprintf(&tetotext_path, "%sassets/tetotext.png", SDL_GetBasePath());  /* allocate a string of the full file path */
    tetotext_surface = SDL_LoadPNG(tetotext_path);
    if (!tetotext_surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_free(tetotext_path);  /* file is loaded. */

    SDL_asprintf(&wav_path, "%sassets/jamiepage-machinelove.wav", SDL_GetBasePath());  /* allocate a string of the full file path */
    if (!SDL_LoadWAV(wav_path, &spec, &wav_data, &wav_data_len)) {
        SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_free(wav_path);  /* done with this string. */

    /* Now for loading the audio file. */
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* SDL_OpenAudioDeviceStream starts the device paused. */
    SDL_ResumeAudioStreamDevice(stream);

    texture_width = surface->w;
    texture_height = surface->h;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    if (!texture2) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface2);

    tetotext_texture = SDL_CreateTextureFromSurface(renderer, tetotext_surface);
    if (!tetotext_texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(tetotext_surface);/* done with this, the texture has a copy of the pixels now. */

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        drawTeto();
      }
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (SDL_GetAudioStreamQueued(stream) < (int)wav_data_len) {
        /* feed more data to the stream. It will queue at the end, and trickle out as the hardware needs more data. */
        SDL_PutAudioStreamData(stream, wav_data, wav_data_len);
    }

    SDL_FRect dst_rect;
    const Uint64 now = SDL_GetTicks();

    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    

    /* Just draw the static texture a few times. You can think of it like a
       stamp, there isn't a limit to the number of times you can draw with it. */
    const float white = (float) (255);
    SDL_SetRenderDrawColorFloat(renderer, white, white, white, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

    SDL_RenderClear(renderer);  /* start with a blank canvas. */

    texture_width = 200;
    texture_height= 200;

    dst_rect.w = (float)texture_width;
    dst_rect.h = (float)texture_height;

    dst_rect.x = (WINDOW_WIDTH  - dst_rect.w) * 0.5f;
    dst_rect.y = (WINDOW_HEIGHT - dst_rect.h) * 0.5f;


    /* The pictures have the same size. No need to recalculate sizes or positions. */
    if ( ((now / 500) % 2) == 0 ){
        SDL_RenderTexture(renderer, texture, NULL, &dst_rect);
    } else {
        SDL_RenderTexture(renderer, texture2, NULL, &dst_rect);
    }

    for (const Sprite &s : sprites) {
        SDL_FRect dst;
        dst.w = (float)tetotext_texture->w;
        dst.h = (float)tetotext_texture->h;
        dst.x = s.x - dst.w * 0.5f;
        dst.y = s.y - dst.h * 0.5f;

        SDL_RenderTexture(renderer, tetotext_texture, NULL, &dst);
    }

    SDL_RenderPresent(renderer);  /* put it all on the screen! */

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_DestroyTexture(texture);
    /* SDL will clean up the window/renderer for us. */
}

void drawTeto() {
    SDL_FRect dst_rect;
    SDL_GetMouseState(&mouseCoordX, &mouseCoordY);
    dst_rect.x = mouseCoordX;
    dst_rect.y = mouseCoordY;
    dst_rect.w = (float)texture_width;
    dst_rect.h = (float)texture_height;
    sprites.push_back({dst_rect.x, dst_rect.y});

    
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);
}