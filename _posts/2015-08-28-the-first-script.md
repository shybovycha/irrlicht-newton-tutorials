---
layout: post
order: 30
date: '2015-08-28T18:10+01:00'
timestamp: '2015-11-15T23:17+01:00'
---

# The first script

As we discussed, we will describe the whole game in scripts, and the core functionality
we will define in the core. In this chapter we will be adding **Lua** to our application.
You do not need to download Lua itself - you'd better install it with your system's
package manager _(`yum` or `apt` or whatever your Linux uses, `brew` for OSX...)_.

## Dependencies

The only thing you need to download from Internet this time is Lua wrapper called
**luacppinterface**.
So [**go and get it**](https://github.com/davidsiaw/luacppinterface/archive/master.zip) from
Github.

And unpack it... right to the `source` directory of our project! That's right! That's
really small library so it will not pollute your project with tons of files.

Now, I mentioned dependency managers earlier. This is how _we_ will handle them in our _C++_
application - we will simply put the sources of all the libraries we depend on, with the
versions we depend on, right in our project. Given that, you may put Irrlicht there as well -
you are free to do anything with our project!

## Build instructions

To build our project we will need to change our `CMakeLists.txt` file to fetch
our new dependency:

{% highlight cmake %}
cmake_minimum_required(VERSION 3.1)
project(irrlicht_newton_game1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES source/main.cpp)
set(EXECUTABLE_NAME irrlicht_newton_game1)

set(LUACPPINTERFACE_PATH source/luacppinterface-master)

add_subdirectory(${LUACPPINTERFACE_PATH})

find_package(X11)
find_package(OpenGL)
find_package(ZLIB)
find_package(Lua)

if (NOT IRRLICHT_LIBRARY_PATH)
    find_library(IRRLICHT_LIBRARY_PATH
            NAMES Irrlicht
            PATHS ${IRRLICHT_PATH}/lib/
            PATH_SUFFIXES Linux MacOSX Win32-gcc Win32-visualstudio Win64-visualstudio)

    message(STATUS "Found Irrlicht: ${IRRLICHT_LIBRARY_PATH}")
endif()

include_directories(${IRRLICHT_PATH}/include ${LUA_INCLUDE_DIR} ${LUACPPINTERFACE_PATH}/include)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME}
        luacppinterface
        ${IRRLICHT_LIBRARY_PATH}
        ${X11_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${X11_Xxf86vm_LIB}
        ${LUA_LIBRARIES})
{% endhighlight %}

And here's the thing: if you try to compile our project on another machine, you will
not need to install any other libraries than Lua on that machine! That supposed to sound
like _"sweet, huh?"_, except that one little _"but..."_... Bittersweet...

Back to our busines... `luacppinterface` needs to be tweaked a bit to fit our project -
we will hack its `CMakeLists.txt` file to make it depend on system Lua libraries.
Just make it look like this:

{% highlight cmake %}
cmake_minimum_required (VERSION 2.6)

project(luacppinterface)

include_directories("lua/src")

find_package(Lua)

include_directories(${LUA_INCLUDE_DIR})

add_library(luacppinterface STATIC
    include/luacoroutine.cpp
    include/luareference.cpp
    include/luacppinterface.cpp
    include/luatable.cpp
    include/luafunction.cpp
)

target_link_libraries(luacppinterface ${LUA_LIBRARIES})
{% endhighlight %}

It barely differs from the original file, but it makes a compilation pleasant - you
do not need to specify paths to Lua libs anymore!

## Injecting some Lua

Our application now uses C++ code to place some 3D objects in a scene. Let's move,
say, sphere creation, to the script.

First of all, add `luacppinterface` headers to our `main.cpp` file:

{% highlight cpp %}
#include "luacppinterface-master/include/luacppinterface.h"
{% endhighlight %}

Now let's look at some of Irrlicht' conventions:

* it uses `irr::video::IVideoDriver` for rendering operations
* it uses `irr::scene::ISceneManager` for scene management

So why not to define a `ScriptManager` to handle scripts? Our requirements
for this class (for now) are:

* it should load and evaluate scripts
* it should provide simple API to our scripts

Let's get coding!

{% highlight cpp %}
class ScriptManager {
private:
    Lua luaState;
    std::map<std::string, scene::ISceneNode*> nodes;
    video::IVideoDriver *driver;
    scene::ISceneManager *smgr;

    void bindFunctions();

public:
    ScriptManager(scene::ISceneManager *_smgr, video::IVideoDriver *_driver) {
        driver = _driver;
        smgr = _smgr;
    }

    void createSphereNode(const std::string name, const std::string textureFile);

    void setNodePosition(const std::string name, LuaTable pos);

