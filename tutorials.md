# Newton Game Dynamics for Irrlicht game developers

## Introduction

_what is all this about; what the Reader shall need to know to proceed; why this one is written, etc._

## Brief overview

_here all the basics of Irrlicht and Newton GD are described_

## Perfect application architecture

No, really! You'd better believe your game is perfectly designed at content as well as inside! That is the first step to succed, believe me.

We shall use **Irrlicht** because of its simpleness. But the main criteria - it satisfied all our needs - it does not need much content preparation; it contains UI features inside; extending it with **IrrKlang** will provide us with very simply interface to sound and music.

As the title promotes, we shall also use **Newton GD** physics engine. It is easy to use and so powerful - you would be impressed!

But since we want **perfect** architecture - we shall use **Lua** as well. It is a very _light_ programming language. It will be used for scripting. And here's why: as our game shall grow, it will be more and more hard to modify something. 

For example, if we shall start with complete game, being run at production and success and we will decide to modify some core feature. Let's say, we want to modify our _NPCs_. We want them fly. If we have everything set at the code side, we shall end up with messy architecture, almost unreadable and totally immutable code. Which should be compiled, although. And each update for our game shall take much megabytes for users to download.

On the other hand, if we have our game running with scripts, we shall only modify a few scripts, which describe how characters behave. Perhaps, we will add some. But this update shall use a few kilobytes and users will not even notice the upgrade process. Yet, the updated feature will be noted during gameplay!

But have no fear: we shall do everything step-by-step and break our tutorial into two parts - **the basics** and **the intermediate part**.

## Irrlicht basics

Before we start doing some game design or gamedev magick, we'd better know something about our rendering engine. Just very basics - how to create an application, which will react to **user input** and is able to load and draw some **content**.

