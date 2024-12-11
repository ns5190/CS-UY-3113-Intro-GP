#include "Scene.h"

class LevelC : public Scene {
public:
    int ANIMAL_COUNT = 3;
    int POOP_COUNT = 50;
    
    ~LevelC();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

