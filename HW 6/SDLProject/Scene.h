#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Util.h"
#include "Entity.h"
#include "Map.h"


struct GameState
{
    Map *map;
    Map *background_map_1;
    Map *background_map_2;
    Entity *player;
    Entity *animals;
    Entity *item;
    Entity *background;
    
    Mix_Music *bgm;
    Mix_Music *bgm2;
    Mix_Chunk *deposit_sfx;
    Mix_Chunk *select_sfx;
    Mix_Chunk *complete_sfx;
    Mix_Chunk *times_up_sfx;

    int next_scene_id;
};

class Scene {
public:
    float timer = 45.0f; // seconds
    bool end_scene = false;
    float end_scene_timer = 0;
    bool end_scene_ping[5] = { false, false, false, false, false };
    int m_number_of_enemies = 1;
    int items_collected = 0;
    
    GameState m_game_state;
    
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    GameState const get_state()             const { return m_game_state;             }
    int       const get_number_of_enemies() const { return m_number_of_enemies; }
};