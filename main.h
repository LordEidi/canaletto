//
// Created on 26/06/2021.
//

#ifndef CANALETTO_MAIN_H
#define CANALETTO_MAIN_H

#include <filesystem>

#define FRAME_TICKS	(1000/30)

namespace fs = std::filesystem;

SDL_Texture* testLoadImage(fs::path, SDL_Renderer* renderer);
std::future<SDL_Texture*> loadImage(fs::path file, SDL_Renderer* renderer);

std::vector<fs::path> getPicsFromFolder(fs::path dir);

#endif //CANALETTO_MAIN_H
