/**
* Author: Nabiha Siddique
* Assignment: Simple 2D Scene
* Date due: 2024-09-28, 11:58pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1, // to be generated, that is
                LEVEL_OF_DETAIL    = 0, // mipmap reduction image level
                TEXTURE_BORDER     = 0; // this value MUST be zero

constexpr char FISH_DORI_FILEPATH[] = "dori.png",
               FISH_NEMO_FILEPATH[] = "nemo.png",
               SEAWEED_FILEPATH[] = "seaweed.png",
               SHARK_FILEPATH[] = "shark.png",
               WATER_FILEPATH[] = "water.png",
               FISH_YELLOW_FILEPATH[] = "yellow.png";
            

constexpr glm::vec3 INIT_SCALE = glm::vec3(1.0f, 1.99f, 0.0f),
                    INIT_SCALE_WATER = glm::vec3(10.0f, 8.00f, 0.0f),
                    INIT_POS_FISH_DORI = glm::vec3(-1.0f, 0.0f, 0.0f),
                    INIT_POS_FISH_NEMO = glm::vec3(-3.0f, 0.0f, 0.0f),
                    INIT_POS_SEAWEED = glm::vec3(3.0f, -2.0f, 0.0f),
                    INIT_POS_SHARK = glm::vec3(-5.0f, 2.5f, 0.0f),
                    INIT_POS_WATER = glm::vec3(0.0f, 1.0f, 0.0f),
                    INIT_POS_FISH_YELLOW = glm::vec3(2.5f, 1.5f, 0.0f);


constexpr float ROT_INCREMENT = 1.0f;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_dori_matrix,
          g_nemo_matrix,
          g_seaweed_matrix,
          g_shark_matrix,
          g_water_matrix,
          g_yellow_matrix,
          g_projection_matrix;

float g_previous_ticks = 0.0f;

glm::vec3 g_rotation_dori = glm::vec3(0.0f, 0.0f, 0.0f),
          g_rotation_nemo = glm::vec3(0.0f, 0.0f, 0.0f),
          g_translate_yellow = glm::vec3(0.0f, 0.0f, 0.0f),
          g_translate_shark = glm::vec3(0.0f, 0.0f, 0.0f);

GLuint g_dori_texture_id,
       g_nemo_texture_id,
       g_seaweed_texture_id,
       g_shark_texture_id,
       g_water_texture_id,
       g_yellow_texture_id;

float theta = 0.0f;

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    // Initialise video and joystick subsystems
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Pixel Aquarium!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        SDL_Quit();
        exit(1);
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_dori_matrix = glm::mat4(1.0f);
    g_water_matrix = glm::mat4(1.0f);
    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_dori_texture_id = load_texture(FISH_DORI_FILEPATH);
    g_nemo_texture_id = load_texture(FISH_NEMO_FILEPATH);
    g_seaweed_texture_id = load_texture(SEAWEED_FILEPATH);
    g_shark_texture_id = load_texture(SHARK_FILEPATH);
    g_water_texture_id = load_texture(WATER_FILEPATH);
    g_yellow_texture_id = load_texture(FISH_YELLOW_FILEPATH);
    


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}


void update()
{
    /* Delta time calculations */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    /* Game logic */

    g_rotation_dori += ROT_INCREMENT * delta_time;

    
    g_translate_shark.x += 0.5f * delta_time;
    float frequency = 1.5f;
    float amplitude = 0.5f;
    theta += frequency * delta_time;
    g_translate_shark.y = amplitude * sin(theta);
    
    if (g_translate_shark.x <= -12.5f) {
            g_translate_shark.x = 0.0f;
            g_translate_yellow.x = 1.5f;
            g_translate_yellow.y = 4.5f;
        }
        
        g_translate_yellow.x -= 0.1f * delta_time;
        g_translate_yellow.y -= 0.1f * delta_time;


    /* Model matrix reset */
    g_projection_matrix = glm::mat4(1.0f);
    g_dori_matrix = glm::mat4(1.0f);
    g_nemo_matrix = glm::mat4(1.0f);
    g_seaweed_matrix = glm::mat4(1.0f);
    g_shark_matrix = glm::mat4(1.0f);
    g_water_matrix = glm::mat4(1.0f);
    g_yellow_matrix = glm::mat4(1.0f);
    

    /* Transformations */
    
    g_water_matrix = glm::translate(g_water_matrix, INIT_POS_WATER);
    g_water_matrix = glm::scale(g_dori_matrix, INIT_SCALE_WATER);
    
    g_dori_matrix = glm::translate(g_dori_matrix, INIT_POS_FISH_DORI);
    g_dori_matrix = glm::rotate(g_dori_matrix,
                                     g_rotation_dori.y,
                                     glm::vec3(0.0f, 1.0f, 0.0f));
    g_dori_matrix = glm::scale(g_dori_matrix, INIT_SCALE);
    
    g_nemo_matrix = glm::translate(g_nemo_matrix, INIT_POS_FISH_NEMO);
    g_nemo_matrix = glm::rotate(g_nemo_matrix,
                                     g_rotation_dori.y,
                                     glm::vec3(1.0f, 1.0f, 0.0f));
    g_nemo_matrix = glm::scale(g_nemo_matrix, INIT_SCALE);
    
    g_seaweed_matrix = glm::translate(g_seaweed_matrix, INIT_POS_SEAWEED);
    g_seaweed_matrix = glm::scale(g_seaweed_matrix, INIT_SCALE);
    
    g_shark_matrix = glm::translate(g_shark_matrix, INIT_POS_SHARK);
    g_shark_matrix = glm::translate(g_shark_matrix, g_translate_shark);
    g_shark_matrix = glm::scale(g_shark_matrix, INIT_SCALE);
    
    g_yellow_matrix = glm::translate(g_yellow_matrix, INIT_POS_FISH_YELLOW);
    g_yellow_matrix = glm::translate(g_yellow_matrix, g_translate_yellow);
    g_yellow_matrix = glm::scale(g_yellow_matrix, INIT_SCALE);

}


void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so use 6, not 3
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
                          0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
                          false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_water_matrix,  g_water_texture_id);
    draw_object(g_dori_matrix, g_dori_texture_id);
    draw_object(g_nemo_matrix, g_nemo_texture_id);
    draw_object(g_seaweed_matrix, g_seaweed_texture_id);
    draw_object(g_shark_matrix, g_shark_texture_id);
    draw_object(g_yellow_matrix, g_yellow_texture_id);

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}

