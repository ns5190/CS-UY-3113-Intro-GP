#include "EndScene.h"
#include "Utility.h"

#define MENU_WIDTH 1
#define MENU_HEIGHT 1

unsigned int END_SCENE_DATA[] = { 0 };


GLuint m_font_end;

EndScene::~EndScene()
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

void EndScene::initialise()
{
    GLuint map_texture_id = Utility::load_texture("tileset.png");
    m_game_state.map = new Map(MENU_WIDTH, MENU_HEIGHT, END_SCENE_DATA, map_texture_id, 1.0f, 4, 1);
    
    m_font_end = Utility::load_texture("font.png");
    
    m_game_state.background = new Entity();
    m_game_state.background->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.background->set_size(glm::vec3(32.0f, 18.0f, 0.0f));
    m_game_state.background->m_texture_id = Utility::load_texture("background.png");
    m_game_state.background->update(0.0f, m_game_state.background, NULL, 0, m_game_state.map);

    
    m_game_state.player = new Entity();
    
    m_game_state.item = new Entity[ITEM_COUNT];
    
    m_game_state.item[0].set_entity_type(ITEM);
    m_game_state.item[0].m_texture_id = Utility::load_texture("puppy.png");
    m_game_state.item[0].set_position(glm::vec3(-1.0f, 2.0f, 0.0f));

    m_game_state.item[1].set_entity_type(ITEM);
    m_game_state.item[1].m_texture_id = Utility::load_texture("egg.png");
    m_game_state.item[1].set_position(glm::vec3(-1.0f, 0.0f, 0.0f));
    
    m_game_state.item[2].set_entity_type(ITEM);
    m_game_state.item[2].m_texture_id = Utility::load_texture("poop.png");
    m_game_state.item[2].set_position(glm::vec3(-1.0f, -2.0f, 0.0f));
    
    m_game_state.item[3].set_entity_type(ITEM);
    m_game_state.item[3].m_texture_id = Utility::load_texture("coin.png");
    m_game_state.item[3].set_position(glm::vec3(-1.0f, -4.0f, 0.0f));
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("goth.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(20.0f);
    
    m_game_state.select_sfx = Mix_LoadWAV("select.wav");
    m_game_state.complete_sfx = Mix_LoadWAV("questcomplete.wav");
}

void EndScene::update(float delta_time)
{
    end_scene_timer = end_scene_timer + delta_time ;

    for (int i = 0; i < ITEM_COUNT; i++) {
        m_game_state.item[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
    }
}

void EndScene::render(ShaderProgram *program)
{
    m_game_state.background->render(program);
    if (end_scene_timer >= 1.0f) {
        // Calculate the score based on game variables
        int score = PUPPY_COUNT * 100 + EGG_COUNT * 50 + POOP_COUNT * 150;

        // Render points breakdown
        Utility::draw_text(program, m_font_end, "Puppy: 100 points each", 1.0f, -0.5f, glm::vec3(-5.0f, 4.0f, 0.0f));
        Utility::draw_text(program, m_font_end, "Egg: 50 points each", 1.0f, -0.5f, glm::vec3(-5.0f, 3.0f, 0.0f));
        Utility::draw_text(program, m_font_end, "Poop: 150 points each", 1.0f, -0.5f, glm::vec3(-5.0f, 2.0f, 0.0f));

        // Render the score
        std::stringstream ss;
        ss << "Score: " << score;
        Utility::draw_text(program, m_font_end, ss.str(), 1.0f, -0.5f, glm::vec3(-5.0f, 1.0f, 0.0f));

        // Display win/lose message
        std::string message = (score >= 1000) ? "You are Still Employed..." : "You are Fired!";
        Utility::draw_text(program, m_font_end, message, 1.0f, -0.5f, glm::vec3(-5.0f, 0.0f, 0.0f));

        if (!end_scene_ping[0]) {
            Mix_PlayChannel(-1, m_game_state.complete_sfx, 0);
            end_scene_ping[0] = true;
        }
}
    
}
