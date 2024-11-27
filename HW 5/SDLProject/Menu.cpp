#include "Menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 60
#define LEVEL_HEIGHT 33

constexpr char SPRITESHEET_FILEPATH[] = "background.png",
FONT_FILEPATH[]   = "font.png";

Menu::~Menu()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    Mix_FreeMusic(m_game_state.bgm);
}

GLuint menu_font_texture_id;

void Menu::initialise()
{
    m_game_state.next_scene_id = -1;
    
    menu_font_texture_id = Utility::load_texture(FONT_FILEPATH);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    m_game_state.player = new Entity();
    
    m_game_state.player->set_texture_id(player_texture_id);
    m_game_state.player->set_position(glm::vec3(0.0f, 2.5f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(35.0f, 20.0f, 0.0f));
    m_game_state.player->update(0.0f, NULL, NULL, 0, NULL);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
   
   m_game_state.bgm = Mix_LoadMUS("background.mp3");
   Mix_PlayMusic(m_game_state.bgm, -1);
    
    m_game_state.key_press_sfx = Mix_LoadWAV("key_press.mp3");
}

void Menu::update(float delta_time)
{
    if (m_game_state.player->get_next_level()) m_game_state.next_scene_id = 1;
}

void Menu::render(ShaderProgram *program)
{
    m_game_state.player->render(program);
    Utility::draw_text(program, menu_font_texture_id, "THIS IS BANANAS!", 0.7f, 0.01f, glm::vec3(-8.8f, 3.0f, 0.0f));
    Utility::draw_text(program, menu_font_texture_id, "Press enter or return to start", 0.5f, 0.01f, glm::vec3(-5.0f, 1.0f, 0.0f));
}