    LuaTable getNodePosition(const std::string name);

    void loadScript(const std::string filename) {
        std::ifstream inf(filename);
        std::string code((std::istreambuf_iterator<char>(inf)), std::istreambuf_iterator<char>());

        bindFunctions();

        luaState.RunScript(code);
    }
};
{% endhighlight %}

This is just a skeleton - we will fill it out in a minute. Just catching up:

1. this class depends on `IVideoDriver` and `ISceneManager` to handle 3D objects and the scene
2. it contains `Lua luaState` field to store the current state of our script running
3. it stores all the nodes as a `<string, ISceneNode*>` map to allow access to our nodes from scripts
4. it exposes three methods as an API to Lua scripts: `createSphereNode`, `setNodePosition` and
  `getNodePosition` so we will be able to make some manipulations in our scripts
5. it provides really short and simple interface to our C++ core: `ScriptManager(...)` and `loadScript`

The main principle, each and every programmer breaks every day is **KISS** _(Keep It Stupidly Simple)_.
And that principle should guide us through this whole tutorial to not overthink and override
ourselves as well as the project we are making. That is why our APIs are that simple.

But let's get back to our `ScriptManager`. It shows how things will look like, but never
defines how they will actually **work**. So here are the key points to Lua API:

1. `LuaTable` is an array-like structure in Lua, representing both indexed as well as key-value
  arrays in Lua. This type is a way to pass variables between Lua script and C++ program. You
  may use both `table.Get<value_type>(index)` and `table.Get<value_type>("key")` methods
  to access its values.

2. To bind our `ScriptManager` methods to Lua functions, we need to use pointers to those
  functions. And as it is not that simple in usual C++, we will use C++11x lambdas:

  {% highlight cpp %}
  auto createSphere = luaState.CreateFunction<void(std::string, std::string)>([&](std::string name, std::string tex) -> void { createSphereNode(name, tex); });
  {% endhighlight %}

3. All the functions and variables you want to pass to Lua scripts should be global. And since
  we have our pretty `luaState` member, we may set global members through its methods:

  {% highlight cpp %}
  LuaTable global = luaState.GetGlobalEnvironment();

  // ...

  global.Set("createSphere", createSphere);
  {% endhighlight %}

4. We will be using just a map of a Irrlicht' nodes and its name to bypass those nodes between
  scripts and core:

  {% highlight cpp %}
  void createSphereNode(const std::string name, const std::string textureFile) {
	  scene::ISceneNode *node = smgr->addSphereSceneNode();

	  if (node) {
  	      node->setPosition(core::vector3df(0, 0, 30));
	      node->setMaterialTexture(0, driver->getTexture(textureFile.c_str()));
	      node->setMaterialFlag(video::EMF_LIGHTING, false);
	  }

	  nodes[name] = node;
  }

  void setNodePosition(const std::string name, LuaTable pos) {
        float x, y, z;

        x = pos.Get<float>("x");
        y = pos.Get<float>("y");
        z = pos.Get<float>("z");

        nodes[name]->setPosition(core::vector3df(x, y, z));
    }

    LuaTable getNodePosition(const std::string name) {
        LuaTable pos = luaState.CreateTable();

        core::vector3df v = nodes[name]->getPosition();

        pos.Set("x", v.X);
        pos.Set("y", v.Y);
        pos.Set("z", v.Z);

        return pos;
    }
  {% endhighlight %}

Given those, we have our API and are able to create and run our first Lua script.
Add one in the `media/scripts/` directory:

{% highlight lua %}
createSphere("sphere1", "media/textures/wall.bmp")
{% endhighlight %}

**Note:** paths in the script will be used by C++ core, relatively to the binary file, which
is... generated by our C++ code! So all the paths in the scripts are just the same as they
are in C++ core.

And add the `ScriptManager` initialization code:

{% highlight cpp %}
ScriptManager *scriptMgr = new ScriptManager(smgr, driver);

scriptMgr->loadScript("media/scripts/test1.lua");
{% endhighlight %}

Now you may remove the code, creating sphere in the `main()` function. And run the code.
You should see exactly the same picture as before:

