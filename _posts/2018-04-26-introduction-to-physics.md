---
layout: post
title: Introduction to physics
date: '2018-04-26T18:06:00+10:00'
order: 70
---

***TODO***

## Gravity

Let's start modifying our Irrlicht sample application. First of all, we will add some Newton headers:

```cpp
#include "newton-dynamics-master/coreLibrary_300/source/newton/Newton.h"
#include "newton-dynamics-master/packages/dMath/dVector.h"
#include "newton-dynamics-master/packages/dMath/dMatrix.h"
#include "newton-dynamics-master/packages/dMath/dQuaternion.h"
```

The basic thing in the whole Newton GD library is `NewtonWorld`. That is what it means - the world, where
all the physics happen. It is something different from where we place our 3D models. And that should be
obvious - graphics are managed by Irrlicht and physics - by Newton. Those are totally different libraries.
So we need to tie those two so that graphics correspond to what happens in _physical_ world.

First of all, we need to have a variable for our `NewtonWorld`. And since physics are handled by scripts too,
we need to have that variable close to our other objects - in the `ScriptManager` class.

There are two functions we need to bind to our `NewtonBody`:

```cpp
static void transformCallback(const NewtonBody* body, const dFloat* matrix, int threadIndex) {
    // update ISceneNode so that it is in the same position and rotation as the NewtonBody
}

static void applyForceAndTorqueCallback(const NewtonBody* body, dFloat timestep, int threadIndex) {
    // just add gravity to our body
    dFloat Ixx, Iyy, Izz;
    dFloat mass;

    NewtonBodyGetMassMatrix(body, &mass, &Ixx, &Iyy, &Izz);

    dVector gravityForce(0.0f, mass * -9.8f, 0.0f, 1.0f);
    NewtonBodySetForce(body, &gravityForce[0]);
}
```

The first one, `transformCallback`, is called whenever body changes its transform -
e. g. either position or rotation. This is a good place to synchronize our Irrlicht meshes'
positions with their Newton bodies.

The `applyForceAndTorqueCallback` function is called on each `NewtonUpdate` to set the final
forces and torques for bodies. We will modify this one later, but for now its implementation
is just good.

But what's with that `NewtonUpdate`? This is a function, which does as it says: it
updates `NewtonWorld` and all its bodies, taking into account the time since the
last update. This function call has one great candidate to be placed into: `handleFrame`.
But we need to modify that method to receive the time since the last frame been rendered
and we will use this time to update `NewtonWorld` too.

```cpp
private:
    void updatePhysics(float dt) {
         NewtonUpdate(newtonWorld, dt);
    }

public:
    void handleFrame(float dt) {
        auto handler = luaState.GetGlobalEnvironment().Get<LuaFunction<void(void)>>("handleFrame");

        updatePhysics(dt);
        setKeyStates();

        handler.Invoke();
    }
```

Remember about architecture: everything, what needs to be exposed to our scripts should be
declared as `public` in our `ScriptManager`. Everything else - as `protected` or `private`.
This is the basic principle of *encapsulation*, so let's keep it in our application.

And update the main application loop:

```cpp
while (device->run()) {
    // Work out a frame delta time.
    const u32 now = device->getTimer()->getTime();
    const f32 frameDeltaTime = (f32) (now - then);
    then = now;

    // ...

    scriptMgr->handleFrame(frameDeltaTime);

    // ...
}
```

**Hint:** to make simulation slower and so watch ball falling in detail, make the
`NewtonUpdate` argument even smaller. Thousand times, say.

Since we have initialization for our Newton stuff, we need to clean it up at the exit
to prevent memory leaks. Let's declare a method for that:

```cpp
private:
    void stopPhysics() {
        NewtonDestroyAllBodies(newtonWorld);
        NewtonDestroy(newtonWorld);
    }
```

And call it right before the program's end:

```cpp
device->drop();

scriptMgr->handleExit();
delete scriptMgr;
```

And now is the right moment to add key codes' definitions and exit function to our
`ScriptManager` so that we could write more clear code and close our application
correctly, using, say, <kbd>Esc</kbd> key.

