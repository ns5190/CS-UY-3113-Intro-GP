#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

Entity::Entity()
{

    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);


    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
}

Entity::~Entity()
{
    delete[] m_animation_up;
    delete[] m_animation_down;
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_walking;
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::ai_activate(float delta_time,Entity* player)
{
    switch (m_ai_type) {
        case DOG:
            ai_dog(player);
            break;
            
        case CHICKEN:
            ai_chicken(player);
            break;
            
        case PIG:
            ai_pig(delta_time, player);
            break;
            
        default:
            break;
    }
}

void Entity::ai_dog(Entity *player)
{
    int random_int = rand() % 1000;

    switch (m_ai_state) {
        case WALKING:
            m_speed = 0.75f;
            
            if (random_int >= 0 && random_int < 5) {
                move_left();
                m_animation_indices = m_walking[LEFT];
            }
            if (random_int >= 5 && random_int < 10) {
                move_right();
                m_animation_indices = m_walking[RIGHT];
            }
            if (random_int >= 10 && random_int < 15) {
                move_up();
                m_animation_indices = m_walking[UP];
            }
            if (random_int >= 15 && random_int < 20) {
                move_down();
                m_animation_indices = m_walking[DOWN];
            }
            if (random_int == 999) {
                m_ai_state = IDLE;
            }
            
            if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = RUNNING;
            break;
            
        case RUNNING:
            m_speed = 2.0f;
            if (random_int >= 0 && random_int < 5) {
                move_left();
                m_animation_indices = m_walking[LEFT];
            }
            if (random_int >= 5 && random_int < 10) {
                move_right();
                m_animation_indices = m_walking[RIGHT];
            }
            if (random_int >= 10 && random_int < 15) {
                move_up();
                m_animation_indices = m_walking[UP];
            }
            if (random_int >= 15 && random_int < 20) {
                move_down();
                m_animation_indices = m_walking[DOWN];
            }
            
            if (glm::distance(m_position, player->get_position()) > 3.0f) m_ai_state = WALKING;
            break;
            
        case IDLE:
            m_movement = glm::vec3(0.0f);
            if (random_int <= 5) { m_ai_state = WALKING; }
            
            if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = RUNNING;
            break;
            
        default:
            break;
    }
}

void Entity::ai_chicken(Entity *player)
{
    int random_int = rand() % 1000;
    
    switch (m_ai_state) {
        case WALKING:
            m_speed = 1.0f;
            if (glm::distance(m_position, player->get_position()) > 5.0f) {
                m_laid_egg = false;
                m_can_lay = false;
            }
            
            if (random_int >= 0 && random_int < 5) {
                move_left();
                m_animation_indices = m_walking[LEFT];
            }
            if (random_int >= 5 && random_int < 10) {
                move_right();
                m_animation_indices = m_walking[RIGHT];
            }
            if (random_int >= 10 && random_int < 15) {
                move_up();
                m_animation_indices = m_walking[UP];
            }
            if (random_int >= 15 && random_int < 20) {
                move_down();
                m_animation_indices = m_walking[DOWN];
            }
            if (random_int == 999) {
                m_ai_state = IDLE;
            }
            
            if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = RUNNING;
            break;
            
        case RUNNING:
            m_speed = 3.0f;
            m_can_lay = true;
            
            if (abs(m_position.x - player->get_position().x) > abs(m_position.y - player->get_position().y)) {
                if (m_position.x < player->get_position().x)  { m_animation_indices = m_walking[LEFT];  }
                else                                          { m_animation_indices = m_walking[RIGHT]; }
            } else {
                if (m_position.y > player->get_position().y)  { m_animation_indices = m_walking[UP];    }
                else                                          { m_animation_indices = m_walking[DOWN];  }
            }
            m_movement = glm::normalize(m_position - player->get_position());
            
            if (glm::distance(m_position, player->get_position()) > 3.0f) m_ai_state = WALKING;
            break;
            
        case IDLE:
            m_laid_egg = false;
            m_can_lay = false;
            m_movement = glm::vec3(0.0f);
            if (random_int <= 5) { m_ai_state = WALKING; }
            
            if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = RUNNING;
            break;
            
        default:
            break;
    }
}

void Entity::ai_pig(float delta_time, Entity* player)
{
    int random_int = rand() % 1000;

    switch (m_ai_state) {
        case WALKING:
            m_speed = 0.5f;
            m_finding_poop = 0.0f;
            
            if (random_int >= 0 && random_int < 5) {
                move_left();
                m_animation_indices = m_walking[LEFT];
            }
            if (random_int >= 5 && random_int < 10) {
                move_right();
                m_animation_indices = m_walking[RIGHT];
            }
            if (random_int >= 10 && random_int < 15) {
                move_up();
                m_animation_indices = m_walking[UP];
            }
            if (random_int >= 15 && random_int < 20) {
                move_down();
                m_animation_indices = m_walking[DOWN];
            }
            if (random_int >= 998) {
                m_ai_state = IDLE;
            }
            
            if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = RUNNING;
            break;
            
        case RUNNING:
            m_speed = 1.5f;
            m_finding_poop = 0;
            
            if (abs(m_position.x - player->get_position().x) > abs(m_position.y - player->get_position().y)) {
                if (m_position.x < player->get_position().x)  { m_animation_indices = m_walking[LEFT];  }
                else                                          { m_animation_indices = m_walking[RIGHT]; }
            } else {
                if (m_position.y > player->get_position().y)  { m_animation_indices = m_walking[UP];    }
                else                                          { m_animation_indices = m_walking[DOWN];  }
            }
            m_movement = glm::normalize(m_position - player->get_position());
            
            if (glm::distance(m_position, player->get_position()) > 3.0f) m_ai_state = WALKING;
            break;
            
        case IDLE:
            m_movement = glm::vec3(0.0f);
            m_finding_poop = m_finding_poop + delta_time;
            if (m_finding_poop > 5) {
                m_spawn_poop = true;
            }
            if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = RUNNING;
            break;
            
        default:
            break;
    }
}


void Entity::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map)
{
    if (!m_is_active) return;
    if (m_entity_type == ANIMAL) { ai_activate(delta_time, player); }
    
    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;
    m_collided_animal = false;

    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / SECONDS_PER_FRAME;
            
            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }
    
    m_velocity = m_movement * m_speed;
    m_velocity += m_acceleration * delta_time;
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(objects, object_count);
    check_collision_x(map);
    
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(objects, object_count);
    check_collision_y(map);
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale_size);
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float y_overlap = 0;

            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
                m_collided_animal = true;
                m_holding_index = i;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
                m_collided_animal = true;
                m_holding_index = i;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {

            float x_overlap = 0;
            
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
                m_collided_animal = true;
                m_holding_index = i;

            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
                m_collided_animal = true;
                m_holding_index = i;
            }
        }
    }
}


void const Entity::check_collision_y(Map* map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }
}

void const Entity::check_collision_x(Map* map)
{
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = -m_velocity.x;
        
        m_collided_right = true;
    }
}


void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) return;

    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


bool const Entity::check_collision(Entity* other) const
{
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

