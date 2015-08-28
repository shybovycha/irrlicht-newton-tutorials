---
layout: post
date: '2015-08-28T18:06:00+01:00'
---

<h2 id="adding-some-newtonianity">Adding some Newtonianity</h2>
<h3 id="compiling-newton-sdk-under-linux">Compiling Newton SDK under Linux</h3>
<p>Writing this part took me a long time. Well, this story is very short, but I faced an unbreakable wall - misunderstanting...</p>
<p>So, the essence of the whole paragraph is just three steps:</p>
<ol>
<li>Download Newton 3.0.8+ SDK and unpack it somewhere on your PC</li>
<li>
<p>Open your terminal, navigate to the directory where you've unpacked Newton SDK and run this:</p>
<pre><code>cd coreLibrary_300/projects/posix64 && make
</code></pre>
</li>
<li>
<p>Now just copy the library to your <code>/usr/lib</code> directory:</p>
<pre><code>sudo cp *.so /usr/lib/
</code></pre>
</li>
</ol>
<h3>Compoling under Windows</h3>
<p>There are some tricks under this platform. For Newton 3.10 particulally. The first trick is the large amount of similar directories
with sources and projects. And you could be scared when first taking a look over. Just follow these instructions and you shall not be lost!
</p>
<ol>
    <li>Unpack the downloaded archive somewhere, let's say <i>NEWTON_HOME</i></li>
    <li>Navigate to the <pre>NEWTON_HOME/packages/projects/visualStudio_2008</pre> <i>(replace 2008 with the VS version you have installed)</i></li>
    <li>Run the <strong>build.sln</strong> file <i>(will be opened in the VS)</i></li>
    <li>Sequently, choose <b>DebugDLL</b> and when done <b>ReleaseDLL</b> build mode and press <b>Build</b></li>
    <li>Perform the same build steps for <pre>newton.sln</pre> file at <pre>NEWTON_HOME/coreLibrary_300/projects/windows/project_vs2008/</pre> directory</li>
</ol>
<p>By default, libraries and DLLs are built to... not very handy places. So we shall add few extra steps to make a life easier:</p>
<ol>
    <li>Create a directory <pre>NEWTON_HOME/lib</pre></li>
    <li>Copy all the <pre>*.lib</pre> and <pre>*.dll</pre> files from <pre>NEWTON_HOME/packages/projects/visualStudio_2008/Win32/</pre> dir to <pre>NEWTON_HOME/lib</pre></li>
    <li>Perform the same copying pattern for files within the <pre>NEWTON_HOME/coreLibrary_300/projects/windows/project_vs2008/Win32/</pre> directory</li>
    <li>Set your IDE to take the library files from <pre>NEWTON_HOME/lib</pre> firectory <i>(for my VS2008 Express and lower that is <b>Tools [Top Menu] -> Options... -> Projects and solutions [left folder] -> VC++ Directories [sub-folder] -> Library files [right top dropdown]</b>)</i></li>
</ol>
<p>And you are ready to proceed with our first Newton- and Irrlicht-powered application!</p>
<h3 id="the-first-attempt">The first attempt</h3>
<p>Let's start modifying our Irrlicht sample application. First of all, we shall add some Newton headers:</p>
{% highlight cpp %}
#include<dMathDefines.h>
#include<dVector.h>
#include<dQuaternion.h>
#include<dMatrix.h>
#include<Newton.h>
{% endhighlight %}
<p>These will include all the data structures and functions we shall use.</p>
<p>And here goes the first Newton call function:</p>
<pre><code>NewtonSetMemorySystem(AllocMemory, FreeMemory);
</code></pre>
<p>Add this right after Irrlicht initialization. And all the code below (except the <em>Newton world stepping</em> and <em>function definitions</em>) shall be added after this line.</p>
<p>One sets the way Newton shall manage objects in memory. If you want to define your cool way to serve memory allocation and freement - just define that logic within those two methods. But for now we shall use the <code>stdlib</code> methods:</p>
{% highlight cpp %}
void* AllocMemory(int sizeInBytes)
{
    return malloc(sizeInBytes);
}

