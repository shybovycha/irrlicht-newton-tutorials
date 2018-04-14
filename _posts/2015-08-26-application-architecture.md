---
layout: post
title: Application architecture
date: '2015-08-26T21:50:00+01:00'
---

## Basic terms

Let's talk a bit about our application before we create it. In order to make the development process sequential
and least painful, we need to design the application well. The design of an application or the **application architecture**
is the hardest thing to change on later stages of development. Thus it must be well-thought at the very beginning to
prevent suffering in the future.

Well, there are number of application architecture' levels:

<div class="row">
    <div class="col">
        <img src="{{ site.baseurl }}/images/japan_feudal_system.jpg" />
    </div>
    <div class="col">
        <p>
            The highest level defines which modules will the whole
            application consist of and what functionality will each of those modules have.
        </p>

        <p>
            The next level is how the modules communicate to each other, how they work together.
        </p>

        <p>
            The lower level is the structure of each module - what classes, entities, data structures and similar things will
            the module consist of.
        </p>

        <p>
            One of the lowest, yet still very important architecture levels is how files are organized.
        </p>
    </div>
</div>

From the highest architecture layer point of view, I can advice a very simple architecture:
assume our game will have a stable, rarely changed **core**,
a set of **assets** _(models, textures, sounds - any content, made by artists and used to be presented to the player)_
and a bunch of **scripts**, defining all the logic of a game - how character looks like, how the menus are shown and how
they react to player's actions, how objects in the game world behave and how that world looks like and all the stuff.

The main benefits of such an approach are:

1. scripts and assets may be changed at any time
2. scripts and assets define what we show to the user and how the application behaves so that none of the changes to scripts or assets make us to re-compile the core
3. we can modify the core and thus change how game works internally (mainly for optimization purposes) without changing the overall application functionality and behaviour

We can make the core so flexible that we may re-use it in the future projects.

## The tools

We will use **Irrlicht** engine because of its simplicity. And it satisfies all our needs - it
does not need much content preparation; it provides GUI; extending it with **IrrKlang** will give
us very simple interface to sound and music playback.

**Newton Game Dynamics** engine we will use to simulate physics. It is easy to use and is really powerful -
you would be impressed!

The last, not the least, we will use **Lua** scripting language to write scripts. Lua is a lightweight
programming language and perfectly suits that goal.

One of the most beautiful parts of this tutorial, will be the part on making of **assets**. We will use
**Blender 3D** to create a couple of 3D models.

I also found **CMake** kind of user-friendly. It is not that handy as any of dependency managers for all
those languages, supporting them _(`npm` for JavaScript, `go get` for Go, `RubyGems` for Ruby, `leiningen`
for Clojure and many others)_. Yet it makes your project a little more portable, helps to handle your
dependencies, totally eliminates the need of all those *How to configure VisualStudio for OGRE* tutorials.
Just try it!

## Conclusion

Remember all the three rules for our architecture. And keeping them in mind, let's get to some coding already!

<a href="{{ site.baseurl }}{% post_url 2015-08-27-first-application %}" class="btn btn-success">Next chapter</a>