To stop our application, we need to break our `while (device->run())` loop. This could be
achieved by simply closing the `IrrlichtDevice` with `device->closeDevice()`. But we
do not have an access to the device from the `ScriptManager`. So let's add it as a
constructor argument:

```cpp
private:
    irr::IrrlichtDevice *device;

public:
    ScriptManager(irr::IrrlichtDevice *_device, scene::ISceneManager *_smgr, video::IVideoDriver *_driver) {
        driver = _driver;
        smgr = _smgr;
        device = _device;

        initPhysics();
    }
```

So now we can create a function, exposed to our scripts, which will stop our application:

```cpp
void exit() {
    device->closeDevice();
}
```

And bind it to the Lua function:

```cpp
auto exitFn = luaState.CreateFunction<void(void)>(
                [&]() -> void { exit(); });

global.Set("exit", exitFn);
```

Now we can use our `exit` function in the Lua scripts. But we will need to use hexadecimal
key codes and that's... ugly. So we need to define some symbolic names for those codes:

```cpp
void setGlobalVariables() {
    setKeyStates();
    setKeyCodeConstants();
}

void setKeyCodeConstants() {
    std::map<std::string, int> keyMapping = {
        { "KEY_LBUTTON", 0x01 }, // Left mouse button
        { "KEY_RBUTTON", 0x02 }, // Right mouse button
        { "KEY_CANCEL", 0x03 }, // Control-break processing
        { "KEY_MBUTTON", 0x04 }, // Middle mouse button (three-button mouse)
        { "KEY_XBUTTON1", 0x05 }, // Windows 2000/XP: X1 mouse button
        { "KEY_XBUTTON2", 0x06 }, // Windows 2000/XP: X2 mouse button
        { "KEY_BACK", 0x08 }, // BACKSPACE key
        { "KEY_TAB", 0x09 }, // TAB key
        { "KEY_CLEAR", 0x0C }, // CLEAR key
        { "KEY_RETURN", 0x0D }, // ENTER key
        { "KEY_SHIFT", 0x10 }, // SHIFT key
        { "KEY_CONTROL", 0x11 }, // CTRL key
        { "KEY_MENU", 0x12 }, // ALT key
        { "KEY_PAUSE", 0x13 }, // PAUSE key
        { "KEY_CAPITAL", 0x14 }, // CAPS LOCK key
        { "KEY_KANA", 0x15 }, // IME Kana mode
        { "KEY_HANGUEL", 0x15 }, // IME Hanguel mode (maintained for compatibility use KEY_HANGUL)
        { "KEY_HANGUL", 0x15 }, // IME Hangul mode
        { "KEY_JUNJA", 0x17 }, // IME Junja mode
        { "KEY_FINAL", 0x18 }, // IME final mode
        { "KEY_HANJA", 0x19 }, // IME Hanja mode
        { "KEY_KANJI", 0x19 }, // IME Kanji mode
        { "KEY_ESCAPE", 0x1B }, // ESC key
        { "KEY_CONVERT", 0x1C }, // IME convert
        { "KEY_NONCONVERT", 0x1D }, // IME nonconvert
        { "KEY_ACCEPT", 0x1E }, // IME accept
        { "KEY_MODECHANGE", 0x1F }, // IME mode change request
        { "KEY_SPACE", 0x20 }, // SPACEBAR
        { "KEY_PRIOR", 0x21 }, // PAGE UP key
        { "KEY_NEXT", 0x22 }, // PAGE DOWN key
        { "KEY_END", 0x23 }, // END key
        { "KEY_HOME", 0x24 }, // HOME key
        { "KEY_LEFT", 0x25 }, // LEFT ARROW key
        { "KEY_UP", 0x26 }, // UP ARROW key
        { "KEY_RIGHT", 0x27 }, // RIGHT ARROW key
        { "KEY_DOWN", 0x28 }, // DOWN ARROW key
        { "KEY_SELECT", 0x29 }, // SELECT key
        { "KEY_PRINT", 0x2A }, // PRINT key
        { "KEY_EXECUT", 0x2B }, // EXECUTE key
        { "KEY_SNAPSHOT", 0x2C }, // PRINT SCREEN key
        { "KEY_INSERT", 0x2D }, // INS key
        { "KEY_DELETE", 0x2E }, // DEL key
        { "KEY_HELP", 0x2F }, // HELP key
        { "KEY_KEY_0", 0x30 }, // 0 key
        { "KEY_KEY_1", 0x31 }, // 1 key
        { "KEY_KEY_2", 0x32 }, // 2 key
        { "KEY_KEY_3", 0x33 }, // 3 key
        { "KEY_KEY_4", 0x34 }, // 4 key
        { "KEY_KEY_5", 0x35 }, // 5 key
        { "KEY_KEY_6", 0x36 }, // 6 key
        { "KEY_KEY_7", 0x37 }, // 7 key
        { "KEY_KEY_8", 0x38 }, // 8 key
        { "KEY_KEY_9", 0x39 }, // 9 key
        { "KEY_KEY_A", 0x41 }, // A key
        { "KEY_KEY_B", 0x42 }, // B key
        { "KEY_KEY_C", 0x43 }, // C key
        { "KEY_KEY_D", 0x44 }, // D key
        { "KEY_KEY_E", 0x45 }, // E key
        { "KEY_KEY_F", 0x46 }, // F key
        { "KEY_KEY_G", 0x47 }, // G key
        { "KEY_KEY_H", 0x48 }, // H key
        { "KEY_KEY_I", 0x49 }, // I key
        { "KEY_KEY_J", 0x4A }, // J key
        { "KEY_KEY_K", 0x4B }, // K key
        { "KEY_KEY_L", 0x4C }, // L key
        { "KEY_KEY_M", 0x4D }, // M key
        { "KEY_KEY_N", 0x4E }, // N key
        { "KEY_KEY_O", 0x4F }, // O key
        { "KEY_KEY_P", 0x50 }, // P key
        { "KEY_KEY_Q", 0x51 }, // Q key
        { "KEY_KEY_R", 0x52 }, // R key
        { "KEY_KEY_S", 0x53 }, // S key
        { "KEY_KEY_T", 0x54 }, // T key
        { "KEY_KEY_U", 0x55 }, // U key
        { "KEY_KEY_V", 0x56 }, // V key
        { "KEY_KEY_W", 0x57 }, // W key
        { "KEY_KEY_X", 0x58 }, // X key
        { "KEY_KEY_Y", 0x59 }, // Y key
        { "KEY_KEY_Z", 0x5A }, // Z key
        { "KEY_LWIN", 0x5B }, // Left Windows key (Microsoft� Natural� keyboard)
        { "KEY_RWIN", 0x5C }, // Right Windows key (Natural keyboard)
        { "KEY_APPS", 0x5D }, // Applications key (Natural keyboard)
        { "KEY_SLEEP", 0x5F }, // Computer Sleep key
        { "KEY_NUMPAD0", 0x60 }, // Numeric keypad 0 key
        { "KEY_NUMPAD1", 0x61 }, // Numeric keypad 1 key
        { "KEY_NUMPAD2", 0x62 }, // Numeric keypad 2 key
        { "KEY_NUMPAD3", 0x63 }, // Numeric keypad 3 key
        { "KEY_NUMPAD4", 0x64 }, // Numeric keypad 4 key
        { "KEY_NUMPAD5", 0x65 }, // Numeric keypad 5 key
        { "KEY_NUMPAD6", 0x66 }, // Numeric keypad 6 key
        { "KEY_NUMPAD7", 0x67 }, // Numeric keypad 7 key
        { "KEY_NUMPAD8", 0x68 }, // Numeric keypad 8 key
        { "KEY_NUMPAD9", 0x69 }, // Numeric keypad 9 key
        { "KEY_MULTIPLY", 0x6A }, // Multiply key
        { "KEY_ADD", 0x6B }, // Add key
        { "KEY_SEPARATOR", 0x6C }, // Separator key
        { "KEY_SUBTRACT", 0x6D }, // Subtract key
        { "KEY_DECIMAL", 0x6E }, // Decimal key
        { "KEY_DIVIDE", 0x6F }, // Divide key
        { "KEY_F1", 0x70 }, // F1 key
        { "KEY_F2", 0x71 }, // F2 key
        { "KEY_F3", 0x72 }, // F3 key
        { "KEY_F4", 0x73 }, // F4 key
        { "KEY_F5", 0x74 }, // F5 key
        { "KEY_F6", 0x75 }, // F6 key
        { "KEY_F7", 0x76 }, // F7 key
        { "KEY_F8", 0x77 }, // F8 key
        { "KEY_F9", 0x78 }, // F9 key
        { "KEY_F10", 0x79 }, // F10 key
        { "KEY_F11", 0x7A }, // F11 key
        { "KEY_F12", 0x7B }, // F12 key
        { "KEY_F13", 0x7C }, // F13 key
        { "KEY_F14", 0x7D }, // F14 key
        { "KEY_F15", 0x7E }, // F15 key
        { "KEY_F16", 0x7F }, // F16 key
        { "KEY_F17", 0x80 }, // F17 key
        { "KEY_F18", 0x81 }, // F18 key
        { "KEY_F19", 0x82 }, // F19 key
        { "KEY_F20", 0x83 }, // F20 key
        { "KEY_F21", 0x84 }, // F21 key
        { "KEY_F22", 0x85 }, // F22 key
        { "KEY_F23", 0x86 }, // F23 key
        { "KEY_F24", 0x87 }, // F24 key
        { "KEY_NUMLOCK", 0x90 }, // NUM LOCK key
        { "KEY_SCROLL", 0x91 }, // SCROLL LOCK key
        { "KEY_LSHIFT", 0xA0 }, // Left SHIFT key
        { "KEY_RSHIFT", 0xA1 }, // Right SHIFT key
        { "KEY_LCONTROL", 0xA2 }, // Left CONTROL key
        { "KEY_RCONTROL", 0xA3 }, // Right CONTROL key
        { "KEY_LMENU", 0xA4 }, // Left MENU key
        { "KEY_RMENU", 0xA5 }, // Right MENU key
        { "KEY_OEM_1", 0xBA }, // for US    ";:"
        { "KEY_PLUS", 0xBB }, // Plus Key   "+"
        { "KEY_COMMA", 0xBC }, // Comma Key  ","
        { "KEY_MINUS", 0xBD }, // Minus Key  "-"
        { "KEY_PERIOD", 0xBE }, // Period Key "."
        { "KEY_OEM_2", 0xBF }, // for US    "/?"
        { "KEY_OEM_3", 0xC0 }, // for US    "`~"
        { "KEY_OEM_4", 0xDB }, // for US    "[{"
        { "KEY_OEM_5", 0xDC }, // for US    "\|"
        { "KEY_OEM_6", 0xDD }, // for US    "]}"
        { "KEY_OEM_7", 0xDE }, // for US    "'""
        { "KEY_OEM_8", 0xDF }, // None
        { "KEY_OEM_AX", 0xE1 }, // for Japan "AX"
        { "KEY_OEM_102", 0xE2 }, // "<>" or "\|"
        { "KEY_ATTN", 0xF6 }, // Attn key
        { "KEY_CRSEL", 0xF7 }, // CrSel key
        { "KEY_EXSEL", 0xF8 }, // ExSel key
        { "KEY_EREOF", 0xF9 }, // Erase EOF key
        { "KEY_PLAY", 0xFA }, // Play key
        { "KEY_ZOOM", 0xFB }, // Zoom key
        { "KEY_PA1", 0xFD }, // PA1 key
        { "KEY_OEM_CLEAR", 0xFE }, // Clear key
    };

    for (auto it = keyMapping.begin(); it != keyMapping.end(); ++it) {
        luaState.GetGlobalEnvironment().Set(it->first, it->second);
    }
}
```

Now we can create a <kbd>Esc</kbd> key handler in our script:

```lua
function handleFrame()
    -- Esc
    if KEY_STATE[KEY_ESCAPE] == true then
        exit()
    end
