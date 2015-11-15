---
layout: post
order: 40
date: '2015-08-28T18:06:00+01:00'
timestamp: '2015-11-15T22:45:00+01:00'
---

# Adding some Newtonianity

At this point we have an application with

* **1x Ninja**, walking around
* **1x sphere**, hanging in the center of the screen
* **1x cube**, flying around the sphere

That's our "game"? Doubtely... So let's make things move like in real world! Or just like that...

## Requirements

First of all, [**go and get**](https://github.com/MADEAPPS/newton-dynamics/archive/master.zip) the
Newton GD files. And unpack it... right to the `source` directory of our project! That's right!
I'm not insane and I'm aware you are going to put **a lot** of files in your project. But have no
fear - you may always add them to `.gitignore` and skip them from being tracked in your Git repo:

    source/newton-dynamics-master/applications
    source/newton-dynamics-master/packages/projects
    source/newton-dynamics-master/packages/thirdParty
    source/newton-dynamics-master/coreLibrary_300/projects

You are using Git, right?.. Now, you place the Newton GD sources in your project directory and change
your `CMakeLists.txt` file to look like this:

{% highlight cmake %}
cmake_minimum_required(VERSION 3.1)
project(irrlicht_newton_game1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

option("NEWTON_DEMOS_SANDBOX" "Build demos sandbox" OFF)

set(NEWTONGD_PATH source/newton-dynamics-master)
set(NEWTONGD_INCLUDE_DIRS
        ${NEWTONGD_PATH}/packages/dCustomJoints
        ${NEWTONGD_PATH}/packages/dContainers
        ${NEWTONGD_PATH}/packages/dMath
        )

set(NEWTON_LIBRARIES Newton dMath)

add_subdirectory(${NEWTONGD_PATH})

find_package(X11)
find_package(OpenGL)
find_package(ZLIB)

if (NOT IRRLICHT_LIBRARY_PATH)
    find_library(IRRLICHT_LIBRARY_PATH
            NAMES Irrlicht
            PATHS ${IRRLICHT_PATH}/lib/
            PATH_SUFFIXES Linux MacOSX Win32-gcc Win32-visualstudio Win64-visualstudio)

    message(STATUS "Found Irrlicht: ${IRRLICHT_LIBRARY_PATH}")
endif()

include_directories(${IRRLICHT_PATH}/include ${NEWTONGD_INCLUDE_DIRS})

set(SOURCE_FILES source/main.cpp)
set(EXECUTABLE_NAME irrlicht_newton_game1)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME}
        ${NEWTON_LIBRARIES}
        ${IRRLICHT_LIBRARY_PATH}
        ${X11_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${X11_Xxf86vm_LIB})
{% endhighlight %}

Try to compile your project - it should be just fine. And observe the power of CMake!

## Gravity

Let's start modifying our Irrlicht sample application. First of all, we will add some Newton headers:

{% highlight cpp %}
#include "newton-dynamics-master/coreLibrary_300/source/newton/Newton.h"
#include "newton-dynamics-master/packages/dMath/dVector.h"
#include "newton-dynamics-master/packages/dMath/dMatrix.h"
#include "newton-dynamics-master/packages/dMath/dQuaternion.h"
{% endhighlight %}

The basic thing in the whole Newton GD library is `NewtonWorld`. That is what it means - the world, where
all the physics happen. It is something different from where we place our 3D models. And that should be
obvious - graphics are managed by Irrlicht and physics - by Newton. Those are totally different libraries.
So we need to tie those two so that graphics correspond to what happens in _physical_ world.

First of all, we need to have a variable for our `NewtonWorld`. And since physics are handled by scripts too,
we need to have that variable close to our other objects - in the `ScriptManager` class.

There are two functions we need to bind to our `NewtonBody`:

{% highlight cpp %}
static void transformCallback(const NewtonBody* body, const dFloat* matrix, int threadIndex)
{
    Entity *entity = (Entity*) NewtonBodyGetUserData(body);
    scene::ISceneNode* node = entity->getSceneNode();

    if (node)
    {
        core::matrix4 transform;
        transform.setM(matrix);

        node->setPosition(transform.getTranslation());
        node->setRotation(transform.getRotationDegrees());
    }
}

static void applyForceAndTorqueCallback(const NewtonBody* body, dFloat timestep, int threadIndex)
{
    dFloat Ixx, Iyy, Izz;
    dFloat mass;

    NewtonBodyGetMassMatrix(body, &mass, &Ixx, &Iyy, &Izz);

    dVector gravityForce(0.0f, mass * -9.8f, 0.0f, 1.0f);
    NewtonBodySetForce(body, &gravityForce[0]);
}
{% endhighlight %}


<p><strong>Hint:</strong> to make simulation slower and so watch ball falling in detail, make the <i>NewtonUpdate</i> FPS argument even smaller. A thousand times, say.</p>
<p><strong>Note:</strong> I used the home directory for user named <em>user</em> to store Newton SDK and Irrlicht SDK. You may want to change this!</p>
<p>If you did everything right, you shall see the metal ball falling right through the camera and down... down... down...</p>
