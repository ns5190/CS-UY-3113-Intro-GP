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
    // ––––– PHYSICS ––––– //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);

    // ––––– TRANSLATION ––––– //
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
}

Entity::~Entity()
{
    delete[] m_animation_up_left;
    delete[] m_animation_up_right;
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_walking;
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
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

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::ai_activate(Entity* player)
{
    switch (m_ai_type)
    {
    case G1:
        ai_g1(player);
        break;

    case G2:
        ai_g2(player);
        break;
            
    case G3:
        ai_g3(player);
        break;

    default:
        break;
    }
}

void Entity::ai_g1(Entity* player)
{
    switch (m_ai_state) {
        case WALKING:
            m_speed = 0.5f;
            if (m_g1_direction == -1) {
                m_animation_indices = m_walking[LEFT];
            } else if (m_g1_direction == 1) {
                m_animation_indices = m_walking[RIGHT];
            }
            
            if (m_g1_path >= MAX_G1_PATH) {
                m_g1_direction *= -1;
                m_g1_path = 0;
            }
            
            m_movement = glm::vec3(m_g1_direction, 0.0f, 0.0f);
            m_g1_path += 0.01f;
            
            if (glm::distance(m_position, player->get_position()) < 5.0f) m_ai_state = ATTACKING;
            break;
            
        case ATTACKING:
            m_speed = 2.0f;
            if (m_position.x > player->get_position().x) {
                m_animation_indices = m_walking[LEFT];
                m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            }
            else {
                m_animation_indices = m_walking[RIGHT];
                m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            
            if (glm::distance(m_position, player->get_position()) > 5.0f) m_ai_state = WALKING;
            break;
        
        default:
            break;
    }
}

void Entity::ai_g2(Entity* player)
{
    switch (m_ai_state) {
        case IDLE:
            if (m_position.x > player->get_position().x) {
                m_animation_indices = m_walking[LEFT];
            }
            else {
                m_animation_indices = m_walking[RIGHT];
            }
                
            m_movement = glm::vec3(cos(glm::radians(m_g2_path)), sin(4 * glm::radians(m_g2_path)) / 2, 0.0f);
            m_g2_path += 0.5f;
            break;

        default:
            break;
    }
}

void Entity::ai_g3(Entity* player)
{
    switch (m_ai_state) {
        case IDLE:
            m_is_attacking = false;

            if (m_position.x > player->get_position().x) {
                m_animation_indices = m_walking[LEFT];

            }
            else {
                m_animation_indices = m_walking[RIGHT];
            }
            if (glm::distance(m_position, player->get_position()) < 5.0f) m_ai_state = ATTACKING;
        break;

        case ATTACKING:
            if (m_position.x > player->get_position().x) {
                m_animation_indices = m_walking[UP_LEFT];
                m_fire_direction = -1;
            }
            else {
                m_animation_indices = m_walking[UP_RIGHT];
                m_fire_direction = 1;
            }
            if (!m_is_attacking) {
                m_is_attacking = true;
                if (m_fire_ball > 3) m_fire_ball = 0;
                m_fire_ball += 1;
                
            }
            if (glm::distance(m_position, player->get_position()) > 5.0f) m_ai_state = IDLE;
            break;
                
        default:
            break;
    }
}


void Entity::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map)
{
    if (m_health <= 0) m_is_active = false;
    if (!m_is_active) return;
    if (m_entity_type == WEAPON && !m_is_attacking) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;
    m_collided_with_enemy = false;

    if (m_entity_type == ENEMY) ai_activate(player);
    
    if (m_entity_type == FIRE) {
        m_movement = glm::vec3(m_fire_direction, 0.0f, 0.0f);
        m_attacked = false;
        m_fire_distance += 0.1f;
    }

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

    m_velocity.x = m_movement.x * m_speed;
    if (m_ai_type == G2) {
        m_velocity.y = m_movement.y * m_speed;
    }
    m_velocity += m_acceleration * delta_time;

    // We make two calls to our check_collision methods, one for the collidable objects and one for
    // the map.
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(objects, object_count);
    check_collision_y(map);

    m_position.x += m_velocity.x * delta_time;
    check_collision_x(objects, object_count);
    check_collision_x(map);
    
    // collision detection between fire ball and player
    if (m_entity_type == FIRE && !m_attacked && check_collision(player)) {
        player->set_health(player->get_health() - 1);
        m_attacked = true;
    }
    
    // collision detection between enemy and weapon
    if (m_entity_type == WEAPON && !m_attacked) {
        for (int i = 0; i < object_count; i++) {
            Entity* collidable_entity = &objects[i];
            if (check_collision(collidable_entity)){
                if (player->get_position().x < collidable_entity->get_position().x && collidable_entity->m_ai_type == G1) {
                    collidable_entity->set_position(glm::vec3(collidable_entity->get_position().x + 2.5f, collidable_entity->get_position().y, 0.0f));
                } else if (player->get_position().x > collidable_entity->get_position().x && collidable_entity->m_ai_type == G1) {
                    collidable_entity->set_position(glm::vec3(collidable_entity->get_position().x - 2.5f, collidable_entity->get_position().y, 0.0f));
                }
                collidable_entity->set_health(collidable_entity->get_health() - 1);
                m_attacked = true;
            }
        }
    }

    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    
    // ————— ATTACKING ————— //
    if (m_is_attacking && m_entity_type == WEAPON)
    {
        if (m_current_arc > MAX_ATTACK_ARC && m_attack_direction == 1) {
                m_current_arc -= 6.0f * delta_time;
        } else if (m_current_arc < -MAX_ATTACK_ARC && m_attack_direction == 0) {
            m_current_arc += 6.0f * delta_time;
        } else {
            m_is_attacking = false;
            m_attacked = false;
            if (m_attack_direction == 0) {
                m_current_arc = -1.5f;
            } else if (m_attack_direction == 1) {
                m_current_arc = 1.5f;
            }
        }
    }
    
    if (m_collided_with_enemy) {
        m_health -= 1;
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale_size);
    
    // Moving the weapon so it looks like its attacking from the right direction
    if (m_entity_type == WEAPON) {
        m_model_matrix = glm::rotate(m_model_matrix, m_current_arc, glm::vec3(0.0f, 0.0f, 1.0f));
        if (m_attack_direction == 0) {
            m_model_matrix = glm::rotate(m_model_matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        m_model_matrix = glm::translate(m_model_matrix, glm::vec3(0.4f, 0.4f, 0.0f));
    }
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
                m_collided_with_enemy = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
                m_collided_with_enemy = true;
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
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x = (collidable_entity->get_position().x - 4.5f - x_overlap);
                m_velocity.x = -1;
                m_collided_right = true;
                m_collided_with_enemy = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x = (collidable_entity->get_position().x + 4.5f + x_overlap);
                m_velocity.x = 0;
                m_collided_left = true;
                m_collided_with_enemy = true;
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
    // Probes for tiles; the x-checking is much simpler
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
    if (m_health <= 0 && (m_entity_type == PLAYER || m_entity_type == WEAPON || m_entity_type == ENEMY)) {
        return;
    }
    
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
    float collision_factor = 0.9f;
    
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active || other->m_health == 0) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) * collision_factor / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) * collision_factor / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}