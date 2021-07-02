#include <iostream>
#include <algorithm>
#include <SDL_ttf.h>
#include <future>
#include "SDL.h"
#include "SDL_image.h"
#include "main.h"

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cout << "Error SDL2 Initialization : " << SDL_GetError();
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cout << "Error SDL2_image Initialization";
        return 2;
    }

    SDL_Window* window = SDL_CreateWindow("Canaletto", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        std::cout << "Error window creation";
        return 3;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cout << "Error renderer creation";
        return 4;
    }

    // set current path to path given as argument

    // load all jpg from current path
    std::vector<fs::path> pics = getPicsFromFolder(fs::current_path());

    // Using a for loop with index
    uint16_t currentPic = 0;

    SDL_Texture* texture = testLoadImage(pics[0], renderer);
    currentPic++;

    uint32_t ticksSincePicChanged = 0;
    uint32_t lastTicks = 0;

    std::future<SDL_Texture*> fut;

    bool bRunning = true;
    bool bPaused = false;
    while (bRunning)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            switch( e.type )
            {
                case SDL_QUIT:
                    bRunning = false;
                    break;
                /* Look for a keypress */
                case SDL_KEYDOWN:
                    /* Check the SDLKey values and move change the coords */
                    switch( e.key.keysym.sym )
                    {
                        case SDLK_SPACE:
                            std::cerr << "paused/unpaused" << std::endl;
                            bPaused = !bPaused;
                            break;
                        case SDLK_LEFT:
                            currentPic -= 1;
                            if(currentPic < 0)
                            {
                                currentPic = 0;
                            }
                            std::cerr << "backward " << currentPic << std::endl;
                            break;
                        case SDLK_RIGHT:
                            currentPic += 1;
                            if(currentPic > pics.size() - 1)
                            {
                                currentPic = pics.size();
                            }
                            std::cerr << "forward " << currentPic << std::endl;
                            break;
                        default:
                            break;
                    }
            }
        }

        uint32_t ticks = SDL_GetTicks();

        // check if it is time to load next image
        // if so, load new image
        // TODO does not work if reset (key left), since ticks do not match formula anymore...
        // TODO also make it work with player only starts after presing space bar
        // TODO as well as space bar being pause button
        if(currentPic < pics.size() && ticks > (ticksSincePicChanged + 1000) && !bPaused)
        {
            // load texture of next image (if already needed)
            fut = loadImage(pics[currentPic], renderer);

            std::cout << (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) << std::endl;
            /*
            SDL_Texture* nextTex = loadImage(pics[currentPic], renderer);
            if(nextTex != nullptr)
            {
                SDL_DestroyTexture(texture);
                texture = nextTex;

                currentPic++;
                ticksSincePicChanged = SDL_GetTicks();
            }
             */
        }
        else
        {
            SDL_RenderClear(renderer);

            // render image to offscreen
            SDL_RenderCopy(renderer, texture, NULL, NULL);

            SDL_RenderPresent(renderer);

            if ((ticks - lastTicks) < FRAME_TICKS ) {
                SDL_Delay(FRAME_TICKS - (ticks - lastTicks));
                //std::cout << "slept: " << FRAME_TICKS - (ticks - lastTicks) << std::endl;
            }

            lastTicks = ticks;
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

SDL_Texture* testLoadImage(fs::path file, SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load(file.c_str());
    if (surface == NULL) {
        std::cout << "Error loading image: " << IMG_GetError();
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        std::cout << "Error creating texture";
        return nullptr;
    }

    SDL_FreeSurface(surface);

    return texture;
}

std::future<SDL_Texture*> loadImage(fs::path file, SDL_Renderer* renderer)
{
    std::cerr << "Loading image: " << file << std::endl;

    std::future<SDL_Texture*> fut = std::async (testLoadImage, file, renderer);
    return fut;
}

std::vector<fs::path> getPicsFromFolder(fs::path dir)
{
    std::vector<fs::path> result ;

    using iterator = std::conditional< false, fs::recursive_directory_iterator, fs::directory_iterator >::type ;

    const iterator end ;
    for( iterator iter { dir } ; iter != end ; ++iter )
    {
        const std::string extension = iter->path().extension().string() ;
        if( fs::is_regular_file(*iter) && extension.compare(".jpg") == 0 )
        {
            result.push_back( *iter );
        }
    }

    std::sort(result.begin(), result.end());

    return result ;
}
