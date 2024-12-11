#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 30
#define LEVEL_HEIGHT 20

GLuint m_font_A;

bool start_blink_A = true;
float blink_timer_A = 0;
const float blink_switch_A = 0.5;

unsigned int LEVEL_A_BACKGROUND_1_DATA[] =
{
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 422, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 422, 175, 175, 175, 175, 175, 175, 401, 175, 175,
    175, 175, 401, 175, 175, 175, 175, 175, 175, 175, 175, 401, 175, 175, 175, 423, 175, 175, 175, 151, 175, 175, 175, 151, 175, 175, 175, 175, 175, 175,
    175, 175, 150, 175, 422, 175, 175, 175, 175, 420, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 423, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 421, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 152, 175, 175, 175, 175, 175, 175, 175, 175, 175, 152, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 151, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 200, 201, 201, 201, 203, 175, 175, 175, 175, 325, 326, 326, 326, 327, 175, 175, 423, 175,
    175, 175, 175, 175, 401, 175, 175, 175, 175, 151, 175, 175, 225, 226, 206, 176, 177, 203, 175, 175, 175, 350, 351, 351, 351, 352, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 151, 175, 200, 201, 202, 176, 226, 207, 176, 228, 175, 175, 175, 375, 376, 376, 376, 377, 175, 175, 175, 175,
    175, 175, 151, 175, 175, 175, 175, 175, 175, 175, 225, 226, 231, 227, 206, 226, 226, 228, 175, 175, 175, 175, 175, 175, 175, 175, 175, 401, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 401, 175, 175, 250, 251, 251, 251, 251, 251, 251, 253, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 423, 175, 175, 175, 175, 422, 175, 175, 175, 175, 175, 175, 151, 175, 175,
    175, 175, 420, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 151, 175, 175, 175, 175, 175, 175, 152, 175, 175, 175,
    175, 423, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 422, 175, 175, 175, 175, 151, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 401, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 422, 175, 175, 175, 175, 401, 175, 175, 175,
    175, 175, 151, 151, 175, 175, 175, 422, 175, 175, 401, 401, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 401, 401, 175, 175,
    175, 175, 175, 152, 151, 175, 175, 175, 175, 401, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175
};

unsigned int LEVEL_A_BACKGROUND_2_DATA[] =
{
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,  63,  64,  65,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 387, 388,   0,   0,   0,   0,   0,   0,   0,  63,  64,  65,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,  63,  64,  65,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 113, 114,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 113, 114,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2, 113, 114,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 113, 114,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
};

unsigned int LEVEL_A_DATA[] =
{
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,  13,  14,  15,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,  38,  39,  40,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  13,  14,  15,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 362, 363,   0,   0,   0,   0,   0,   0,   0,  38,  39,  40,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,  13,  14,  15,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,  38,  39,  40,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  88,  89,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  88,  89,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,
    2,  88,  89,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  88,  89,   2,
    2,    0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,    0,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
};

LevelA::~LevelA()
{
    delete [] m_game_state.animals;
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
    Mix_FreeChunk(m_game_state.ping_sfx);
    Mix_FreeChunk(m_game_state.complete_sfx);
    Mix_FreeChunk(m_game_state.times_up_sfx);

}

