#include <ctime>

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_stdinc.h"
#define SDL_MAIN_USE_CALLBACKS 1

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

const int RECT_SIZE = 100;
const float accelerationY = 0.05;
const float damping = 0.1;
const float velocityX = 5;
const float velocityY = 5;
const float MIN_VELOCITY = 0.01f;

struct RectState {
    float posX, posY, vX, vY;
    int r = 255;
    int g = 255;
    int b = 255;
    Uint64 lastTick;
};

inline int get_random_color() { return SDL_rand(256); }

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (!SDL_CreateWindowAndRenderer("Dis a window", 1000, 800,
                                     SDL_WINDOW_MAXIMIZED, &window,
                                     &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_srand(time(NULL));
    RectState *rect = new RectState({.posX = 50,
                                     .posY = 50,
                                     .vX = velocityX,
                                     .vY = velocityY,
                                     .lastTick = SDL_GetTicks()});
    *appstate = rect;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

    RectState *curState = (RectState *)appstate;

    int screenW, screenH;

    SDL_GetRenderOutputSize(renderer, &screenW, &screenH);
    Uint64 now = SDL_GetTicks();
    float dt = (now - curState->lastTick) / 10.0f;
    curState->lastTick = now;

    SDL_FRect rect = {.x = curState->posX,
                      .y = curState->posY,
                      .w = RECT_SIZE,
                      .h = RECT_SIZE};

    curState->posX += curState->vX * dt;
    curState->posY += curState->vY * dt;

    if (curState->posX <= 0) {
        curState->posX = 0;
        curState->vX *= -(1 - damping);
        curState->r = get_random_color();
        curState->g = get_random_color();
        curState->b = get_random_color();
    }

    if (curState->posY <= 0) {
        curState->posY = 0;
        curState->vY *= -(1 - damping);
        curState->r = get_random_color();
        curState->g = get_random_color();
        curState->b = get_random_color();
    }

    if (curState->posX + RECT_SIZE >= screenW) {
        curState->posX = screenW - RECT_SIZE;
        curState->vX *= -(1 - damping);
        curState->r = get_random_color();
        curState->g = get_random_color();
        curState->b = get_random_color();
    }

    if (curState->posY + RECT_SIZE >= screenH) {
        curState->posY = screenH - RECT_SIZE;
        curState->vY *= -(1 - damping);
        curState->r = get_random_color();
        curState->g = get_random_color();
        curState->b = get_random_color();
    }
    curState->vY += accelerationY * dt;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, curState->r, curState->g, curState->b,
                           255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    if (((RectState *)appstate) != nullptr) {
        delete (RectState *)appstate;
    }
}
