/**
* Author: Nabiha Siddique
* Assignment: Animal Farm Noir
* Date due: 12/11/24, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVELA_WIDTH 14
#define LEVELA_HEIGHT 8
#define LEVELA_LEFT_EDGE 18.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Utility.h"
#include "Scene.h"
#include "Menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "EndScene.h"
#include "Effects.h"


const int WINDOW_WIDTH  = 640 * 1.5,
          WINDOW_HEIGHT = 480 * 1.5;

const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr char BGM_FILEPATH[] = "goth.mp3";
constexpr int    LOOP_FOREVER = -1;
Mix_Music *g_music;


const float MILLISECONDS_IN_SECOND = 1000.0;
Effects *g_effects = nullptr;
Scene    *g_current_scene;
Menu     *g_menu;
LevelA   *g_level_a;
LevelB   *g_level_b;
LevelC   *g_level_c;
EndScene *g_end;
Scene    *g_levels[4];

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
    
    if (g_current_scene == g_end) {
        g_end->PUPPY_COUNT  = g_level_a->items_collected;
        g_end->EGG_COUNT = g_level_b->items_collected;
        g_end->POOP_COUNT  = g_level_c->items_collected;
    }
}

void initialise()
{

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Animal Farm Noir",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-10.0f, 10.0f, -7.50f, 7.50f, -2.0f, 2.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_music = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_music, LOOP_FOREVER);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    g_menu = new Menu();
    
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    g_end     = new EndScene();

    g_levels[0] = g_level_a;
    g_levels[1] = g_level_b;
    g_levels[2] = g_level_c;
    g_levels[3] = g_end;

    switch_to_scene(g_menu);
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->m_game_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running  = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_m:
                        // Mute volume
                        Mix_HaltMusic();
                        break;
                    case SDLK_SPACE:
                        // Collection
                        if (g_current_scene == g_level_a) {
                            if (g_current_scene->m_game_state.player->m_collided_animal && !g_current_scene->m_game_state.player->m_is_holding)
                            {
                                g_current_scene->m_game_state.player->m_is_holding = true;
                                g_current_scene->m_game_state.item->activate();
                                g_effects->start(SHAKING);
                                
                            }
                            else if (g_current_scene->m_game_state.player->m_is_holding &&
                                     g_current_scene->m_game_state.player->get_position().x > 10   && g_current_scene->m_game_state.player->get_position().x < 16 &&
                                     g_current_scene->m_game_state.player->get_position().y > -8.5 && g_current_scene->m_game_state.player->get_position().y < -3.5)
                            {
                                g_current_scene->m_game_state.player->m_is_holding = false;
                                g_current_scene->m_game_state.item->deactivate();
                                g_current_scene->items_collected++;
                                g_effects->start(NONE);
                                
                                Mix_PlayChannel(-1, g_current_scene->get_state().deposit_sfx, 0);
                            }
                        } else if (g_current_scene == g_level_b) {
                            if (g_current_scene->m_game_state.player->m_collided_animal && !g_current_scene->m_game_state.player->m_is_holding)
                            {
                                g_current_scene->m_game_state.player->m_is_holding = true;
                                g_effects->start(SHAKING);
                                
                                
                            }
                            else if (g_current_scene->m_game_state.player->m_is_holding &&
                                     g_current_scene->m_game_state.player->get_position().x > 10   && g_current_scene->m_game_state.player->get_position().x < 16 &&
                                     g_current_scene->m_game_state.player->get_position().y > -8.5 && g_current_scene->m_game_state.player->get_position().y < -3.5)
                            {
                                g_current_scene->m_game_state.player->m_is_holding = false;
                                g_current_scene->m_game_state.item[g_current_scene->m_game_state.player->m_holding_index].deactivate();
                                g_current_scene->items_collected++;
                                g_effects->start(NONE);
                            
                                Mix_PlayChannel(-1, g_current_scene->get_state().deposit_sfx, 0);
                            }
                        } else if (g_current_scene == g_level_c) {
                            if (g_current_scene->m_game_state.player->m_collided_animal && !g_current_scene->m_game_state.player->m_is_holding)
                            {
                                g_current_scene->m_game_state.player->m_is_holding = true;
                                g_effects->start(SHAKING);
                
                            }
                            else if (g_current_scene->m_game_state.player->m_is_holding &&
                                     g_current_scene->m_game_state.player->get_position().x > 10   && g_current_scene->m_game_state.player->get_position().x < 16 &&
                                     g_current_scene->m_game_state.player->get_position().y > -8.5 && g_current_scene->m_game_state.player->get_position().y < -3.5)
                            {
                                g_current_scene->m_game_state.player->m_is_holding = false;
                                g_current_scene->m_game_state.item[g_current_scene->m_game_state.player->m_holding_index].deactivate();
                                g_current_scene->items_collected++;
                                g_effects->start(NONE);
 
                                Mix_PlayChannel(-1, g_current_scene->get_state().deposit_sfx, 0);
                            }
                        }
                        break;
                            
                    case SDLK_RETURN:
                        if (g_current_scene == g_menu) {
                            Mix_PlayChannel(-1, g_current_scene->get_state().select_sfx, 0);
                            switch_to_scene(g_level_a);
                        } else {
                            if (g_current_scene->end_scene) {
                                Mix_PlayChannel(-1, g_current_scene->get_state().select_sfx, 0);
                                switch_to_scene(g_levels[g_current_scene->m_game_state.next_scene_id]);
                            }
                        }
                        break;
                    default:
                        break;
                    }

            default:
                break;
            }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->m_game_state.player->move_left();
        g_current_scene->m_game_state.player->m_animation_indices = g_current_scene->m_game_state.player->m_walking[g_current_scene->m_game_state.player->LEFT];
    }
    if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->m_game_state.player->move_right();
        g_current_scene->m_game_state.player->m_animation_indices = g_current_scene->m_game_state.player->m_walking[g_current_scene->m_game_state.player->RIGHT];
    }
    if (key_state[SDL_SCANCODE_UP])
    {
        g_current_scene->m_game_state.player->move_up();
        g_current_scene->m_game_state.player->m_animation_indices = g_current_scene->m_game_state.player->m_walking[g_current_scene->m_game_state.player->UP];
    }
    if (key_state[SDL_SCANCODE_DOWN])
    {
        g_current_scene->m_game_state.player->move_down();
        g_current_scene->m_game_state.player->m_animation_indices = g_current_scene->m_game_state.player->m_walking[g_current_scene->m_game_state.player->DOWN];
    }

    // This makes sure that the player can't move faster diagonally
    if (glm::length(g_current_scene->m_game_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->m_game_state.player->set_movement(glm::normalize(g_current_scene->m_game_state.player->get_movement()));
    }
}

void update()
{

    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    if (g_current_scene == g_menu) {
        return;
    }
    
    if (g_current_scene == g_end || (g_current_scene->end_scene && g_current_scene->end_scene_timer > 2.0)) {
        g_view_matrix = glm::mat4(1.0f);
        return;
    }
    
    g_view_matrix = glm::mat4(1.0f);
    
    if (g_current_scene != g_menu) {
        float camera_x = g_current_scene->m_game_state.player->get_position().x;
        float camera_y = g_current_scene->m_game_state.player->get_position().y;
        if (g_current_scene->m_game_state.player->get_position().x < 10.5) { camera_x = 10.5; }
        if (g_current_scene->m_game_state.player->get_position().x > 18.5) { camera_x = 18.5; }
        if (g_current_scene->m_game_state.player->get_position().y > -8)   { camera_y = -8;   }
        if (g_current_scene->m_game_state.player->get_position().y < -11)  { camera_y = -11;  }
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-camera_x, -camera_y, 0));
        g_shader_program.set_light_position_matrix(g_current_scene->m_game_state.player->get_position());
        
    }
    g_view_matrix = glm::translate(g_view_matrix, g_effects->get_view_offset());
    
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_current_scene->render(&g_shader_program);
    
    g_effects->render();
    
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    delete g_end;
    delete g_level_c;
    delete g_level_b;
    delete g_level_a;
    delete g_menu;
    delete g_effects;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}