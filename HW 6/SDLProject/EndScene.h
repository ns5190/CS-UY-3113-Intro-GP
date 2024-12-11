#include "Scene.h"

class EndScene : public Scene {
public:
    int ENEMY_COUNT = 0;
    int ITEM_COUNT = 4;
    int PUPPY_COUNT = 0;
    int EGG_COUNT = 0;
    int POOP_COUNT = 0;
    
    ~EndScene();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