If you think you already know everything you want about Irrlicht (or any other rendering engine you shall use - it's simple, though!) - just skip this section.

### Install Irrlicht

This section depends on the operating system you are using. But no matter which one you use, first of all - [**go and get the latest version of Irrlicht!**](http://irrlicht.sourceforge.net/downloads/).

#### Linux

_Currently I'm writing from Ubuntu. So this section will be my favorite one_

To get started under Linux, you should compile Irrlicht. Unfortunately, it is bundled with already compiled libraries for Windows, but not for Linux.

But don't be scared! It's simple! Yet, there is one caveat: **do not run `make install` on Linux! Ever!**. If you do, it will be hard for you to get rid of an old version of the software you installed in such a way. For example, when you'll make an update of that software.

To bypass this, you'd better use `checkinstall` utilitie. It creates Debian package from sources and installs it natively instead of just copying files to their places, keeping no trace of that.

So, unzip the `irrlicht-*.*.*.zip`, go to the `irrlicht-*.*.*/sources/Irrlicht` and run `make`. And, after many output, if you do not see any error messages - congratulations! You've compiled Irrlicht!

Now, and that's important, here's how you shall compile your Irrlicht applications:

    g++ <source file> -o <output executable file> -L<absolute path to Irrlicht lib/Linux dir> -I<absolute path to Irrlicht include dir> -lIrrlicht -lGL
    
For example, if your program is stored at `main.cpp` file and you've compiled Irrlicht at `/home/moo_user/irrlicht-1.8/` directory, your compiler command-line will look like this:

    g++ main.cpp -o application -L/home/moo_user/irrlicht-1.8/lib/Linux -I/home/moo_user/irrlicht-1.8/include/ -lIrrlicht -lGL
    
**Note:** there is no space between `-L` or `-I` and the path! As well, as between `-l` and the library name! That is **GNU C++** compiler command-line options format.

_Add some screenshots and QtCreator configuration example_

#### Windows

If you are using Windows, it's better for you to use **Visual Studio**. Personally I prefer **MS VS 2008 Express** because of its user interface peculiarities.

So, the main idea here is to set the project you are using to write the code within to use the Irrlicht **include** and **library** directories. 

And here is why I prefer **MS VS 2008** - that was the last Visual Studio version where all the libraries could be specified for all the projects, in one place. So you do not need to configure each solution and each project separately, from scratch.

So, you just set the **include directories** to include the `<Irrlicht dir>/include` directory and **library directories** to include the `<Irrlicht dir>/lib/Win**-visualStudio` directory (depending on your OS architecture version it may vary between `Win32-...` and `Win64-...`).

_Add some screenshots_

#### MacOS X

_I do not have MacMini yet (and I hope I'll never do) so I'll just skip this sub-section_

### First application

Our first application will show you Irrlicht basic features we will use later. They are: **mesh handling** - loading, rendering, animating, etc.; **user input handling** - reacting to keyboard and mouse events; **user interface (UI)** - displaying some information within the application window.

The good start for that is standard example from Irrlicht pack, the **04 - Movement** one. Let's take a look over its code:

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
    
        IrrlichtDevice* device = createDevice(video::EDT_OPENGL,
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
            node->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
            node->setMaterialFlag(video::EMF_LIGHTING, false);
        }
    
        /*
        Now we create another node, movable using a scene node animator. Scene
        node animators modify scene nodes and can be attached to any scene node
        like mesh scene nodes, billboards, lights and even camera scene nodes.
        Scene node animators are not only able to modify the position of a
        scene node, they can also animate the textures of an object for
        example. We create a cube scene node and attach a 'fly circle' scene
        node animator to it, letting this node fly around our sphere scene node.
        */
        scene::ISceneNode* n = smgr->addCubeSceneNode();
    
        if (n)
        {
            n->setMaterialTexture(0, driver->getTexture("../../media/t351sml.jpg"));
            n->setMaterialFlag(video::EMF_LIGHTING, false);
            scene::ISceneNodeAnimator* anim =
                smgr->createFlyCircleAnimator(core::vector3df(0,0,30), 20.0f);
            if (anim)
            {
                n->addAnimator(anim);
                anim->drop();
            }
        }
    
        /*
        The last scene node we add to show possibilities of scene node animators is
        a b3d model, which uses a 'fly straight' animator to run between to points.
        */
        scene::IAnimatedMeshSceneNode* anms =
            smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/ninja.b3d"));
    
        if (anms)
        {
            scene::ISceneNodeAnimator* anim =
                smgr->createFlyStraightAnimator(core::vector3df(100,0,60),
                core::vector3df(-100,0,60), 3500, true);
            if (anim)
            {
                anms->addAnimator(anim);
                anim->drop();
            }
    
            /*
            To make the model look right we disable lighting, set the
            frames between which the animation should loop, rotate the
            model around 180 degrees, and adjust the animation speed and
            the texture. To set the right animation (frames and speed), we
            would also be able to just call
            "anms->setMD2Animation(scene::EMAT_RUN)" for the 'run'
            animation instead of "setFrameLoop" and "setAnimationSpeed",
            but this only works with MD2 animations, and so you know how to
            start other animations. But a good advice is to not use
            hardcoded frame-numbers...
            */
            anms->setMaterialFlag(video::EMF_LIGHTING, false);
    
            anms->setFrameLoop(0, 13);
            anms->setAnimationSpeed(15);
    //      anms->setMD2Animation(scene::EMAT_RUN);
    
            anms->setScale(core::vector3df(2.f,2.f,2.f));
            anms->setRotation(core::vector3df(0,-90,0));
    //      anms->setMaterialTexture(0, driver->getTexture("../../media/sydney.bmp"));
    
        }
    
    
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
            driver->getTexture("../../media/irrlichtlogoalpha2.tga"),
            core::position2d<s32>(10,20));
    
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
    
            /* Check if keys W, S, A or D are being held down, and move the
            sphere node around respectively. */
            core::vector3df nodePosition = node->getPosition();
    
            if(receiver.IsKeyDown(irr::KEY_KEY_W))
                nodePosition.Y += MOVEMENT_SPEED * frameDeltaTime;
            else if(receiver.IsKeyDown(irr::KEY_KEY_S))
                nodePosition.Y -= MOVEMENT_SPEED * frameDeltaTime;
    
            if(receiver.IsKeyDown(irr::KEY_KEY_A))
                nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
            else if(receiver.IsKeyDown(irr::KEY_KEY_D))
                nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;
    
            node->setPosition(nodePosition);
    
            driver->beginScene(true, true, video::SColor(255,113,113,133));
    
            smgr->drawAll(); // draw the 3d scene
            device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)
    
            driver->endScene();
    
            int fps = driver->getFPS();
    
            if (lastFPS != fps)
            {
                core::stringw tmp(L"Movement Example - Irrlicht Engine [");
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
    
Here are some basics we could extract from this code:

  1. Each 3D model is a _scene node_.
  2. Primitive scene nodes (such as _cube_ or _sphere_) could be easily created with built-in functions:
   
    ```
    scene::ISceneNode* node = smgr->addSphereSceneNode();
    scene::ISceneNode* node = smgr->addCubeSceneNode();
    ```

  3. Animated 3D models (such as _character models_) could be loaded from file:

    ```
    scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../../media/ninja.b3d"));
    ```

    _Hint:_ if mesh is animated, animation could be started with:
    
        node->setFrameLoop(0, 13);
        node->setAnimationSpeed(15);
        
    _Hint:_ animation could be stopped with setting its speed to zero:
    
        node->setAnimationSpeed(0);
        
  4. Node could be described not only by its vertices and indices (_forming a set of triangles which are drawn in 3D forming a 3D model called **mesh**_) but by its **position**, **rotation** and **scale**.


    Those could be set with:

        node->setPosition(core::vector3df(x, y, z));
        node->setRotation(core::vector3df(x_angle, y_angle, z_angle));
        node->setScale(core::vector3df(width_factor, height_factor, depth_factor));
        
    _Hint:_ rotation is a set of angles relatively to the corresponding axes, the node will be rotated around. E. g., `vector3df(45, 90, 0)` sets the rotation by `45 deg` around _X axis_, `90 deg` around _Y axis_ and no rotation aroung _Z axis_.
    
    _Add image describing rotations here_

  5. Graphics User Interface' (GUI) widgets for information output are labels; they are created with **GUI Manager**:

    ```
        gui::IGUIStaticText* label = device->getGUIEnvironment()->addStaticText(L"", core::rect<s32>(10, 10, 400, 20));
    ```
        
    _Hint:_ its text could be set with:
    
        label->setText((const wchar_t*) "some text");
        
  6. User input is handled by an external `IEventReceiver` class object.
  
    Its method, `virtual bool OnEvent(const SEvent& event)` defines the logic of handling events like _mouse events_, _keyboard events_, _joystick events_, _GUI events_, etc.

    The type of event is passed with the `event.EventType` field. The corresponding field is filled with the event parameters.
    
    For example:
    
        if (event.type == EET_MOUSE_INPUT_EVENT) 
        {
            if (event.MouseInput.isLeftPressed())
            {
                printf("%d, %d is cursor position\n", event.MouseInput.X, event.mouseInput.Y);
            }
        }
        
    _Hint:_ `EventReceiver` object has nothing in common with our main game loop. So we should create some interface, some architecture trick to link those two. Because they are strongly related!
    
  7. Main game loop should contain **rendering call**, **GUI rendering call** and other game logic processing calls.

    The simplest main loop could look like this:
    
        while(device->run())
        {
            driver->beginScene(true, true, video::SColor(255,113,113,133));
    
            smgr->drawAll(); // draw the 3d scene
            device->getGUIEnvironment()->drawAll(); // draw the gui
    
            driver->endScene();
        }
        
  8. There is no simple (built-in) way to get the delta time between two rendered frames. **This is an important variable!** We'll need that later, when we inject physics engine. And Newton GD is not the only engine requiring this variable!

    But that could be easily done with this workaround:
    
        // before main loop
        u32 then = device->getTimer()->getTime();
        
        // ...
        
        // within the main game loop
        const u32 now = device->getTimer()->getTime();
        const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // delta time in seconds
        then = now;
        
---

That's all you need to know about rendering engine for now.

## Adding some Newtonianity

### Compiling Newton SDK under Linux

Writing this part took me a long time. Well, this story is very short, but I faced an unbreakable wall - misunderstanting...

So, the essence of the whole paragraph is just three steps:

  1. Download Newton 3.0.8+ SDK and unpack it somewhere on your PC
  2. Open your terminal, navigate to the directory where you've unpacked Newton SDK and run this:
        
        cd coreLibrary_300/projects/posix64 && make
        
  3. Now just copy the library to your `/usr/lib` directory:
    
        sudo cp *.so /usr/lib/
        

And you are ready to proceed with our first Newton- and Irrlicht-powered application!
    
### The first attempt

Let's start modifying our Irrlicht sample application. First of all, we shall add some Newton headers:

    #include <dMathDefines.h>
    #include <dVector.h>
    #include <dQuaternion.h>
    #include <dMatrix.h>
    #include <Newton.h>
    
These will include all the data structures and functions we shall use.

And here goes the first Newton call function:

    NewtonSetMemorySystem(AllocMemory, FreeMemory);
    
Add this right after Irrlicht initialization. And all the code below (except the _Newton world stepping_ and _function definitions_) shall be added after this line.
    
One sets the way Newton shall manage objects in memory. If you want to define your cool way to serve memory allocation and freement - just define that logic within those two methods. But for now we shall use the `stdlib` methods:

    void* AllocMemory(int sizeInBytes)
    {
        return malloc(sizeInBytes);
    }
    
    void FreeMemory(void *ptr, int sizeInBytes)
    {
        free(ptr);
    }
    
Then, we create a Newton world. This is a virtual space where all the magic is done. It shall contain all our mesh' (_visual part of The Entity_) bodies (_invisible physical part of The Entity_).

    NewtonWorld* g_world = NewtonCreate();
    NewtonSetSolverModel(g_world, 1);    

Solver is the way Newton performs calculations. That is just a configuration option. Yet, important one.

Now, let's create our first Entity! Remember the Sphere node, we created with Irrlicht? We shall now add some magic to make it act like a metal ball!

First of all, let's remember how we created our Sphere mesh:

    scene::ISceneNode * node = smgr->addSphereSceneNode();
    
    if (node)
    {
        node->setPosition(core::vector3df(0,0,30));
        node->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
        node->setMaterialFlag(video::EMF_LIGHTING, false);
    }
    
**Note:** do not forget to copy Irrlicht `/media` directory to the parent directory for the tutorial sample!

Now just add these lines right after the Sphere' mesh creation code:

    dMatrix offset(GetIdentityMatrix());
    NewtonCollision* shape = NewtonCreateSphere(g_world, 10.f, 0, &offset[0][0]);

    NewtonBody* body = NewtonCreateDynamicBody(g_world, shape, &offset[0][0]);

    NewtonBodySetUserData(body, node);

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
    
And define two functions, `SetTransformCallback` and `ApplyGravity`:

    void SetTransformCallback (const NewtonBody* body, const dFloat* matrix, int threadIndex)
    {
        scene::ISceneNode* node;
    
        // Get the position from the matrix
        dVector position(matrix[12], matrix[13], matrix[14], 1.0f);
        dQuaternion rotation;
    
        NewtonBodyGetRotation(body, &rotation.m_q0);
    
        // get the entity associated with this rigid body
        node = (scene::ISceneNode*) NewtonBodyGetUserData(body);
    
        core::vector3df node_position = core::vector3df(position[0], position[1], position[2]);

        // transform Newtonian rotation to Irrlicht rotation
        core::quaternion tmp_quaternion = core::quaternion(rotation.m_q0, rotation.m_q1, rotation.m_q2, rotation.m_q3);
        core::vector3df tmp_rotation;
        tmp_quaternion.toEuler(tmp_rotation);
        core::vector3df node_rotation(core::radToDeg(tmp_rotation.X), core::radToDeg(tmp_rotation.Y), core::radToDeg(tmp_rotation.Z));
    
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

First of them synchronizes Irrlicht meshes with their Newtonian bodies. The second one Applies some default forces and torques to bodies.

And the last step is Newton stepping. It shall tell Newton: _Hey! You! Just update my physics! Now!_ And all the magic shall be performed.

You should end up with the code like this:

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
    
    
Let's compile it and run!

     g++ main.cpp -I/home/user/newton-3.0.8/packages/posix64/ -L/home/user/newton-3.0.8/packages/posix64/ -I/home/user/newton-3.0.8/packages/dMath/ -I/home/user/irrlicht-1.8/include/ -L /home/user/irrlicht-1.8/lib/Linux/ -lNewton -lIrrlicht -lGL -o tuorial01 

**Note:** I used the home directory for user named _user_ to store Newton SDK and Irrlicht SDK. You may want to change this!

If you did everything right, you shall see the metal ball falling right through the camera and down... down... down...

## Advanced application

For now, we have no architecture. Our application is a simple Irrlicht application, processing user input, rendering graphics based on physics state of our in-game world and being totally unscallable.

Let's change the way the things go. Let's change the **architecture**! By the name or Refactoring!..

To tell the truth, I do not like refactoring my projects (unless I'm at work). I like totally rewriting them! It's the best way to learn something (as for me). If the project is not too big, it's better to write everything from beginning - you'll notice all the defects at your architecture during this creepy process.

**So, the architecture for our application is:**
  
  1. the **Core** is something like server - it takes the **messages** from scripts and **handles** them, bypassing to other scripts; also, it reads the application **configuration** and runs the scripts described at config (at startup); the main application loop is just **processing all the scripts and their messages** plus **rendering**
  2. **Script** is being run each application tick and process the **world events**, which are generated by other scripts during their runtime

### First Lua-scripted C++ application

Let's write one small application which will call some Lua script function and react to the Lua script function call. It will be the most simple scripted application you could imagine!

Here's the **C++** part:

    #include <lua.hpp>
    #include <iostream>
    
    using namespace std;
    
    int someServerHandler(lua_State  *l)
    {
        int argc = lua_gettop(l);
    
        for (int i = 0; i < argc; i++)
        {
            cout << "ARG[" << i + 1 << "] = " << lua_tostring(l, i + 1) << endl;
        }
        
        lua_pushstring(l, "m_pi");
        lua_pop(l, argc - 1);
        //lua_pushnumber(l, 3.14);
    
        return argc;
    }
    
    int main()
    {
        lua_State *l = lua_open();
        luaL_openlibs(l);
    
        lua_register(l, "someScriptEvent", someServerHandler);
    
        luaL_dofile(l, "script.lua");
        
        lua_getglobal(l, "someScriptHandler");
    
        if (!lua_isfunction(l, -1))
        {
                lua_pop(l, 1);
                cout << "Could not find function someScriptHandler" << endl;
        } else
        {
                lua_pushstring(l, "event_argument");
    
                // 1 argument, 0 results, 0 WAT?
                lua_pcall(l, 1, 0, 0);
        }
             
        lua_close(l);
    
        return 0;
    }

And here is our little script:

    function someScriptHandler(x)
        print("someScriptHandler("..x..")");
    end
    
    res = someScriptEvent("hello", "world");
    
    print(someScriptEvent());
    
    for k, v in res do
        print(k.." = "..v);
    end
    
That's it! Here what we've done: we created a C++ application which runs `script.lua` file and calls its function `someScriptHandler()`. And whenever script calls the `someScriptEvent()` function, our C++ application reacts calling its `someServerHandler()` function.

Hey! We have just written our first scripted server! Although, it does nothing useful, it's a real scripted application! Now we are cool guys and could proceed creating an AAA-games' compete!