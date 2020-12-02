#include "Monarch.hpp"

#include "components/Transform.hpp"
#include "components/3d/FirstPersonController.hpp"
#include "components/3d/Camera.hpp"

#include "Octree.hpp"

class VoxelApp : public Monarch::Application {
public:
    VoxelApp();
    virtual ~VoxelApp() override;
    virtual void update() override;

    virtual void handleEvent(Monarch::Event& e) override;
private:
    bool handleKey(Monarch::KeyPressedEvent& e);

    static int width;
    static int height;
    bool cursor = false;

    Monarch::Entity camera;

    Octree octree;
};
