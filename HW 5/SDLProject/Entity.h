#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
enum EntityType { PLATFORM, PLAYER, ENEMY  };
enum AIType     { BANANA_BIRD};
enum AIState    { WALKING, ATTACKING };


enum AnimationDirection { IDLE, MOVE, CRASH, JUMP, ATTACK };

class Entity
{
private:
    bool m_is_active = true;
    
    int m_walking[1][8]; //
    bool m_jumping_animation = false;
    bool has_key = false;
    bool next_level = false;
    bool win = false;
    
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;
    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    
    glm::mat4 m_model_matrix;
    
    float     m_speed,
    m_jumping_power,
    crash_timer = 0.0f;
    
    int health;
    
    bool lose = false;
    bool play_crash_sound = false;
    
    bool m_is_jumping = false;
    
    // ————— TEXTURES ————— //
    GLuint    m_texture_id;
    
    // ————— ANIMATION ————— //
    int m_animation_cols;
    int m_animation_frames,
    m_animation_index,
    m_animation_rows;
    
    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;
    
    float m_width = 1.0f,
    m_height = 1.0f;
    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;
    
public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 5;
    
    // ————— METHODS ————— //
    Entity();
    Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[1][8], float animation_time,
           int animation_frames, int animation_index, int animation_cols,
           int animation_rows, float width, float height, EntityType EntityType);
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType); // Simpler constructor
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState); // AI constructor
    ~Entity();
    
    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    
    // Overloading our methods to check for only the map
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map);
    void render(ShaderProgram* program);
    
    void ai_activate(Entity *player);
    void ai_banana_bird(Entity *player);
    
    void normalise_movement() { m_movement = glm::normalize(m_movement); }
    
        void crash(float enemy_x) {
            float knockback_distance = 1.0f;
            crash_timer = 0.5f;
            
            m_position.x += (enemy_x > m_position.x) ? -knockback_distance : knockback_distance;
            
            m_animation_indices = m_walking[CRASH];
            
            health -= 1;
            
            if (health <= 0) die();
            play_crash_sound = true;
        }
    
    void die() {
        lose = true;
    }
    
    void move() { if (!m_jumping_animation && crash_timer <= 0.0f) m_animation_indices = m_walking[MOVE]; }
    void idle() { if (!m_jumping_animation && crash_timer <= 0.0f) m_animation_indices = m_walking[IDLE]; }
    
    void const jump() {
        m_is_jumping = true;
        m_jumping_animation = true;
        m_animation_indices = m_walking[JUMP];
    }
    
    void move_left() {
        m_movement.x = -1.0f;
        move();
        m_scale.x = -2.0f;
    }
    
    void move_right() {
        m_movement.x = 1.0f;
        move();
        m_scale.x = 2.0f;
    }
    
    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;   };
    AIType     const get_ai_type()        const { return m_ai_type;       };
    AIState    const get_ai_state()       const { return m_ai_state;      };
    float const get_jumping_power() const { return m_jumping_power; }
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    bool      const get_jumping_animation() const { return m_jumping_animation; }
    bool      const get_key() const { return has_key; }
    bool      const get_next_level() const { return next_level; }
    bool      const get_win() const { return win; }
    int       const get_health() const { return health; }
    bool      const get_lose() const { return lose; }
    bool      const get_play_crash_sound() const { return play_crash_sound; }
    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };
    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type){ m_ai_type = new_ai_type;};
    void const set_ai_state(AIState new_state){ m_ai_state = new_state;};
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
    void const set_jumping_power(float new_jumping_power) { m_jumping_power = new_jumping_power;}
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }
    void const set_key(bool new_key) { has_key = new_key; }
    void const set_next_level(bool new_next_level) { next_level = new_next_level; }
    void const set_win(bool new_win) { win = new_win; }
    void const set_health(int new_health) { health = new_health; }
    void const set_play_crash_sound(bool new_play_crash_sound) { play_crash_sound = new_play_crash_sound; }
    
    // Setter for m_walking
    void set_walking(int walking[1][8])
    {
        for (int i = 0; i < 1; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                m_walking[i][j] = walking[i][j];
            }
        }
    }
};

#endif // ENTITY_H
