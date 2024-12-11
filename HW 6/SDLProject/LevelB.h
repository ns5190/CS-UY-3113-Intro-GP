#include "Scene.h"

class LevelB : public Scene {
public:
    int ANIMAL_COUNT = 3;
    int EGG_COUNT = 50;
    
    ~LevelB();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