end
```

Now we are ready to create our first Newton bodies. Bodies are some invisible objects,
which define how our Irrlicht meshes will behave (e. g. where they will be placed,
how they will interact when moving, etc.). Basically, there are two types of bodies:

1. **dynamic**, whose movement is determined by the forces, applied to them
2. **kinematic**, which are controlled by setting their velocities

Those two kinds of bodies are totally different, so the interactions between them
are not pre-defined. So when your dynamic body will fall onto a kinematic one, it will
fall through.

And each body has its shape, which determines behaviour of the body, when it collides others
and the collision detection itself, of course. Shapes could be **convex** or **concave**.
Convex shapes are easier to work with _(on the level of physics simulation)_, but not all the
bodies in practice are convex. For example, levels are oftenly concave. So they need their special
shapes, which are called `Triangle Mesh`.

**Note:** to keep the performance of your application high, try to minimalize the use of
triangle meshes and use as simple shapes, as possible. Sometimes it is more effective to
combine a set of primitive shapes, like spheres, cylinders and boxes into one **compound**
shape, then to use a trimesh.

Let's create our first simple scene, empowered with physics! We will need only two things:

1. a sphere
2. the floor

Since we do not have the good mesh in standard Irrlicht distribution for the floor
_(there is a Quake-like level, but that is too much for our case)_, we will learn how
to make that simple thing in Blender. The next part is a short break between coding
sessions.

We discussed collision shapes a bit. So let's create one for our brand new model!

We have a nice ramp to work with. But how we can reconstruct the same shape in the
terms of Newton? Newton offers a set of collision shapes for us:

* Sphere

    <img src="{{ site.baseurl }}/images/collision_shapes/sphere.jpg" alt="Sphere" class="img-responsive">

* Box

    <img src="{{ site.baseurl }}/images/collision_shapes/box.jpg" alt="Box" class="img-responsive">

* Cone

    <img src="{{ site.baseurl }}/images/collision_shapes/cone.jpg" alt="Cone" class="img-responsive">

* Capsule

    <img src="{{ site.baseurl }}/images/collision_shapes/capsule.jpg" alt="Capsule" class="img-responsive">

* Cylinder

    <img src="{{ site.baseurl }}/images/collision_shapes/cylinder.jpg" alt="Cylinder" class="img-responsive">

* Chamfer Cylinder

    <img src="{{ site.baseurl }}/images/collision_shapes/chamfer_cylinder.jpg" alt="Chamfer Cylinder" class="img-responsive">

* Convex Hull

    <img src="{{ site.baseurl }}/images/collision_shapes/convex_hull.jpg" alt="Convex Hull" class="img-responsive">

* Trimesh

    <img src="{{ site.baseurl }}/images/collision_shapes/trimesh.jpg" alt="Trimesh" class="img-responsive">

Obviously, not sphere, cone, capsule, nor cylinder make sense for us.
We could use box shape, but then we simply ignore our inner faces (inside walls):

<img src="{{ site.baseurl }}/images/collision_shapes/ramp_box.png" alt="Box collision shape for our ramp" class="img-responsive">

A bit better, but still the same situation with convex hull shape:

<img src="{{ site.baseurl }}/images/collision_shapes/ramp_convex.png" alt="Convex hull collision shape for our ramp" class="img-responsive">

Generally, the way we create our Newtonian body is:

1. create collision shape
2. create blank Newtonian body
3. set body properties like collision shape, mass, inertia parameters, etc.
4. store the pointer to the graphical entity for that body in the `userData` property

And then Newton Game Dynamics will take your body into account when processing other objects
in the `NewtonWorld`.

## Tree mesh collision shape

So we gonna use the triangle mesh shape. What we gonna do is loop through all the triangles
of our mesh and build its copy, but in the world of _"physic"_ bodies.

To loop through all the triangles, we need to take each 3 edges of our mesh and the corresponding
vertices _(because each edge is represented by two its vertices - their indexes in the list
of vertices)_ and create Newtonian triangle. Irrlicht stores vertices in one of three formats:

1. plain vertex, represented by its three coordinates; the `irr::video::S3DVertex` class in Irrlicht
2. vertex with texture coordinates; `irr::video::S3DVertex2TCoords` class
3. vertex with its tangent information; `irr::video::S3DVertexTangents` class

All those are represented by `irr::video::S3DVertex` class or its children. Moreover, we do not
need nothing but the information on vertex' coordinates in our case, so we may use only the
base class' properties.

The code, creating trimesh collision shape is quite simple and straightforward:

```cpp
void createTrimeshShape(irr::scene::IMeshBuffer *meshBuffer, NewtonCollision *treeCollision,
                                     irr::core::vector3df scale = irr::core::vector3df(1, 1, 1)) {
    irr::core::vector3df vArray[3];

    irr::video::S3DVertex *mb_vertices = (irr::video::S3DVertex *) meshBuffer->getVertices();

    u16 *mb_indices = meshBuffer->getIndices();

    for (unsigned int j = 0; j < meshBuffer->getIndexCount(); j += 3) {
        int v1i = mb_indices[j + 0];
        int v2i = mb_indices[j + 1];
        int v3i = mb_indices[j + 2];

        vArray[0] = mb_vertices[v1i].Pos * scale.X;
        vArray[1] = mb_vertices[v2i].Pos * scale.Y;
        vArray[2] = mb_vertices[v3i].Pos * scale.Z;

        NewtonTreeCollisionAddFace(treeCollision, 3, &vArray[0].X, sizeof(irr::core::vector3df), 1);
    }
}
```

We take the edges _(indices)_, find their vertices, create a triangle - and we're done!
You may have noticed, we do not actually create the collision shape here - we take it as
an argument for our function. You will see why this is done that way in a moment.

Now it's body's turn! But we need to extend our `Entity` class with the `NewtonBody`
field so that we can seamlesly integrate it to our engine:

```cpp
class Entity {
private:
    scene::ISceneNode *mNode;
    NewtonBody *mBody;

public:
    Entity(scene::ISceneNode *node) : mNode(node), mBody(0) { }

