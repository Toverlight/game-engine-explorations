#include "asset_store.hpp"

void AssetStore::clean()
{
    for (auto& pair : textures_) {
        SDL_DestroyTexture(pair.second);
    }
    textures_.clear();

    for (auto& pair : sounds_) {
        Mix_FreeChunk(pair.second);
    }
    sounds_.clear();

    for (auto& pair : music_) {
        Mix_FreeMusic(pair.second);
    }
    music_.clear();

    for (auto& pair : fonts_) {
        TTF_CloseFont(pair.second);
    }
    fonts_.clear();
    
}

void AssetStore::loadTexture(const std::string &file_path)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer_, file_path.c_str());
    if (texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load texture: %s\n", file_path.c_str());
        return;
    }
    textures_.emplace(file_path, texture); // 如果已经存在，则不会插入
}

void AssetStore::loadSound(const std::string &file_path)
{
    Mix_Chunk *sound = Mix_LoadWAV(file_path.c_str());
    if (sound == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load sound: %s\n", file_path.c_str());
        return;
    }
    sounds_.emplace(file_path, sound); // 如果已经存在，则不会插入
}

void AssetStore::loadMusic(const std::string &file_path)
{
    Mix_Music *music = Mix_LoadMUS(file_path.c_str());
    if (music == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s\n", file_path.c_str());
        return;
    }
    music_.emplace(file_path, music); // 如果已经存在，则不会插入
}

void AssetStore::loadFont(const std::string &file_path, int file_size)
{
    TTF_Font *font = TTF_OpenFont(file_path.c_str(), file_size);
    if (font == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font: %s\n", file_path.c_str());
        return;
    }
    fonts_.emplace(file_path + std::to_string(file_size), font); // 如果已经存在，则不会插入
}

SDL_Texture *AssetStore::getTexture(const std::string &file_path)
{
    auto iter = textures_.find(file_path);
    if (iter == textures_.end()) {
        loadTexture(file_path);
        iter = textures_.find(file_path);
    }
    if (iter == textures_.end()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find texture: %s\n", file_path.c_str());
        return nullptr;
    }
    return iter->second;
}

Mix_Chunk *AssetStore::getSound(const std::string &file_path)
{
    auto iter = sounds_.find(file_path);
    if (iter == sounds_.end()) {
        loadSound(file_path);
        iter = sounds_.find(file_path);
    }
    if (iter == sounds_.end()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find sound: %s\n", file_path.c_str());
        return nullptr;
    }
    return iter->second;
}

Mix_Music *AssetStore::getMusic(const std::string &file_path)
{
    auto iter = music_.find(file_path);
    if (iter == music_.end()) {
        loadMusic(file_path);
        iter = music_.find(file_path);
    }
    if (iter == music_.end()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find music: %s\n", file_path.c_str());
        return nullptr;
    }
    return iter->second;
}

TTF_Font *AssetStore::getFont(const std::string &file_path, int file_size)
{
    std::string key = file_path + std::to_string(file_size);
    auto iter = fonts_.find(key);
    if (iter == fonts_.end()) {
        loadFont(file_path, file_size);
        iter = fonts_.find(key);
    }
    if (iter == fonts_.end()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find font: %s\n", file_path.c_str());
        return nullptr;
    }
    return iter->second;
}