![_screenshot #1_]({{ site.baseurl }}/images/04_movement_untouched.png)

## Homework

Your task is: try to move all the other "factory" functions _(creating cube, ninja,
circle animator for cube and fly animator for Ninja)_ to Lua script, adding API for them
to `ScriptManager`.

## More separation

We will now advance our script and add some convention to it. These will be our tasks
for the rest of this chapter:

1. move keyboard events handling to script
2. create two function in script so we may call them _by convention, not by configuration_

The last phrase I took from **Ember.js introduction**. It says _"prefer convention over
configuration"_, meaning we'd better call the functions of same name on different scripts,
instead of setting somehow which function to call.

That is, we will define `handleFrame()` function in our script, which will be called
on each `onFrame` event in our C++ core and the `main()` function, which will be called right
after script has been loaded.

{% highlight cpp %}
auto handler = luaState.GetGlobalEnvironment().Get<LuaFunction<void(void)>>("handleFrame");

// ...

handler.Invoke();
{% endhighlight %}

Moreover, we will define a global keyboard state table for each of scripts we load and will
be updating it as user presses keys on his keyboard. And this variable will be shared with
script, but as read-only one. So changes in that table will have no effect on the application
itself.

{% highlight cpp %}
class ScriptManager {
private:
    std::map<int, bool> keyStates;

public:
    void setGlobalVariables() {
        setKeyStates();
    }

    void setKeyStates() {
        LuaTable keysTable = luaState.CreateTable();

        for (auto &kv : keyStates) {
            keysTable.Set(kv.first, kv.second);
        }

        luaState.GetGlobalEnvironment().Set("KEY_STATE", keysTable);
    }

    void setKeyState(int key, bool state) {
        keyStates[key] = state;
    }

    void handleFrame() {
        auto handler = luaState.GetGlobalEnvironment().Get<LuaFunction<void(void)>>("handleFrame");

        setKeyStates();

        handler.Invoke();
    }

    void loadScript(const std::string filename) {
        std::ifstream inf(filename);
        std::string code((std::istreambuf_iterator<char>(inf)), std::istreambuf_iterator<char>());

        bindFunctions();
        setGlobalVariables();

        luaState.RunScript(code);

        auto scriptMainFn = luaState.GetGlobalEnvironment().Get<LuaFunction<void(void)>>("main");
        scriptMainFn.Invoke();
    }
};

class MyEventReceiver : public IEventReceiver {
public:
    MyEventReceiver(ScriptManager *scriptManager) {
        scriptMgr = scriptManager;

        for (u32 i = 0; i < KEY_KEY_CODES_COUNT; ++i)
            scriptMgr->setKeyState(i, false);
    }

    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent &event) {
        // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
            scriptMgr->setKeyState(event.KeyInput.Key, event.KeyInput.PressedDown);

        return false;
    }

private:
    ScriptManager *scriptMgr;
};
{% endhighlight %}

Variables are added to a `GlobalEnvironment` just as function do:

{% highlight cpp %}
luaState.GetGlobalEnvironment().Set("KEY_STATE", keysTable);
{% endhighlight %}

Lua-defined functions are found by their names and called with `Invoke(args)` method:

{% highlight cpp %}
auto handler = luaState.GetGlobalEnvironment().Get<LuaFunction<void(void)>>("handleFrame");

handler.Invoke();
{% endhighlight %}

Let's add some simple interaction to our script now. I'll help you a bit:

{% highlight cpp %}
void moveNode(const std::string name, LuaTable pos) {
    scene::ISceneNode *node = findNode(name);
    core::vector3df vec = tableToVector3df(pos);

    core::matrix4 m;

    core::vector3df rot = node->getRotation();
    m.setRotationDegrees(rot);

    m.transformVect(vec);
    node->setPosition(node->getPosition() + vec);
    node->updateAbsolutePosition();
}
{% endhighlight %}

This is how nodes could be moved relatively to their current position in Irrlicht.

And here's how our Lua script may look like now:

{% highlight lua %}
function handleFrame()
    -- w
    if KEY_STATE[0x57] == true then
        move("sphere1", { x = 0, y = 1, z = 0 })
    end

    -- s
    if KEY_STATE[0x53] == true then
        move("sphere1", { x = 0, y = -1, z = 0 })
    end
end

function main()
    createSphere("sphere1", "media/textures/wall.bmp")
    setPosition("sphere1", { x = 0, y = 0, z = 30 })

    createCube("cube1", "media/textures/t351sml.jpg")
    addCircleAnimator("cube1", { x = 0, y = 0, z = 30 }, 20.0)

    createAnimatedMesh("ninja", "media/models/ninja.b3d", "media/textures/nskinbl.jpg", 0, 13, 15)
    setRotation("ninja", { x = 0, y = -90, z = 0 })
    setScale("ninja", { x = 2, y = 2, z = 2 })
    addForwardAnimator("ninja", { x = 100, y = 0, z = 60 }, { x = -100, y = 0, z = 60 }, 3500, true)
end
{% endhighlight %}

If you run our application *now*, you should be able to control sphere with <kbd>w</kbd> and
<kbd>s</kbd> keys:

![_screenshot #2_]({{ site.baseurl }}/images/lua_script_with_kbd_handling.png)