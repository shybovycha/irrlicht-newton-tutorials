---
layout: post
date: '2015-08-28T18:03:00+01:00'
---

# Perfect application architecture

No, really! You'd better believe your game is perfectly designed at content as well as inside! That is the first step to succed, believe me.

We shall use **Irrlicht** because of its simpleness. But the main criteria - it satisfied all our needs - it does not need much content preparation; it contains UI features inside; extending it with **IrrKlang** will provide us with very simply interface to sound and music.

As the title promotes, we shall also use **Newton GD** physics engine. It is easy to use and so powerful - you would be impressed!

But since we want **perfect** architecture - we shall use **Lua** as well. It is a very _light_ programming language. It will be used for scripting. And here's why: as our game shall grow, it will be more and more hard to modify something.

For example, if we shall start with complete game, being run at production and success and we will decide to modify some core feature. Let's say, we want to modify our _NPCs_. We want them fly. If we have everything set at the code side, we shall end up with messy architecture, almost unreadable and totally immutable code. Which should be compiled, although. And each update for our game shall take much megabytes for users to download.

On the other hand, if we have our game running with scripts, we shall only modify a few scripts, which describe how characters behave. Perhaps, we will add some. But this update shall use a few kilobytes and users will not even notice the upgrade process. Yet, the updated feature will be noted during gameplay!

But have no fear: we shall do everything step-by-step and break our tutorial into two parts - **the basics** and **the intermediate part**.
