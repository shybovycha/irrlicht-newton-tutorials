/** Example 004 Movement

This Tutorial shows how to move and animate SceneNodes. The
basic concept of SceneNodeAnimators is shown as well as manual
movement of nodes using the keyboard.  We'll demonstrate framerate
independent movement, which means moving by an amount dependent
on the duration of the last run of the Irrlicht loop.

Example 19.MouseAndJoystick shows how to handle those kinds of input.

As always, I include the header files, use the irr namespace,
and tell the linker to link with the .lib file.
*/
#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>

#include <dMathDefines.h>
#include <dVector.h>
#include <dQuaternion.h>
#include <dMatrix.h>
#include <Newton.h>

using namespace irr;

/*
To receive events like mouse and keyboard input, or GUI events like "the OK
button has been clicked", we need an object which is derived from the
irr::IEventReceiver object. There is only one method to override:
irr::IEventReceiver::OnEvent(). This method will be called by the engine once
when an event happens. What we really want to know is whether a key is being
held down, and so we will remember the current state of each key.
*/
class MyEventReceiver : public IEventReceiver
{
public:
    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

        return false;
    }

    // This is used to check whether a key is being held down
    virtual bool IsKeyDown(EKEY_CODE keyCode) const
    {
        return KeyIsDown[keyCode];
    }

    MyEventReceiver()
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
            KeyIsDown[i] = false;
    }

private:
    // We use this array to store the current state of each key
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

// this is the call back for allocation newton memory
void* AllocMemory (int sizeInBytes)
{
    return malloc (sizeInBytes);
}

// this is the callback for freeing Newton Memory
void FreeMemory (void *ptr, int sizeInBytes)
{
    free (ptr);
}

// Transform callback to set the matrix of a the visual entity
void SetTransformCallback (const NewtonBody* body, const dFloat* matrix, int threadIndex)
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

/*
The event receiver for keeping the pressed keys is ready, the actual responses
will be made inside the render loop, right before drawing the scene. So lets
just create an irr::IrrlichtDevice and the scene node we want to move. We also
create some other additional scene nodes, to show that there are also some
different possibilities to move and animate scene nodes.
*/
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

    /*
    Create the node which will be moved with the WSAD keys. We create a
    sphere node, which is a built-in geometry primitive. We place the node
    at (0,0,30) and assign a texture to it to let it look a little bit more
    interesting. Because we have no dynamic lights in this scene we disable
    lighting for each model (otherwise the models would be black).
    */
    scene::ISceneNode * node = smgr->addSphereSceneNode();
    if (node)
    {
        node->setPosition(core::vector3df(0,0,30));
        node->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
        node->setMaterialFlag(video::EMF_LIGHTING, false);
    }

    NewtonSetMemorySystem (AllocMemory, FreeMemory);

    // create the Newton World
    NewtonWorld* g_world = NewtonCreate ();

    // use the standard x87 floating point model
    // NewtonSetPlatformArchitecture (g_world, 0);

    NewtonSetSolverModel (g_world, 1);

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
        core::position2d<s32>(10,20));

    /*
    And one for Newton GD
    */
    device->getGUIEnvironment()->addImage(
        driver->getTexture("../media/newton_logo.png"),
        core::position2d<s32>(10, 130));

    gui::IGUIStaticText* diagnostics = device->getGUIEnvironment()->addStaticText(
        L"", core::rect<s32>(10, 10, 400, 20));
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
        const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
        then = now;

        driver->beginScene(true, true, video::SColor(255,113,113,133));

        smgr->drawAll(); // draw the 3d scene
        device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

        driver->endScene();

        int fps = driver->getFPS();

        NewtonUpdate (g_world, (1.0f / fps));

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

/*
That's it. Compile and play around with the program.
**/