    Entity(scene::ISceneNode *node, NewtonBody *body) : mNode(node), mBody(body) { }

    scene::ISceneNode *getSceneNode() const {
        return mNode;
    }

    NewtonBody *getBody() const {
        return mBody;
    }

    void setBody(NewtonBody *body) {
        mBody = body;
    }
};
```

And now we are ready to set our `NewtonBody`:

```cpp
void createMeshBody(const std::string name) {
    Entity *entity = entities[name];
    irr::scene::IMeshSceneNode *node = (irr::scene::IMeshSceneNode *) entity->getSceneNode();

    NewtonCollision *shape = NewtonCreateTreeCollision(newtonWorld, 0);
    NewtonTreeCollisionBeginBuild(shape);

    irr::scene::IMesh *mesh = node->getMesh();

    for (unsigned int i = 0; i < mesh->getMeshBufferCount(); i++) {
        irr::scene::IMeshBuffer *mb = mesh->getMeshBuffer(i);
        createTrimeshShape(mb, shape, node->getScale());
    }

    NewtonTreeCollisionEndBuild(shape, 1);

    float mass = 0.0f;

    dMatrix origin;
    NewtonCollisionGetMatrix(shape, &origin[0][0]);
    NewtonBody *body = NewtonCreateDynamicBody(newtonWorld, shape, &origin[0][0]);

    dVector inertia;
    NewtonConvexCollisionCalculateInertialMatrix(shape, &inertia[0], &origin[0][0]);
    NewtonBodySetMassMatrix(body, mass, mass * inertia.m_x, mass * inertia.m_y, mass * inertia.m_z);
    NewtonBodySetCentreOfMass(body, &origin[0][0]);

    NewtonDestroyCollision(shape);

    NewtonBodySetTransformCallback(body, transformCallback);
    NewtonBodySetForceAndTorqueCallback(body, applyForceAndTorqueCallback);

    NewtonBodySetUserData(body, entity);
    NewtonInvalidateCache(newtonWorld);

    entity->setBody(body);
}
```

There is an interesting piece here, though: we used recursion to create collision shape...
But if you remember, we did not create collision shape with our `createTrimeshShape` method -
all we do in that method is that we add new triangles to the existing shape. That's because meshes
in Irrlicht are stored as a tree structure - they may have sub-meshes, which are still
parts of the whole mesh. And they can have their sub-meshes too. So we created a blank
collision shape and fill it with new parts of the mesh.

Doing it that way prevents us from overcomplicating the task and building a composite mesh,
made of a set of trimeshes. That would be really hard to calculate in real-time! And our
really simple scene would work with the speed of 5x5 battle in Unreal Tournament 3...

Looking back to our list, we should now fill out all the fields for our `NewtonBody`. And since
we are making the static model, we will set its mass to zero. This is enough for Newton to
treat our body as the static one. I placed the other code to show the other fields, we need
to fill in case we have a "usual" body.

So the other fields of `NewtonBody` are:

1. `massMatrix`, which determines how the mass is spread along the body
2. `transformCallback` and `forceAndTorqueCallback` are two mandatory fields, required by Newton
3. `userData`, which will hold the pointer to the whole entity

`massMatrix` could be calculated automatically from the collision shape, like in our case.
Without digging much into details, we will simply set it so the mass of our body is distributed
uniformely.

`transformCallback` is the function, which will be called for our body each time it changes its
position due to the interaction with other bodies inside `NewtonWorld`.

`forceAndTorqueCallback` is the function, which applies forces and torques to our body. This is
a bit tricky, but you need to keep track of each force and torque by yourself and then apply
them in a way that they summ up and create the final force, influencing the body. We will
talk about it later, when we will deal with impulses.

So, the `transformCallback`:

```cpp
static void transformCallback(const NewtonBody *body, const dFloat *matrix, int threadIndex) {
    Entity *entity = (Entity *) NewtonBodyGetUserData(body);
    scene::ISceneNode *node = entity->getSceneNode();

    if (!node)
        return;

    core::matrix4 transform;
    transform.setM(matrix);

    node->setPosition(transform.getTranslation());
    node->setRotation(transform.getRotationDegrees());
}
```

Nothing tricky here.

To put everything in place, let's add a sphere to our scene. The process is totally same,
except of the collision shape creation - in case of primitives like box, sphere or cylinder,
it is much more easy than with trimeshes - you do not need to loop through any indices or
vertices - just set shape params like dimensions or radius. And body creation process is
totally same.

```cpp
void createSphereNode(const std::string name, const std::string textureFile) {
    scene::ISceneNode *node = smgr->addSphereSceneNode();

    if (node) {
        node->setMaterialTexture(0, driver->getTexture(textureFile.c_str()));
        node->setMaterialFlag(video::EMF_LIGHTING, false);
    }

    entities[name] = new Entity(node);
}

