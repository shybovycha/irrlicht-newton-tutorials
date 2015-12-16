---
layout: post
order: 10
date: '2015-08-28T18:03:00+01:00'
timestamp: '2015-11-15T21:50:00+01:00'
---

# Perfect application architecture

Positive thinking is a good-to-have when starting something =)

## Basic rules

Let's talk a bit about our application before we create it - let's make some *planning*! We will think of our
application at least as of well-designed one. So what is the **application architecture**? Not just a game, but
any application? It's the way developer split its parts, divided classes, modules, variables, entities and other
pieces of application, packing them into separate units and defining how they will interact. So that any new
improvement or change to the application does not make one feel sick at the task definition stage.

But how about a game? What the good architecture of a game looks like? Do we need to separate one 3D models from
others? Or does that means writing each quest in a separate `*.cpp` file? No-no-no. That means we should create
such an application, so that any new quest, model or even a text change will not require us to re-compile the
damn bunch of code for hours or look for a places, where that particular model or text is used and changing
it everywhere in the source code.

I assume our game to have a stable, rarely changed **core**, a set of **assets** _(models, textures, sounds - any
content, made by artists and used to be presented to the player)_ and a bunch of **scripts**, defining all the
logic of a game - how character looks like, how the menus are shown and how they react to player's actions, how
objects in the game world behave and how that world looks like and all the stuff. The main criteria here are:

1. scripts and assets may be changed at any time
2. scripts and assets define the whole game
3. none of the changes to scripts or assets force us to re-compile game core

Basically, we can make the core so flexible, we may use it in any game. Or we may want to create a GUI editor
to create scripts in more handy way. But that requires much more work. And there is never a limit to make
something better. So currently we will not advance that much.

## The tools

We will use **Irrlicht** engine because of its simplicity. And it satisfies all our needs - it
does not need much content preparation; it provides GUI; extending it with **IrrKlang** will give
us very simple interface to sound and music playback.

**Newton Game Dynamics** engine we will use to physics simulate. It is easy to use and sooo powerful -
you would be impressed!

The last, not the least, we will use **Lua** scripting language to write scripts. Lua is a lightweight
programming language and perfectly suits that goal.

One of the most beautiful parts of this tutorial, will be the part on making of **assets**. We will use
**Blender 3D** to create a couple of 3D models.

I also found **CMake** kind of user-friendly. It is not that handy as any of dependency managers for all
those languages, supporting them _(`npm` for JavaScript, `go get` for Go, `RubyGems` for Ruby, `leiningen`
for Clojure and many others)_. Yet it makes your project to be a little more portable, helps to handle your
dependencies, totally eliminates the need of all those *How to configure VisualStudio for OGRE* tutorials.
Just try it!

## Conclusion

Remember all the three rules for our architecture. And keeping them in mind, let's get to some coding already!