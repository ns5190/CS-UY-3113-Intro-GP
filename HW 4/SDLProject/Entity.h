#include "Map.h"

enum EntityType { PLATFORM, PLAYER, ENEMY, WEAPON, HEALTH, FIRE};
enum AIType     { G1, G2, G3 };
enum AIState    { WALKING, IDLE, ATTACKING  };

class Entity
{
private:
    bool m_is_active = true;

    // ––––– ANIMATION ––––– //
    int* m_animation_right = NULL, // move to the right
        * m_animation_left = NULL, // move to the left
        * m_animation_up_left  = NULL, // move upwards
        * m_animation_up_right = NULL; // move downwards

    // ––––– PHYSICS (GRAVITY) ––––– //
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    glm::mat4 m_model_matrix;
    glm::vec3 m_scale_size = glm::vec3(1.0f, 1.0f, 0.0f);

    // ————— ENEMY AI ————— //
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;

    float m_width = 1;
    float m_height = 1;


public:
    // ————— STATIC VARIABLES ————— //
    static const int    SECONDS_PER_FRAME = 16;
    static const int    LEFT     = 0,
                        RIGHT    = 1,
                        UP_LEFT  = 2,
                        UP_RIGHT = 3;

    // ————— ANIMATION ————— //
    int** m_walking = new int* [4]
        {
            m_animation_left,
            m_animation_right,
            m_animation_up_left,
            m_animation_up_right
        };
    
    int m_animation_frames  = 0,
        m_animation_index   = 0,
        m_animation_cols    = 0,
        m_animation_rows    = 0;

    int*    m_animation_indices = NULL;
    float   m_animation_time = 0.0f;

    // ––––– PHYSICS (JUMPING) ––––– //
    bool  m_is_jumping = false;
    float m_jumping_power = 0;

    // ––––– PHYSICS (COLLISIONS) ––––– //
    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;
    bool m_collided_with_enemy = false;
    
    // ––––– PHYSICS (ATTACKING) ––––– //
    bool m_is_attacking = false;
    bool m_attacked = false;
    int m_attack_direction = 1;
    float m_current_arc = 1.5f;
    float MAX_ATTACK_ARC = -1.2f;
    
    // ––––– HEALTH ––––– //
    int m_health = 0;
    
    // ––––– G1 FEATURES ––––– //
    int m_g1_direction = -1;
    float m_g1_path = 0.0f;
    float MAX_G1_PATH = 5.0f;
    
    // ––––– G2 FEATURES ––––– //
    int m_g2_direction = 1;
    float m_g2_path = 0.0f;
    
    // ––––– Fire FEATURES ––––– //
    int m_fire_direction = 1;
    int m_fire_ball = 1;
    int m_fire_distance = 0.0f;
    int MAX_FIRE_DISTANCE = 5.0f;

    GLuint    m_texture_id;

    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map); // Now, update should check for both objects in the game AND the map
    void render(ShaderProgram* program);

    bool const check_collision(Entity* other) const;
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);

    // Overloading our methods to check for only the map
    void const check_collision_y(Map* map);
    void const check_collision_x(Map* map);

    void move_left()    { m_movement.x = -1.0f; };
    void move_right()   { m_movement.x = 1.0f; };
    void move_up()      { m_movement.y = 1.0f; };
    void move_down()    { m_movement.y = -1.0f; };

    void ai_activate(Entity* player);
    void ai_g1(Entity* player);
    void ai_g2(Entity* player);
    void ai_g3(Entity* player);

    void activate() { m_is_active = true; };
    void deactivate() { m_is_active = false; };

    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;     };
    AIType     const get_ai_type()        const { return m_ai_type;         };
    AIState    const get_ai_state()       const { return m_ai_state;        };
    glm::vec3  const get_position()       const { return m_position;        };
    glm::vec3  const get_movement()       const { return m_movement;        };
    glm::vec3  const get_velocity()       const { return m_velocity;        };
    glm::vec3  const get_acceleration()   const { return m_acceleration;    };
    float      const get_jumping_power()  const { return m_jumping_power;   };
    float      const get_speed()          const { return m_speed;           };
    int        const get_width()          const { return m_width;           };
    int        const get_height()         const { return m_height;          };
    int        const get_health()         const { return m_health;          };

    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;      };
    void const set_ai_type(AIType new_ai_type)              { m_ai_type = new_ai_type;              };
    void const set_ai_state(AIState new_state)              { m_ai_state = new_state;               };
    void const set_active(bool new_active)                  { m_is_active = new_active;             };
    void const set_position(glm::vec3 new_position)         { m_position = new_position;            };
    void const set_movement(glm::vec3 new_movement)         { m_movement = new_movement;            };
    void const set_velocity(glm::vec3 new_velocity)         { m_velocity = new_velocity;            };
    void const set_speed(float new_speed)                   { m_speed = new_speed;                  };
    void const set_jumping_power(float new_jumping_power)   { m_jumping_power = new_jumping_power;  };
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration;    };
    void const set_width(float new_width)                   { m_width = new_width;                  };
    void const set_height(float new_height)                 { m_height = new_height;                };
    void const set_health(float new_health)                 { m_health = new_health;                };
    void const set_size(glm::vec3 new_size)                 { m_scale_size = new_size;
                                                              m_width     *= m_scale_size.x;
                                                              m_height    *= m_scale_size.y;        };
};