NewtonCollision *createSphereCollisionShape(scene::ISceneNode *node, float radius) {
    dQuaternion q(node->getRotation().X, node->getRotation().Y, node->getRotation().Z, 1.f);
    dVector v(node->getPosition().X, node->getPosition().Y, node->getPosition().Z);
    dMatrix origin(q, v);

    int shapeId = 0;

    return NewtonCreateSphere(newtonWorld, radius, shapeId, &origin[0][0]);
}

void createSphereBody(const std::string name, float radius, float mass) {
    Entity *entity = entities[name];
    scene::ISceneNode *node = entity->getSceneNode();

    dQuaternion q(node->getRotation().X, node->getRotation().Y, node->getRotation().Z, 1.f);
    dVector v(node->getPosition().X, node->getPosition().Y, node->getPosition().Z);
    dMatrix origin(q, v);

    int shapeId = 0;

    NewtonCollision *shape = NewtonCreateSphere(newtonWorld, radius, shapeId, &origin[0][0]);

    dMatrix origin;
    NewtonCollisionGetMatrix(shape, &origin[0][0]);
    NewtonBody *body = NewtonCreateDynamicBody(newtonWorld, shape, &origin[0][0]);

    dVector inertia;
    NewtonConvexCollisionCalculateInertialMatrix(shape, &inertia[0], &origin[0][0]);
    NewtonBodySetMassMatrix(body, mass, mass * inertia.m_x, mass * inertia.m_y, mass * inertia.m_z);
    NewtonBodySetCentreOfMass(body, &origin[0][0]);

    NewtonDestroyCollision(shape);

    NewtonBodySetTransformCallback(body, transformCallback);
    NewtonBodySetForceAndTorqueCallback(body, applyForceAndTorqueCallback);

    NewtonBodySetUserData(body, entity);
    NewtonInvalidateCache(newtonWorld);

    entity->setBody(body);
}
```

Have no fear about code duplication - we will remove it later. When you are done, you should
get picture like this one:

<img src="{{ site.baseurl }}/images/collision_shapes/ramp_with_ball.png" alt="First completed dynamic scene" class="img-responsive">

Congrats! That's our first completed dynamic scene!