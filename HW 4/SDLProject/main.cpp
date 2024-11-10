/**
* Author: Nabiha Siddique
* Assignment: Rise of the AI
* Date due: 2024-11-03, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define FIRE_BALL_COUNT 3
#define PLAYER_HEALTH 5
#define WORLD_WIDTH 39
#define WORLD_HEIGHT 28

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"


struct GameState
{
    Entity* player;
    Entity* weapon;
    Entity* health;
    
    Entity* enemies;
    Entity* fire;
    
    Entity* background;
    Map* map;
};

enum GameScene {MAP, WIN_SCREEN, LOSE_SCREEN};

GameScene current_scene = MAP;


const int   WINDOW_WIDTH = 640*1.5,
            WINDOW_HEIGHT = 480*1.5;

const int FONTBANK_SIZE = 16;

const float BG_RED = 0.1922f,
            BG_BLUE = 0.549f,
            BG_GREEN = 0.9059f,
            BG_OPACITY = 1.0f;

const int   VIEWPORT_X = 0,
            VIEWPORT_Y = 0,
            VIEWPORT_WIDTH = WINDOW_WIDTH,
            VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char GAME_WINDOW_NAME[] = "Terraria?";

const char  V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

const char  SPRITESHEET_FILEPATH[]  = "player.png",
            HEALTH_FILEPATH[]       = "heart.png",
            BACKGROUND_FILEPATH[]   = "forest.png",
            WEAPON_FILEPATH[]       = "weapon.png",
            G1_FILEPATH[]           = "ghost1.png",
            G2_FILEPATH[]           = "ghost2.png",
            G3_FILEPATH[]           = "ghost3.png",
            FIRE_FILEPATH[]        = "flame.png",
            MAP_TILESET_FILEPATH[]  = "PlatformTileSet.png",
            FONT_FILEPATH[]         = "font1.png";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

unsigned int WORLD_DATA[] =
{
    36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,36,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,36,
    20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,36,
    36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,20,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,36,
     4, 0, 0, 0, 0, 0, 0,52, 3, 2, 3, 2, 3, 1, 2, 3, 2, 3, 2, 3, 1, 3, 2,53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,36,
     4, 0, 0, 0, 0, 0, 0,64,34,34,34,35,33,35,34,35,33,83,17,17,18,18,17,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,20,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,64,83,19,17,18,17,20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,20,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,64,83,19,17,18,98, 1, 1, 2, 3, 2,49, 0, 0, 0, 0, 0, 0, 0, 0,20,
    20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,64,34,34,34,35,33,35,33,35,35,65, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,48, 3, 2, 3,36,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,64,33,33,34,20,
    20, 1, 3, 3, 1, 1, 3, 3, 1, 1, 2,51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    36,34,35,33,35,34,33,34,34,34,35,67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,20,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,48, 3, 3, 2, 3, 1, 3, 1, 2, 3, 2,51, 0, 0, 0, 0, 0, 0, 0, 0, 0,36,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,18,18,19,17,18,17,17,19,18,19,20, 0, 0, 0, 0, 0, 0, 0, 0, 0,20,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,64,33,33,33,34,34,34,35,33,35,35,69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
     4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,48, 1, 1,20,
    36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,17,19,20,
    36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,17,19, 4,
    36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,50, 1, 2, 3, 2, 3, 1, 3, 1, 2, 3, 2, 3, 1,99,18,17,19,
    20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,16,18,18,19,19,17,17,19,18,19,17,17,17,17,17,18,17,18,
    20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,48, 1, 2, 3, 3, 1, 1,99,18,19,19,19,17,17,17,18,18,19,19,19,17,19,17,17,18,
    17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32,19,17,18,18,19,19,19,19,18,19,17,17,18,17,17,19,18,19,17,17,18,18,19,18,
    17, 2, 1, 2, 3, 1, 3, 3, 2, 2, 2, 2, 2, 2,99,19,17,17,19,19,17,17,19,17,17,17,17,18,18,19,19,19,19,19,19,17,17,19,19,
    17,18,18,19,19,19,17,17,17,17,18,18,19,19,17,17,19,18,19,17,17,17,17,17,18,17,17,17,19,18,19,17,18,19,17,18,19,19,19
};


GameState g_game_state;

SDL_Window* g_display_window;
bool g_game_is_running  = true;

ShaderProgram g_shader_program;
GLuint g_font_texture_id;
glm::mat4 g_view_matrix, g_projection_matrix;

float   g_previous_ticks = 0.0f,
        g_accumulator = 0.0f;


GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return texture_id;
}

void draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    for (int i = 0; i < text.size(); i++) {
        int spritesheet_index = (int) text[i];
        float offset = (screen_size + spacing) * i;
        
        float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
        });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
        });
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void initialise()
{
    // ————— GENERAL ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— VIDEO SETUP ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-19.0f, 13.0f, 0.0f));
    g_projection_matrix = glm::ortho(-5.0f * 3.5, 5.0f * 3.5, -3.75f * 3.5, 3.75f * 3.5, -1.0f * 3.5, 1.0f * 3.5);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
 
    g_font_texture_id = load_texture(FONT_FILEPATH);


    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(WORLD_WIDTH, WORLD_HEIGHT, WORLD_DATA, map_texture_id, 1.0f, 16, 15);
    

    g_game_state.background = new Entity();
    g_game_state.background->set_health(10000);
    g_game_state.background->set_position(glm::vec3(18.0f, -13.0f, 0.0f));
    g_game_state.background->set_size(glm::vec3(60.0f, 28.0f, 0.0f));
    g_game_state.background->m_texture_id = load_texture(BACKGROUND_FILEPATH);
    g_game_state.background->update(0.0f, g_game_state.background, NULL, 0, g_game_state.map);

    // ————— PLAYER ————— //
    // IDLE
    g_game_state.player = new Entity();
    g_game_state.player->set_entity_type(PLAYER);
    g_game_state.player->set_position(glm::vec3(4.0f, -25.0f, 0.0f));
    g_game_state.player->set_movement(glm::vec3(0.0f));
    g_game_state.player->set_speed(6.0f);
    g_game_state.player->set_health(PLAYER_HEALTH);
    g_game_state.player->set_acceleration(glm::vec3(0.0f, -15.0f, 0.0f));
    g_game_state.player->m_texture_id = load_texture(SPRITESHEET_FILEPATH);

    // WALKING
    g_game_state.player->m_walking[g_game_state.player->LEFT]     = new int[14] { 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31};
    g_game_state.player->m_walking[g_game_state.player->RIGHT]    = new int[14] { 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
    g_game_state.player->m_walking[g_game_state.player->UP_LEFT]  = new int[14] { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    g_game_state.player->m_walking[g_game_state.player->UP_RIGHT] = new int[14] { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

    g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];
    g_game_state.player->m_animation_frames = 14;
    g_game_state.player->m_animation_index = 0;
    g_game_state.player->m_animation_time = 0.0f;
    g_game_state.player->m_animation_cols = 2;
    g_game_state.player->m_animation_rows = 26;
    g_game_state.player->set_height(1.0f);
    g_game_state.player->set_width(1.0f);
    g_game_state.player->set_size(glm::vec3(1.3f * 1.3, 2.0f * 1.4, 0.0f));

    // JUMPING
    g_game_state.player->m_jumping_power = 10.0f;
    
    // ————— WEAPON ————— //
    g_game_state.weapon = new Entity();
    g_game_state.weapon->set_entity_type(WEAPON);
    g_game_state.weapon->set_position(g_game_state.player->get_position());
    g_game_state.weapon->set_height(1.0f);
    g_game_state.weapon->set_width(1.0f);
    g_game_state.weapon->set_health(10000);
    g_game_state.weapon->set_size(glm::vec3(3.0f, 3.0f, 0.0f));
    g_game_state.weapon->m_texture_id = load_texture(WEAPON_FILEPATH);
    
    // ————— HEALTH ————— //
    g_game_state.health = new Entity[PLAYER_HEALTH];
    
    for (int i = 0; i < PLAYER_HEALTH; i++) {
        g_game_state.health[i].set_health(10000);
        g_game_state.health[i].set_position(glm::vec3(2.3f + (i*1.2), -0.8f, 0.0f));
        g_game_state.health[i].set_size(glm::vec3(1.0f, 1.0f, 0.0f));
        g_game_state.health[i].set_entity_type(HEALTH);
        g_game_state.health[i].m_texture_id = load_texture(HEALTH_FILEPATH);
    }
    
    // —————————— ENEMY —————————— //
    g_game_state.enemies = new Entity[ENEMY_COUNT];
    
    // ————— G1 ————— //
    g_game_state.enemies[0].set_position(glm::vec3(30.0f, -20.0f, 0.0f));
    g_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[0].set_speed(0.5f);
    g_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -15.0f, 0.0f));
    g_game_state.enemies[0].set_entity_type(ENEMY);
    g_game_state.enemies[0].set_ai_type(G1);
    g_game_state.enemies[0].set_ai_state(WALKING);
    g_game_state.enemies[0].set_health(3);
    g_game_state.enemies[0].m_texture_id = load_texture(G1_FILEPATH);
    
    // WALKING
    g_game_state.enemies[0].m_walking[g_game_state.enemies[0].LEFT]     = new int[16] { 1, 1, 1, 1, 3, 3, 3, 3, 5, 5, 5, 5, 3, 3, 3, 3};
    g_game_state.enemies[0].m_walking[g_game_state.enemies[0].RIGHT]    = new int[16] { 0, 0, 0, 0, 2, 2, 2, 2, 4, 4, 4, 4, 2, 2, 2, 2};
    
    g_game_state.enemies[0].m_animation_indices = g_game_state.enemies[0].m_walking[g_game_state.enemies[0].RIGHT] ;
    g_game_state.enemies[0].m_animation_frames = 16;
    g_game_state.enemies[0].m_animation_index = 0;
    g_game_state.enemies[0].m_animation_time = 0.0f;
    g_game_state.enemies[0].m_animation_cols = 2;
    g_game_state.enemies[0].m_animation_rows = 3;
    g_game_state.enemies[0].set_height(1.0f);
    g_game_state.enemies[0].set_width(1.0f);
    g_game_state.enemies[0].set_size(glm::vec3(1.3f * 3.5, 2.0f * 1.5, 0.0f));
    
    // ————— G2 ————— //
    g_game_state.enemies[1].set_position(glm::vec3(5.0f, -13.0f, 0.0f));
    g_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[1].set_speed(1.0f);
    g_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    g_game_state.enemies[1].set_entity_type(ENEMY);
    g_game_state.enemies[1].set_ai_type(G2);
    g_game_state.enemies[1].set_ai_state(IDLE);
    g_game_state.enemies[1].set_health(3);
    g_game_state.enemies[1].m_texture_id = load_texture(G2_FILEPATH);
    
    // WALKING
    g_game_state.enemies[1].m_walking[g_game_state.enemies[1].LEFT] = new int[16] { 1, 1, 1, 1, 3, 3, 3, 3, 5, 5, 5, 5, 7, 7, 7, 7};
    g_game_state.enemies[1].m_walking[g_game_state.enemies[1].RIGHT] = new int[16] { 0, 0, 0, 0, 2, 2, 2, 2, 4, 4, 4, 4, 6, 6, 6, 6};

    g_game_state.enemies[1].m_animation_indices = g_game_state.enemies[1].m_walking[g_game_state.enemies[1].RIGHT];
    g_game_state.enemies[1].m_animation_frames = 16;
    g_game_state.enemies[1].m_animation_index = 0;
    g_game_state.enemies[1].m_animation_time = 0.0f;
    g_game_state.enemies[1].m_animation_cols = 2;
    g_game_state.enemies[1].m_animation_rows = 4;
    g_game_state.enemies[1].set_height(1.0f);
    g_game_state.enemies[1].set_width(1.0f);
    g_game_state.enemies[1].set_size(glm::vec3(1.3f * 3.5, 2.0f * 1.5, 0.0f));
    
    // ————— G3 ————— //

    g_game_state.enemies[2].set_position(glm::vec3(10.0f, -3.0f, 0.0f));
    g_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    g_game_state.enemies[2].set_speed(0.0f);
    g_game_state.enemies[2].set_acceleration(glm::vec3(0.0f, -15.0f, 0.0f));
    g_game_state.enemies[2].set_entity_type(ENEMY);
    g_game_state.enemies[2].set_ai_type(G3);
    g_game_state.enemies[2].set_ai_state(IDLE);
    g_game_state.enemies[2].set_health(5);
    g_game_state.enemies[2].m_texture_id = load_texture(G3_FILEPATH);
    
    // WALKING
    g_game_state.enemies[2].m_walking[g_game_state.enemies[2].LEFT]     = new int[8] { 1, 1, 1, 1, 1, 1, 1, 1};
    g_game_state.enemies[2].m_walking[g_game_state.enemies[2].RIGHT]    = new int[8] { 0, 0, 0, 0, 0, 0, 0, 0};
    g_game_state.enemies[2].m_walking[g_game_state.enemies[2].UP_LEFT]  = new int[8] { 3, 3, 3, 3, 5, 5, 5, 5};
    g_game_state.enemies[2].m_walking[g_game_state.enemies[2].UP_RIGHT] = new int[8] { 2, 2, 2, 2, 4, 4, 4, 4};

    g_game_state.enemies[2].m_animation_indices = g_game_state.enemies[2].m_walking[g_game_state.enemies[2].RIGHT];
    g_game_state.enemies[2].m_animation_frames = 8;
    g_game_state.enemies[2].m_animation_index = 0;
    g_game_state.enemies[2].m_animation_time = 0.0f;
    g_game_state.enemies[2].m_animation_cols = 2;
    g_game_state.enemies[2].m_animation_rows = 3;
    g_game_state.enemies[2].set_height(1.0f);
    g_game_state.enemies[2].set_width(1.0f);
    g_game_state.enemies[2].set_size(glm::vec3(1.3f * 3.5, 2.0f * 1.5, 0.0f));
    
    // ATTACK
    // ————— Fireball ————— //
    g_game_state.fire = new Entity[FIRE_BALL_COUNT];
    
    for (int i = 0; i < FIRE_BALL_COUNT; i++) {
        g_game_state.fire[i].set_health(10000);
        g_game_state.fire[i].set_speed(1.0f);
        g_game_state.fire[i].set_position(glm::vec3(g_game_state.enemies[2].get_position().x + (g_game_state.enemies[2].m_fire_direction * 1.5),
                                                     g_game_state.enemies[2].get_position().y - 1.7f + (i*1.2), 0.0f));
        g_game_state.fire[i].set_size(glm::vec3(1.0f, 1.0f, 0.0f));
        g_game_state.fire[i].set_entity_type(FIRE);
        g_game_state.fire[i].set_active(false);
        g_game_state.fire[i].m_texture_id = load_texture(FIRE_FILEPATH);
    }
    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_game_state.player->set_movement(glm::vec3(0.0f));

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
            case SDLK_SPACE: // JUMP
                if (g_game_state.player->m_collided_bottom)
                {
                    g_game_state.player->m_is_jumping = true;
                    if (g_game_state.player->m_attack_direction == 0) {
                        g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->UP_LEFT];
                    } else if (g_game_state.player->m_attack_direction == 1) {
                        g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->UP_RIGHT];
                    }
                }
                break;
                    
            case SDLK_a: // ATTACK
                if (!g_game_state.weapon->m_is_attacking && g_game_state.player->get_health() > 0)
                {
                    g_game_state.weapon->m_is_attacking = true;
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
        g_game_state.player->move_left();
        g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->LEFT];
        g_game_state.player->m_attack_direction = 0;
        g_game_state.weapon->m_attack_direction = 0;
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_game_state.player->move_right();
        g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];
        g_game_state.player->m_attack_direction = 1;
        g_game_state.weapon->m_attack_direction = 1;
    }

    if (glm::length(g_game_state.player->get_movement()) > 1.0f)
    {
        g_game_state.player->set_movement(glm::normalize(g_game_state.player->get_movement()));
    }
}

void update()
{
    if (g_game_state.player->get_health() == 0) {
        current_scene = LOSE_SCREEN;
        return;
    }
    
    if (g_game_state.enemies[0].get_health() == 0 && g_game_state.enemies[1].get_health() == 0 && g_game_state.enemies[2].get_health() == 0) {
        current_scene = WIN_SCREEN;
        return;
    }
    
    if (g_game_state.player->get_health() == 0) {
        g_game_state.weapon->set_health(0);
    }
    
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP)
    {
        if (g_game_state.weapon->m_attack_direction == 1) {
            g_game_state.weapon->set_position(glm::vec3(g_game_state.player->get_position().x + 0.3f, g_game_state.player->get_position().y - 0.2f,0.0f));
        } else if (g_game_state.weapon->m_attack_direction == 0) {
            g_game_state.weapon->set_position(glm::vec3(g_game_state.player->get_position().x - 0.3f, g_game_state.player->get_position().y - 0.2f,0.0f));
        }
        
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, g_game_state.enemies, 3, g_game_state.map);
        
        g_game_state.weapon->update(FIXED_TIMESTEP, g_game_state.weapon, g_game_state.enemies, 3, g_game_state.map);
        
        for (int i = 0; i < g_game_state.player->get_health(); i++) {
            g_game_state.health[i].update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);
        }
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            g_game_state.enemies[i].update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);
        }
        
        if (g_game_state.enemies[2].m_is_attacking) {
            for (int i = 0; i < g_game_state.enemies[2].m_fire_ball; i++) {
                g_game_state.fire[i].set_active(true);
                g_game_state.fire[i].m_fire_direction = g_game_state.enemies[2].m_fire_direction;
            }
        }
        
        for (int i = 0; i < FIRE_BALL_COUNT; i++) {
            g_game_state.fire[i].update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);
        }
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    g_game_state.background->render(&g_shader_program);
    
    switch (current_scene) {
        case MAP:
            g_game_state.map->render(&g_shader_program);
            g_game_state.player->render(&g_shader_program);
                
            for (int i = 0; i < ENEMY_COUNT; i++) {
                g_game_state.enemies[i].render(&g_shader_program);
            }
            
            for (int i = 0; i < FIRE_BALL_COUNT; i++) {
                g_game_state.fire[i].render(&g_shader_program);
            }
            
            if (g_game_state.weapon->m_is_attacking) {
                g_game_state.weapon->render(&g_shader_program);
            }
            
            for (int i = 0; i < g_game_state.player->get_health(); i++) {
                g_game_state.health[i].render(&g_shader_program);
            }
            break;
            
        case LOSE_SCREEN:
            draw_text(&g_shader_program, g_font_texture_id, std::string("Game Over!"),
                      2.8f, -0.5f, glm::vec3(9.0f, -13.0f, 0.0f));
            break;
            
        case WIN_SCREEN:
            draw_text(&g_shader_program, g_font_texture_id, std::string("You Win!"),
                      2.8f, -0.5f, glm::vec3(10.5f, -13.0f, 0.0f));
            break;
            
        default:
            break;
    }
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete[] g_game_state.enemies;
    delete[] g_game_state.health;
    delete[] g_game_state.fire;
    delete    g_game_state.player;
    delete    g_game_state.weapon;
    delete    g_game_state.map;
    delete    g_game_state.background;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running )
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
