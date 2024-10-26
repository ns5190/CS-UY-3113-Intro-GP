/**
* Author: Nabiha Siddique
* Assignment: Lunar Lander
* Date due: 2024-10-15, 11:59pm
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
#define PLATFORM_COUNT 10

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include <cstdlib>
#include "Entity.h"

// ––––– STRUCTS AND ENUMS ––––– //
struct GameState
{
    Entity* player;
    Entity* platforms;
};

// ––––– CONSTANTS ––––– //
constexpr int WINDOW_WIDTH  = 640 * 2,
          WINDOW_HEIGHT = 400 * 2;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char SPRITESHEET_FILEPATH[] = "ship.png";
constexpr char PLATFORM_FILEPATH[]    = "volcano.png";
constexpr char WINNING_TILE_FILEPATH[] = "treasure.png";
constexpr char FONTSHEET_FILEPATH[]   = "font1.png";
constexpr int FONTBANK_SIZE = 16;
std::string winning_message = "Mission Accomplished!";
std::string losing_message = "Mission Failed!";

void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
              float font_size, float spacing, glm::vec3 position);

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL  = 0;
constexpr GLint TEXTURE_BORDER   = 0;

constexpr float ACC_OF_GRAVITY = -29.81f;
bool isCollide = false;
bool gameOver = false;
float fuel = 50.0f;


// ––––– GLOBAL VARIABLES ––––– //
GameState g_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;
GLuint g_font_texture_id;

// ––––– GENERAL FUNCTIONS ––––– //
GLuint load_texture(const char* filepath);
void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id);


void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
              float font_size, float spacing, glm::vec3 position)
{
   float width = 1.0f / FONTBANK_SIZE;
   float height = 1.0f / FONTBANK_SIZE;

   std::vector<float> vertices;
   std::vector<float> texture_coordinates;


   for (int i = 0; i < text.size(); i++) {

       int spritesheet_index = (int) text[i];  // ascii value of character
       float offset = (font_size + spacing) * i;


       float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
       float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;


       vertices.insert(vertices.end(), {
           offset + (-0.5f * font_size), 0.5f * font_size,
           offset + (-0.5f * font_size), -0.5f * font_size,
           offset + (0.5f * font_size), 0.5f * font_size,
           offset + (0.5f * font_size), -0.5f * font_size,
           offset + (0.5f * font_size), 0.5f * font_size,
           offset + (-0.5f * font_size), -0.5f * font_size,
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

   // 4. And render all of them using the pairs
   glm::mat4 model_matrix = glm::mat4(1.0f);
   model_matrix = glm::translate(model_matrix, position);

   shader_program->set_model_matrix(model_matrix);
   glUseProgram(shader_program->get_program_id());

   glVertexAttribPointer(shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0,
                         vertices.data());
   glEnableVertexAttribArray(shader_program->get_position_attribute());

   glVertexAttribPointer(shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT,
                         false, 0, texture_coordinates.data());
   glEnableVertexAttribArray(shader_program->get_tex_coordinate_attribute());

   glBindTexture(GL_TEXTURE_2D, font_texture_id);
   glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));

   glDisableVertexAttribArray(shader_program->get_position_attribute());
   glDisableVertexAttribArray(shader_program->get_tex_coordinate_attribute());
}

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Pirate Loot!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    
    // ––––– PLATFORMS ––––– //
    GLuint platform_texture_id = load_texture(PLATFORM_FILEPATH);
    GLuint winning_platform_texture_id = load_texture(WINNING_TILE_FILEPATH);

    g_state.platforms = new Entity[PLATFORM_COUNT];
    
    // Define wave properties
    float waveAmplitude = 1.5f;   // Controls how tall the wave peaks are
    float waveFrequency = 0.5f;   // Controls the frequency of the wave
    float baseYPosition = -0.5f;  // Adjusted base height to move the wave up on the screen

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        if (i == 8) { // Make tile 9 as the winning tile
            g_state.platforms[i].set_texture_id(winning_platform_texture_id);
            g_state.platforms[i].isWinningTile = true;
        } else {
            g_state.platforms[i].set_texture_id(platform_texture_id);
            g_state.platforms[i].isWinningTile = false;
        }

        // Set platform position based on a sine wave
        float x_position = i - PLATFORM_COUNT / 2.0f;
        float y_position = baseYPosition + waveAmplitude * sin(waveFrequency * i);

        g_state.platforms[i].set_position(glm::vec3(x_position, y_position, 0.0f));
        g_state.platforms[i].set_width(0.5f);  // Width of the platform remains the same
        g_state.platforms[i].set_height(0.5f);  // Height remains fixed for all tiles
        g_state.platforms[i].set_scale(glm::vec3(0.5f, 0.5f, 1.0f));  // Scale of the platform
        g_state.platforms[i].set_entity_type(PLATFORM);
        g_state.platforms[i].update(0.0f, NULL, NULL, 0);
    }


    // ––––– PLAYER ––––– //
    GLuint player_texture_id = load_texture(SPRITESHEET_FILEPATH);
    
    float initialX = -4.0f;
    float initialY = 3.1f;

    glm::vec3 acceleration = glm::vec3(0.0f,-4.905f, 0.0f);
    
    g_state.player = new Entity(
            player_texture_id,         // texture id
            acceleration,              // acceleration
            1.0f,                      // speed
            1.0f,                      // width
            1.0f,                      // height
            PLAYER,                    // Entity type
            false                    // isWinningTile
        );
    g_state.player->set_position(glm::vec3(initialX, initialY, 0.0f));
    g_state.player->set_scale(glm::vec3(0.5f, 0.5f, 1.0f));  // Make player smaller

    // ––––– GENERAL ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    g_state.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.1, 0.0f));
    g_font_texture_id = load_texture(FONTSHEET_FILEPATH);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        g_game_is_running = false;
                        break;
                }
                
            default:
                break;
        }
    }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (fuel > 0.0f) {
        if (key_state[SDL_SCANCODE_LEFT]) {
            g_state.player->add_acceleration(glm::vec3(-0.3f, 0.0f, 0.0f));
            g_state.player->set_acceleration_timer(1.0f);
            fuel -= 0.1f;
        }
        else if (key_state[SDL_SCANCODE_RIGHT]) {
            g_state.player->add_acceleration(glm::vec3(0.3f, 0.0f, 0.0f));
            g_state.player->set_acceleration_timer(1.0f);
            fuel -= 0.1f;
        }
    }

    if (glm::length(g_state.player->get_movement()) > 1.0f)
    {
        g_state.player->normalise_movement();
    }
    
}

void update()
{
    if(isCollide) return;
    
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
        g_state.player->update(FIXED_TIMESTEP, NULL, g_state.platforms, PLATFORM_COUNT);
        delta_time -= FIXED_TIMESTEP;
    }
    g_accumulator = delta_time;
    
    // collisions between the player and platforms
    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        if (g_state.player->check_collision(&g_state.platforms[8])) {
            g_state.player->set_velocity(glm::vec3(0.0f));
            isCollide = true;
            gameOver = true;
            break;
        }
        if(g_state.player->check_collision(&g_state.platforms[i]) && (i != 8)){
            g_state.player->set_velocity(glm::vec3(0.0f));
            isCollide = true;
            gameOver = false;
        }
        if((g_state.player->get_position().x < -5.3f) || (g_state.player->get_position().x > 5.3f) || g_state.player->get_position().y < -5.3f){
            isCollide = false;
            gameOver = true;
        }
    }
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    g_state.player->render(&g_program);
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        g_state.platforms[i].render(&g_program);
    }
    
    if (isCollide && gameOver) {
        draw_text(&g_program, g_font_texture_id, winning_message, 0.35f, 0.05f, glm::vec3(-4.0f, 0.0f, 0.0f));
    }
    if ((isCollide && !gameOver) || (!isCollide && gameOver)){
        draw_text(&g_program, g_font_texture_id, losing_message, 0.4f, 0.1f, glm::vec3(-3.5f, 0.0f, 0.0f));
    }
    
    std::string fuel_message = "Fuel: " + std::to_string((int)fuel);
    draw_text(&g_program, g_font_texture_id, fuel_message, 0.3f, 0.05f, glm::vec3(-4.5f, 3.5f, 0.0f));
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    delete [] g_state.platforms;
    delete g_state.player;
}

// ––––– GAME LOOP ––––– //
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
};
