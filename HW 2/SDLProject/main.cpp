#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 2.0f;

constexpr int WINDOW_WIDTH  = 640 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

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

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL    = 0;
constexpr GLint TEXTURE_BORDER     = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char BANANA_SPRITE_FILEPATH[] = "banana.png",
               M1_SPRITE_FILEPATH[]  = "monkey_1.png",
               M2_SPRITE_FILEPATH[]  = "monley_2.png";

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_SCALE_BANANA  = glm::vec3(1.0f, 1.0f, 0.0f),
                    INIT_POS_BANANA  = glm::vec3(0.0f, 0.5f, 0.0f),
                    INIT_POS_M1    = glm::vec3(-4.0f, 0.5f, 0.0f),
                    INIT_SCALE_MONKEYS = glm::vec3(4.0f, 4.0f, 0.0f),
                    INIT_POS_M2   = glm::vec3(4.0f, 0.5f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_banana_matrix, g_projection_matrix, g_m1_matrix, g_m2_matrix;

float g_previous_ticks = 0.0f;
float speed;

GLuint g_banana_texture_id;
GLuint g_m1_texture_id;
GLuint g_m2_texture_id;

constexpr float BANANA_SPEED = 3.0f; // speed of banana peel
constexpr float MONKEY_SPEED = 3.0f;
constexpr float MONKEY_BOUNDARY = 2.75f;
int g_m2_swtich = -1;


float g_banana_width = 1.0f * INIT_SCALE_BANANA.x;
float g_banana_height = 1.0f * INIT_SCALE_BANANA.y;
float g_m1_width = 1.0f * INIT_SCALE_MONKEYS.x;
float g_m1_height = 1.0f * INIT_SCALE_MONKEYS.y;
float g_m2_width = 1.0f * INIT_SCALE_MONKEYS.x;
float g_m2_height = 1.0f * INIT_SCALE_MONKEYS.y;


glm::vec3 g_banana_position   = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_banana_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_m1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_m1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_m2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_m2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_banana_scale = glm::vec3(0.0f, 0.0f, 0.0f);  // scale trigger vector
glm::vec3 g_banana_size  = glm::vec3(1.0f, 1.0f, 0.0f);  // scale accumulator vector

void initialise();
void process_input();
bool collision();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
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
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Monkey War",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);


    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_banana_texture_id = load_texture(BANANA_SPRITE_FILEPATH);
    g_m1_texture_id = load_texture(M1_SPRITE_FILEPATH);
    g_m2_texture_id = load_texture(M2_SPRITE_FILEPATH);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void process_input()
{


    SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                // End game
                case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                    g_app_status = TERMINATED;
                    break;
            
            
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_t:g_m2_swtich *= -1; break;
                        case SDLK_p:
                            g_banana_movement.x = -1;
                            
                        default:break;
                    }
                    
                default:break;
            }
        }
        const Uint8* key_state = SDL_GetKeyboardState(NULL);
    
    
    // ========== M1 CONTROLS ================ //
    g_m1_movement.y = 0.0f;
    g_m2_movement.y = 0.0f;
    
    if (key_state[SDL_SCANCODE_W] && g_m1_position.y < MONKEY_BOUNDARY)
    {
        g_m1_movement.y = 1.0f;
        
    }
    
    if (key_state[SDL_SCANCODE_S] && g_m1_position.y > -MONKEY_BOUNDARY)
    {
        g_m1_movement.y = -1.0f;
    }
    // ========== M1 CONTROLS ================ //
    

    // ========== M2 CONTROLS ================ //
    if (key_state[SDL_SCANCODE_UP] && g_m2_position.y < MONKEY_BOUNDARY)
    {
        g_m2_movement.y = 1.0f;
    }

    if (key_state[SDL_SCANCODE_DOWN] && g_m2_position.y > -MONKEY_BOUNDARY)
    {
        g_m2_movement.y = -1.0f;
    }
    // ========== M2 CONTROLS ================ //

    
}
bool collision(glm::vec3 banana_position, glm::vec3 m_position, float num) {
    float x1 = g_banana_position.x;
    float y1 = g_banana_position.y;
    float w1 = INIT_SCALE_BANANA.x;
    float h1 = INIT_SCALE_BANANA.y;
    
    float x2;
    float y2;
    float w2;
    float h2;
    
    
    if (num == 1) {
        x2 = g_m1_position.x + INIT_POS_M1.x;
        y2 = g_m1_position.y + INIT_POS_M1.y;
        w2 = INIT_SCALE_MONKEYS.x;
        h2 = INIT_SCALE_MONKEYS.y;
    } else if (num == 2){
        x2 = g_m2_position.x + INIT_POS_M2.x;
        y2 = g_m2_position.y +INIT_POS_M2.y;
        w2 = INIT_SCALE_MONKEYS.x;
        h2 = INIT_SCALE_MONKEYS.y;

    }

    float x_diff = fabs(x2 - x1);
    float y_diff = fabs(y2 - y1);

    float x_dist = x_diff - ((w1 + w2) / 2) + 1.7f;
    float y_dist = y_diff - ((h1 + h2) / 2);

    return ((x_dist <= 0) && (y_dist <= 0));
}