void LevelA::initialise()
{
    m_game_state.next_scene_id = 1;
    

    m_font_A = Utility::load_texture("font.png");
    
    GLuint map_texture_id = Utility::load_texture("tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_A_DATA, map_texture_id, 1.0f, 25, 79);
    
    m_game_state.background_map_1 = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_A_BACKGROUND_1_DATA, map_texture_id, 1.0f, 25, 79);
    m_game_state.background_map_2 = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_A_BACKGROUND_2_DATA, map_texture_id, 1.0f, 25, 79);
    
    m_game_state.background = new Entity();
    m_game_state.background->set_size(glm::vec3(32.0f, 18.0f, 0.0f));
    m_game_state.background->m_texture_id = Utility::load_texture("background.png");
    m_game_state.background->update(0.0f, m_game_state.background, NULL, 0, m_game_state.map);
    
    m_game_state.player = new Entity();
    m_game_state.player->set_entity_type(PLAYER);
    m_game_state.player->set_position(glm::vec3(9.5f, -7.0f, 0.0f));
    m_game_state.player->set_movement(glm::vec3(0.0f));
    m_game_state.player->set_speed(3.0f);
    m_game_state.player->m_texture_id = Utility::load_texture("horse.png");

    m_game_state.player->m_walking[m_game_state.player->DOWN]     = new int[3] {  0,  1,  2 };
    m_game_state.player->m_walking[m_game_state.player->LEFT]    = new int[3] {  3,  4,  5 };
    m_game_state.player->m_walking[m_game_state.player->RIGHT]       = new int[3] {  6,  7, 8 };
    m_game_state.player->m_walking[m_game_state.player->UP]     = new int[3] { 9, 10, 11 };

    m_game_state.player->m_animation_indices = m_game_state.player->m_walking[m_game_state.player->RIGHT];
    m_game_state.player->m_animation_frames = 3;
    m_game_state.player->m_animation_index = 0;
    m_game_state.player->m_animation_time = 0.0f;
    m_game_state.player->m_animation_cols = 3;
    m_game_state.player->m_animation_rows = 4;
    m_game_state.player->set_height(1.0f);
    m_game_state.player->set_width(1.7f);
    m_game_state.player->set_size(glm::vec3(1.5f, 2.5f, 0.0f));
    
    glm::vec3 m_animal_positions[] = {glm::vec3( 7.0f, -15.0f, 0.0f),
                                      glm::vec3(25.0f, -10.0f, 0.0f)};

    m_game_state.animals = new Entity[ANIMAL_COUNT];

    for (int i = 0; i < ANIMAL_COUNT; i++) {
        // Existing
        m_game_state.animals[i].set_position(m_animal_positions[i]);
        m_game_state.animals[i].set_movement(glm::vec3(0.0f));
        m_game_state.animals[i].set_speed(0.75f);
        m_game_state.animals[i].set_entity_type(ANIMAL);
        m_game_state.animals[i].set_ai_type(DOG);
        m_game_state.animals[i].set_ai_state(WALKING);
        m_game_state.animals[i].m_texture_id = Utility::load_texture("dog.png");
        
        // Walking
        m_game_state.animals[i].m_walking[m_game_state.animals[0].LEFT]     = new int[4] { 20, 21, 22, 23 };
        m_game_state.animals[i].m_walking[m_game_state.animals[0].RIGHT]    = new int[4] {  4,  5,  6,  7 };
        m_game_state.animals[i].m_walking[m_game_state.animals[0].DOWN]     = new int[4] {  0,  1,  2,  3 };
        m_game_state.animals[i].m_walking[m_game_state.animals[0].UP]       = new int[4] {  8,  9, 10, 11 };

        m_game_state.animals[i].m_animation_indices = m_game_state.animals[0].m_walking[m_game_state.animals[0].DOWN];
        m_game_state.animals[i].m_animation_frames = 4;
        m_game_state.animals[i].m_animation_index = 0;
        m_game_state.animals[i].m_animation_time = 0.0f;
        m_game_state.animals[i].m_animation_cols = 4;
        m_game_state.animals[i].m_animation_rows = 6;
        m_game_state.animals[i].set_height(0.7f);
        m_game_state.animals[i].set_width(1.0f);
        m_game_state.animals[i].set_size(glm::vec3(2.0f, 2.0f, 0.0f));
    }
    
    m_game_state.item = new Entity();
    m_game_state.item->set_entity_type(ITEM);
    m_game_state.item->m_texture_id = Utility::load_texture("puppy.png");
    m_game_state.item->deactivate();
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("spring.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(20.0f);
    
    m_game_state.deposit_sfx = Mix_LoadWAV("deposit.wav");
    m_game_state.select_sfx = Mix_LoadWAV("select.wav");
    m_game_state.ping_sfx = Mix_LoadWAV("ping.wav");
    m_game_state.complete_sfx = Mix_LoadWAV("questcomplete.wav");
    m_game_state.times_up_sfx = Mix_LoadWAV("achievement.wav");
}

void LevelA::update(float delta_time)
{
    blink_timer_A += delta_time;
    if (blink_timer_A > blink_switch_A) {
        start_blink_A = not start_blink_A;
        blink_timer_A = 0;
    }
    
    if (timer < 0) {
        end_scene = true;
        
        m_game_state.player->update(delta_time, m_game_state.player, m_game_state.animals, ANIMAL_COUNT, m_game_state.map);

        m_game_state.item->activate();
        m_game_state.item->set_position(glm::vec3(-1.5f, 0.0f, 0.0f));
        m_game_state.item->set_size(glm::vec3(2.0f,2.0f,0.0f));
        m_game_state.item->update(delta_time, m_game_state.player, 0, NULL, m_game_state.map);
        
        m_game_state.background->set_position(glm::vec3(0.0f));
        m_game_state.background->update(delta_time, m_game_state.player, 0, NULL, m_game_state.map);
        
        end_scene_timer = end_scene_timer + delta_time ;
        return;
    }
    
    timer = timer - delta_time;
    m_game_state.item->set_position(glm::vec3(m_game_state.player->get_position().x, m_game_state.player->get_position().y - 0.2, 0));
    m_game_state.item->update(delta_time, m_game_state.player, 0, NULL, m_game_state.map);
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.animals, ANIMAL_COUNT, m_game_state.map);
    for (int i = 0; i < ANIMAL_COUNT; i++) {
        m_game_state.animals[i].update(delta_time, m_game_state.player, m_game_state.animals, ANIMAL_COUNT, m_game_state.map);
    }
}


