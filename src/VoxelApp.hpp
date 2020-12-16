#include "Monarch.hpp"

#include "components/Transform.hpp"
#include "components/3d/FirstPersonController.hpp"
#include "components/3d/Camera.hpp"

//#include "Octree.hpp"
#include "Volume.hpp"

class VoxelApp : public Monarch::Application {
public:
    VoxelApp();
    virtual ~VoxelApp() override;
    virtual void update() override;

    virtual void handleEvent(Monarch::Event& e) override;
private:
    bool handleKey(Monarch::KeyPressedEvent& e);

    uint8_t density(glm::vec3 pos);

    static int width;
    static int height;
    bool cursor = false;

    Monarch::Entity camera;
    Monarch::Entity volume;
};