void update()
{
    // --- DELTA TIME CALCULATIONS --- //
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    

    // --- ACCUMULATOR LOGIC --- //
    g_banana_position += g_banana_movement * BANANA_SPEED * delta_time;
    g_m1_position += g_m1_movement * MONKEY_SPEED * delta_time;
    g_m2_position += g_m2_movement * MONKEY_SPEED * delta_time;
    
    
    // --- GAME LOGIC --- //
    if (g_m2_swtich == -1) {
        g_m2_position += g_m2_movement * BANANA_SPEED * delta_time;
    }
//    else{
//        if (g_banana_position.y < g_m2_position.y) {
//            g_m2_position += glm::vec3(0.0f, -1.0f, 0.0f) * MONKEY_SPEED * delta_time;
//        }
//        else if (g_banana_position.y > g_m2_position.y) {
//            g_m2_position += glm::vec3(0.0f, 1.0f, 0.0f) * MONKEY_SPEED * delta_time;
//        }
//    }
    
    
    // --- TRANSLATION --- //
    g_m1_matrix = glm::mat4(1.0f);
    g_m2_matrix = glm::mat4(1.0f);
    g_banana_matrix = glm::mat4(1.0f);

    g_banana_matrix = glm::translate(g_banana_matrix, g_banana_position);
    g_banana_matrix = glm::scale(g_banana_matrix, INIT_SCALE_BANANA);

    g_m1_matrix = glm::translate(g_m1_matrix, INIT_POS_M1);
    g_m1_matrix = glm::translate(g_m1_matrix, g_m1_position);

    g_m2_matrix = glm::translate(g_m2_matrix, INIT_POS_M2);
    g_m2_matrix = glm::translate(g_m2_matrix, g_m2_position);
    
    if (g_banana_position.x < -5.0f) {
        LOG("Player 2 wins!");
        g_app_status = TERMINATED;
    } else if (g_banana_position.x > 5.0f) {
        LOG("Player 1 wins!");
        g_app_status = TERMINATED;
    }
    
    bool top_touch = g_banana_position.y > 3.5f;
    bool bottom_touch = g_banana_position.y < -3.5f;
    
    if (top_touch){
        g_banana_movement.y = -1;
    } else if (bottom_touch) {
        g_banana_movement.y = 1;
    
    }
    if (collision(g_banana_position, g_m1_position, 1)) {
        g_banana_movement.x = 1;
        speed = 1.015 * BANANA_SPEED;
        if (g_m1_movement.y < 0)
        {
            g_banana_movement.y = -1;
        }
        else if (g_m1_movement.y > 0)
        {
            g_banana_movement.y = 1;
        }
    }

    if (collision(g_banana_position, g_m2_position, 2)) {
        g_banana_movement.x = -1;
        speed = 1.015 * BANANA_SPEED;
        if (g_m2_movement.y < 0)
        {
            g_banana_movement.y = -1;
        }
        else if (g_m2_movement.y > 0)
        {
            g_banana_movement.y = 1;
        }
    }
    
    
    
}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_banana_matrix, g_banana_texture_id);
    draw_object(g_m1_matrix, g_m1_texture_id);
    draw_object(g_m2_matrix, g_m2_texture_id);

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
