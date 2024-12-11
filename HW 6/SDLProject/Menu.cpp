#include "Menu.h"
#include "Utility.h"

#define MENU_WIDTH 1
#define MENU_HEIGHT 1

unsigned int MENU_DATA[] = { 0 };

bool start_blink = true;
float blink_timer = 0;
const float blink_switch = 0.5;


GLuint m_font_texture_id;

Menu::~Menu()
{
    delete [] m_game_state.animals;
    delete    m_game_state.player;
    delete    m_game_state.item;
    delete    m_game_state.background;
    delete    m_game_state.map;
    delete    m_game_state.background_map_1;
    delete    m_game_state.background_map_2;
    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeMusic(m_game_state.bgm2);
    Mix_FreeChunk(m_game_state.deposit_sfx);
    Mix_FreeChunk(m_game_state.select_sfx);
    Mix_FreeChunk(m_game_state.complete_sfx);
    Mix_FreeChunk(m_game_state.times_up_sfx);

}

void Menu::initialise()
{
    GLuint map_texture_id = Utility::load_texture("tileset.png");
    m_game_state.map = new Map(MENU_WIDTH, MENU_HEIGHT, MENU_DATA, map_texture_id, 1.0f, 4, 1);
    
    m_font_texture_id = Utility::load_texture("font.png");
    
    m_game_state.background = new Entity();
    m_game_state.background->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.background->set_size(glm::vec3(20.0f, 15.0f, 0.0f));
    m_game_state.background->m_texture_id = Utility::load_texture("background.png");
    m_game_state.background->update(0.0f, m_game_state.background, NULL, 0, m_game_state.map);
    
    m_game_state.player = new Entity();
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("goth.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(20.0f);
    
    m_game_state.select_sfx = Mix_LoadWAV("select.wav");
}

void Menu::update(float delta_time)
{
    blink_timer += delta_time;
    if (blink_timer > blink_switch) {
        start_blink = not start_blink;
        blink_timer = 0;
    }
}

void Menu::render(ShaderProgram *program)
{
    m_game_state.background->render(program);
    
    Utility::draw_text(program, m_font_texture_id, std::string("Welcome to Farm Noir"), 0.8f, -0.4f, glm::vec3(-3.6f, 2.0f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, std::string("... a not-so-dramatic "), 0.8f, -0.4f, glm::vec3(-5.6f, 0.0f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, std::string(" dramatic visualization of "), 0.8f, -0.4f, glm::vec3(-5.6f, -1.0f, 0.0f));
    Utility::draw_text(program, m_font_texture_id, std::string("your new life as a farmhand ... "), 0.8f, -0.4f, glm::vec3(-5.6f, -2.0f, 0.0f));
    
    if (start_blink) {
        Utility::draw_text(program, m_font_texture_id, std::string("Press [Enter] to Start Building a Puppy Army"),
                           0.8f, -0.4f, glm::vec3(-8.5f, -5.0f, 0.0f));
    }

}