void FreeMemory(void *ptr, int sizeInBytes)
{
    free(ptr);
}
{% endhighlight %}
<p>Then, we create a Newton world. This is a virtual space where all the magic is done. It shall contain all our mesh' (<em>visual part of The Entity</em>) bodies (<em>invisible physical part of The Entity</em>).</p>
{% highlight cpp %}
NewtonWorld* g_world = NewtonCreate();
NewtonSetSolverModel(g_world, 1);
{% endhighlight %}
<p>Solver is the way Newton performs calculations. That is just a configuration option. Yet, important one.</p>
<p>Now, let's create our first Entity! Remember the Sphere node, we created with Irrlicht? We shall now add some magic to make it act like a metal ball!</p>
<p>First of all, let's remember how we created our Sphere mesh:</p>
{% highlight cpp %}
scene::ISceneNode * node = smgr->addSphereSceneNode();

if (node)
{
    node->setPosition(core::vector3df(0,0,30));
    node->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
    node->setMaterialFlag(video::EMF_LIGHTING, false);
}
{% endhighlight %}
<p><strong>Note:</strong> do not forget to copy Irrlicht <code>/media</code> directory to the parent directory for the tutorial sample!</p>
<p>Now just add these lines right after the Sphere' mesh creation code:</p>
{% highlight cpp %}
dMatrix offset(GetIdentityMatrix());
NewtonCollision* shape = NewtonCreateSphere(g_world, 10.f, 0, &offset[0][0]);

NewtonBody* body = NewtonCreateDynamicBody(g_world, shape, &offset[0][0]);

NewtonBodySetUserData(body, node);

dVector origin;
dVector inertia;
NewtonConvexCollisionCalculateInertialMatrix(shape, &inertia[0], &origin[0]);

float mass = 16.f;
NewtonBodySetMassMatrix(body, mass, mass * inertia.m_x, mass * inertia.m_y, mass * inertia.m_z);

NewtonBodySetCentreOfMass(body, &origin[0]);

float mass = 16.f;
NewtonBodySetMassProperties(body, mass, shape);

NewtonBodySetMaterialGroupID(body, 0);

NewtonBodySetForceAndTorqueCallback(body, ApplyGravity);

NewtonBodySetTransformCallback(body, SetTransformCallback);
{% endhighlight %}
<p>And define two functions, <code>SetTransformCallback</code> and <code>ApplyGravity</code>:</p>
<pre><code>void SetTransformCallback (const NewtonBody* body, const dFloat* matrix, int threadIndex)
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
</code></pre>
<p>First of them synchronizes Irrlicht meshes with their Newtonian bodies. The second one Applies some default forces and torques to bodies.</p>
<p>And the last step is Newton stepping. It shall tell Newton: <em>Hey! You! Just update my physics! Now!</em> And all the magic shall be performed.</p>
<p>You should end up with the code like this:</p>
{% highlight cpp %}
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
#pragma comment(lib, "Newton.lib")
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
{% endhighlight %}
<p>Let's compile it and run!</p>
<i>Linux build command:</i>
{% highlight bash %}
g++ main.cpp -I/home/user/newton-3.0.8/packages/posix64/ -L/home/user/newton-3.0.8/packages/posix64/ -I/home/user/newton-3.0.8/packages/dMath/ -I/home/user/irrlicht-1.8/include/ -L /home/user/irrlicht-1.8/lib/Linux/ -lNewton -lIrrlicht -lGL -o tuorial01
{% endhighlight %}
<p><strong>Hint:</strong> to make simulation slower and so watch ball falling in detail, make the <i>NewtonUpdate</i> FPS argument even smaller. A thousand times, say.</p>
<p><strong>Note:</strong> I used the home directory for user named <em>user</em> to store Newton SDK and Irrlicht SDK. You may want to change this!</p>
<p>If you did everything right, you shall see the metal ball falling right through the camera and down... down... down...</p>
