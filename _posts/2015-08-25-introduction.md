---
layout: post
title: Introduction
date: '2015-08-25T18:01:00+01:00'
---

## What will you learn?

This tutorial covers the development of a game from a very beginning. This includes:

* planning the **architecture** for a game
* developing the **game engine**
* defining game logic with **scripts**
* creating **3d models**
* and **distributing the application**

And of course, the whole tutorial is built around two libraries - **Irrlicht** and **Newton Game Dynamics**.

As for *{{ site.time | date: '%d %b %Y' }}*, the tutorial covers:

* an introduction to NewtonGD
* creating Irrlicht-powered application
* controlling the application logic with Lua scripts
* building application with CMake

## Why writing everything from scratch? Why not just use Unity / Unreal Engine / *you name it*?

The main reason why you might be interested in this tutorial, especially due to the fact that there are some well-known and
actively discussed/developed/used game engines like **Unreal Engine 4**, **Unity 3D** and others, is:
you want to know how things work and thus get most flexibility of your tools (or even start working towards building your own).

## What should I know to proceed?

You are required to have at least some experience with **C++**, basic knowledge of **graphics** and **game development**
*(just to make sure, you will not call 3d models "textures" or miss the "script" word' meaning =) )*.

## Why these libraries?

Irrlicht is easy to use and contains all the features you will need to build a game - resource management, support for majority of
asset formats out-of-the-box, user input *(keyboard, mouse and joystick events handling)*, GUI
*(Graphical User Interface - buttons, text inputs, etc.)*.

Newton Game Dynamics is also very easy to use; it is constantly developed; and it is darn powerful!

If you are still interested - then please proceed to
<a href="{{ site.baseurl }}{% post_url 2015-08-26-application-architecture %}" class="btn btn-success">the first chapter</a>.
