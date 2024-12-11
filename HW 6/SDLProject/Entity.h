#include "Map.h"

enum EntityType { PLATFORM, PLAYER, ANIMAL, ITEM };
enum AIType     { DOG, CHICKEN, PIG              };
enum AIState    { WALKING, IDLE, RUNNING         };

class Entity
{
private:
    bool m_is_active = true;

    int* m_animation_right = NULL, // move to the right
        * m_animation_left = NULL, // move to the left
        * m_animation_up   = NULL, // move upwards
        * m_animation_down = NULL; // move downwards

    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    
    float     m_speed;
    glm::vec3 m_movement;
    glm::mat4 m_model_matrix;
    glm::vec3 m_scale_size = glm::vec3(1.0f, 1.0f, 0.0f);
    float     m_rotation;

    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;

    float m_width = 1;
    float m_height = 1;


public:

    static const int    SECONDS_PER_FRAME = 4;
    static const int    LEFT    = 0,
                        RIGHT   = 1,
                        UP      = 2,
                        DOWN    = 3;


    int** m_walking = new int* [4]
        {
            m_animation_left,
            m_animation_right,
            m_animation_up,
            m_animation_down
        };

    int m_animation_frames  = 0,
        m_animation_index   = 0,
        m_animation_cols    = 0,
        m_animation_rows    = 0;

    int*    m_animation_indices = NULL;
    float   m_animation_time = 0.0f;


    bool  m_is_holding = false;
    int   m_holding_index;


    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;
    bool m_collided_animal = false;

    bool m_can_lay = false;
    bool m_laid_egg = false;
    
    float m_finding_poop = 0;
    bool  m_spawn_poop = false;
    
    GLuint    m_texture_id;

    Entity();
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map);
    void render(ShaderProgram* program);

    bool const check_collision(Entity* other) const;
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);

    void const check_collision_y(Map* map);
    void const check_collision_x(Map* map);

    void move_left()    { m_movement.x = -1.0f; };
    void move_right()   { m_movement.x = 1.0f; };
    void move_up()      { m_movement.y = 1.0f; };
    void move_down()    { m_movement.y = -1.0f; };

    void ai_activate(float delta_time, Entity* player);
    void ai_dog(Entity* player);
    void ai_chicken(Entity* player);
    void ai_pig(float delta_time, Entity* player);
    
    void activate()   { m_is_active = true; };
    void deactivate() { m_is_active = false; };

    EntityType const get_entity_type()    const { return m_entity_type;     };
    AIType     const get_ai_type()        const { return m_ai_type;         };
    AIState    const get_ai_state()       const { return m_ai_state;        };
    glm::vec3  const get_position()       const { return m_position;        };
    glm::vec3  const get_movement()       const { return m_movement;        };
    glm::vec3  const get_velocity()       const { return m_velocity;        };
    glm::vec3  const get_acceleration()   const { return m_acceleration;    };
    float      const get_speed()          const { return m_speed;           };
    int        const get_width()          const { return m_width;           };
    int        const get_height()         const { return m_height;          };
    bool       const get_active()         const { return m_is_active;       };


    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;      };
    void const set_ai_type(AIType new_ai_type)              { m_ai_type = new_ai_type;              };
    void const set_ai_state(AIState new_state)              { m_ai_state = new_state;               };
    void const set_position(glm::vec3 new_position)         { m_position = new_position;            };
    void const set_movement(glm::vec3 new_movement)         { m_movement = new_movement;            };
    void const set_velocity(glm::vec3 new_velocity)         { m_velocity = new_velocity;            };
    void const set_speed(float new_speed)                   { m_speed = new_speed;                  };
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration;    };
    void const set_width(float new_width)                   { m_width = new_width;                  };
    void const set_height(float new_height)                 { m_height = new_height;                };
    void const set_size(glm::vec3 new_size)                 { m_scale_size = new_size;
                                                              m_width     *= m_scale_size.x;
                                                              m_height    *= m_scale_size.y;        };
};