void LevelA::render(ShaderProgram *program)
{
    
    
    if (end_scene) {
        
        if (end_scene_timer < 2.0) {
            if (!end_scene_ping[0]) {
                Mix_HaltMusic();
                Mix_PlayMusic(m_game_state.bgm2, -1);
                Mix_PlayChannel(-1, m_game_state.times_up_sfx, 0);
                end_scene_ping[0] = true;
            }
            m_game_state.background_map_1->render(program);
            m_game_state.background_map_2->render(program);
            m_game_state.map->render(program);
            for (int i = 0; i < ANIMAL_COUNT; i++) {
                m_game_state.animals[i].render(program);
            }
            m_game_state.player->activate();
            Utility::draw_text(program, m_font_A, std::string("Times Up!"),
                               0.8f, -0.4f, glm::vec3(m_game_state.player->get_position().x - 1.6f, m_game_state.player->get_position().y + 1.4f, 0.0f));
            m_game_state.player->render(program);
            m_game_state.player->deactivate();
        }
        if (end_scene_timer >= 2.0) {
            m_game_state.background->render(program);
            Utility::draw_text(program, m_font_A, std::string("Items Collected:"), 1.5f, -0.8f, glm::vec3(-5.0f, 2.5f, 0.0f));
            if (!end_scene_ping[1]) {
                Mix_PlayChannel(-1, m_game_state.ping_sfx, 0);
                end_scene_ping[1] = true;
            }
        }
        if (end_scene_timer >= 3.0) {
            m_game_state.item->render(program);
            if (!end_scene_ping[2]) {
                Mix_PlayChannel(-1, m_game_state.ping_sfx, 0);
                end_scene_ping[2] = true;
            }
        }
        if (end_scene_timer >= 4.0) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(0) << items_collected;
            std::string total_items = ss.str();
            Utility::draw_text(program, m_font_A, total_items, 1.5f, -0.8f, glm::vec3(1.5f, 0.0f, 0.0f));
            if (!end_scene_ping[3]) {
                Mix_PlayChannel(-1, m_game_state.ping_sfx, 0);
                end_scene_ping[3] = true;
            }
        }
        if (end_scene_timer >= 5.0) {
            if (!end_scene_ping[4]) {
                Mix_PlayChannel(-1, m_game_state.complete_sfx, 0);
                end_scene_ping[4] = true;
            }
            if (start_blink_A) {
                Utility::draw_text(program, m_font_A, std::string("Press [Enter] to ..."), 1.2f, -0.6f, glm::vec3(-7.5f, -2.5f, 0.0f));
                Utility::draw_text(program, m_font_A, std::string("Continue Manual Labor"), 1.2f, -0.6f, glm::vec3(-5.0f, -3.5f, 0.0f));
            }
        }
        return;
    }
    
    m_game_state.background_map_1->render(program);
    m_game_state.background_map_2->render(program);
    m_game_state.map->render(program);

    for (int i = 0; i < ANIMAL_COUNT; i++) {
        m_game_state.animals[i].render(program);
    }
    
    m_game_state.player->render(program);
    m_game_state.item->render(program);
    
    // timer
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << timer;
    std::string time_left = ss.str();
    float timer_x = m_game_state.player->get_position().x;
    float timer_y = m_game_state.player->get_position().y;
    if (m_game_state.player->get_position().x < 10.5) { timer_x = 10.5; }
    if (m_game_state.player->get_position().x > 18.5) { timer_x = 18.5; }
    if (m_game_state.player->get_position().y > -8)   { timer_y = -8;   }
    if (m_game_state.player->get_position().y < -11)  { timer_y = -11;  }
    // task
    Utility::draw_text(program, m_font_A, std::string("Task 1: Collect the puppies"), 0.8f, -0.4f, glm::vec3(timer_x - 9.5f, timer_y + 7.0f, 0.0f)); // align with timer
    Utility::draw_text(program, m_font_A, std::string("Time Left: ") + time_left, 0.8f, -0.4f, glm::vec3(timer_x + 4.5f, timer_y + 7.0f, 0.0f));
    
   
}
