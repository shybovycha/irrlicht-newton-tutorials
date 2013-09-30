#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS
    #pragma comment(lib, "Irrlicht.lib")
    #pragma comment(lib, "Newton.lib")
#endif

#include <irrlicht.h>

#include <dMathDefines.h>
#include <dVector.h>
#include <dQuaternion.h>
#include <dMatrix.h>
#include <Newton.h>

using namespace irr;

class MyEventReceiver : public IEventReceiver
{
public:
    virtual bool OnEvent(const SEvent& event)
    {
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

        return false;
    }

    virtual bool IsKeyDown(EKEY_CODE keyCode) const
    {
        return KeyIsDown[keyCode];
    }

    MyEventReceiver()
    {
        for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
            KeyIsDown[i] = false;
    }

private:
    // We use this array to store the current state of each key
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

// this is the call back for allocation newton memory
void* AllocMemory(int sizeInBytes)
{
    return malloc(sizeInBytes);
}

// this is the callback for freeing Newton Memory
void FreeMemory(void *ptr, int sizeInBytes)
{
    free(ptr);
}

// Transform callback to set the matrix of a the visual entity
void SetTransformCallback(const NewtonBody* body, const dFloat* matrix, int threadIndex)
{
    scene::ISceneNode* node;

    // Get the position from the matrix
    dVector position(matrix[12], matrix[13], matrix[14], 1.0f);
    dQuaternion rotation;

    // we will ignore the Rotation part of matrix and use the quaternion rotation stored in the body
    NewtonBodyGetRotation(body, &rotation.m_q0);

    // get the entity associated with this rigid body
    node = (scene::ISceneNode*) NewtonBodyGetUserData(body);

    core::vector3df node_position = core::vector3df(position[0], position[1], position[2]);

    core::quaternion tmp_quaternion = core::quaternion(rotation.m_q0, rotation.m_q1, rotation.m_q2, rotation.m_q3);
    core::vector3df tmp_rotation;
    tmp_quaternion.toEuler(tmp_rotation);
    core::vector3df node_rotation(core::radToDeg(tmp_rotation.X), core::radToDeg(tmp_rotation.Y), core::radToDeg(tmp_rotation.Z));

    // since this tutorial run the physics and a different fps than the Graphics
    // we need to save the entity current transformation state before updating the new state.
    node->setPosition(node_position);
    node->setRotation(node_rotation);
}

void ApplyGravity(const NewtonBody* body, dFloat timestep, int threadIndex)
{
    dFloat Ixx, Iyy, Izz, mass;
    NewtonBodyGetMassMatrix(body, &mass, &Ixx, &Iyy, &Izz);
    dVector force(0.f, -9.8 * mass, 0.f);
    NewtonBodySetForce(body, &force.m_x);
}

int main()
{
    // create device
    MyEventReceiver receiver;

    IrrlichtDevice* device = createDevice(irr::video::EDT_OPENGL,
            core::dimension2d<u32>(640, 480), 16, false, false, false, &receiver);

    if (device == 0)
        return 1; // could not create selected driver.

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();

    scene::ISceneNode * node = smgr->addSphereSceneNode();
    if (node)
    {
        node->setPosition(core::vector3df(0, 0, 30));
        node->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
        node->setMaterialFlag(video::EMF_LIGHTING, false);
    }

    NewtonSetMemorySystem(AllocMemory, FreeMemory);

    // create the Newton World
    NewtonWorld* g_world = NewtonCreate();

    // use the standard x87 floating point model
    // NewtonSetPlatformArchitecture(g_world, 0);

    NewtonSetSolverModel(g_world, 1);

    dMatrix offset(GetIdentityMatrix());
    NewtonCollision* shape = NewtonCreateSphere(g_world, 10.f, 0, &offset[0][0]);

    NewtonBody* body = NewtonCreateDynamicBody(g_world, shape, &offset[0][0]);

    NewtonBodySetUserData(body, node);

    /*//dMatrix matrix(node->getRotation(), ent->m_curPosition);
    //NewtonBodySetMatrix (body, &matrix[0][0]);

    dVector origin;
    dVector inertia;
    NewtonConvexCollisionCalculateInertialMatrix(shape, &inertia[0], &origin[0]);

    float mass = 16.f;
    NewtonBodySetMassMatrix(body, mass, mass * inertia.m_x, mass * inertia.m_y, mass * inertia.m_z);

    NewtonBodySetCentreOfMass(body, &origin[0]);*/

    float mass = 16.f;
    NewtonBodySetMassProperties(body, mass, shape);

    NewtonBodySetMaterialGroupID(body, 0);

    NewtonBodySetForceAndTorqueCallback(body, ApplyGravity);

    NewtonBodySetTransformCallback(body, SetTransformCallback);

    /*
    To be able to look at and move around in this scene, we create a first
    person shooter style camera and make the mouse cursor invisible.
    */
    smgr->addCameraSceneNodeFPS();
    device->getCursorControl()->setVisible(false);

    /*
    Add a colorful irrlicht logo
    */
    device->getGUIEnvironment()->addImage(
        driver->getTexture("../media/irrlichtlogoalpha2.tga"), 
        core::position2d<s32>(10, 20)
    );

    /*
    And one for Newton GD
    */
    device->getGUIEnvironment()->addImage(
        driver->getTexture("../media/newton_logo.png"),
        core::position2d<s32>(10, 130)
    );

    gui::IGUIStaticText* diagnostics = device->getGUIEnvironment()->addStaticText(
        L"", core::rect<s32>(10, 10, 400, 20)
    );

    diagnostics->setOverrideColor(video::SColor(255, 255, 255, 0));

    /*
    We have done everything, so lets draw it. We also write the current
    frames per second and the name of the driver to the caption of the
    window.
    */
    int lastFPS = -1;

    // In order to do framerate independent movement, we have to know
    // how long it was since the last frame
    u32 then = device->getTimer()->getTime();

    // This is the movemen speed in units per second.
    const f32 MOVEMENT_SPEED = 5.f;

    while(device->run())
    {
        // Work out a frame delta time.
        const u32 now = device->getTimer()->getTime();
        const f32 frameDeltaTime = (f32) (now - then) / 1000.f; // Time in seconds
        then = now;

        // Rendering goes here
        driver->beginScene(true, true, video::SColor(255, 113, 113, 133));

            smgr->drawAll(); // draw the 3d scene
            device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

        driver->endScene();

        int fps = driver->getFPS();

        // Physics simulation goes here

        // Slower simulation:
        // NewtonUpdate (g_world, (1.0f / (fps * 10000.f)));

        // Usual simulation:
        NewtonUpdate(g_world, (1.0f / fps));

        if (lastFPS != fps)
        {
            core::stringw tmp(L"Newton + Irrlicht Tutorial 01 [");
            tmp += driver->getName();
            tmp += L"] fps: ";
            tmp += fps;

            device->setWindowCaption(tmp.c_str());
            lastFPS = fps;
        }
    }

    /*
    In the end, delete the Irrlicht device.
    */
    device->drop();

    return 0;
}
