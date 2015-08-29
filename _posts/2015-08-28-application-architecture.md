---
layout: post
date: '2015-08-28T18:03:00+01:00'
---

# Perfect application architecture

No, really! You'd better believe your game is perfectly designed at content as well as inside! That is the first step to succeed, believe me.

We shall use **Irrlicht** because of its simpleness. But the main criteria - it satisfies all our needs - it does not need much content preparation; it contains UI features; extending it with **IrrKlang** will provide us with very simple interface to sound and music.

As the title of all this site says, we will use **Newton Dynamics** engine to simulate physics. It is easy to use and sooo powerful - you would be impressed!

But since we want our architecture to be **perfect**, we need to pull out some logic. We will use **Lua** scripting language to program some parts of our game. Lua is a lightweight programming language and perfectly suits our goal.

Assume you have a complete game, being successfully played by thousands of gamers. And you decide to modify some core feature. Let's say, you want to make your _NPCs_ fly. If you have everything programmed in one monolithic application, you will end up re-building the whole game for hours, when making any small change. And each update for your game will take many megabytes for users to download.

On the other hand, if you have your game running scripts, you will only modify a few scripts, which describe how characters behave. Perhaps, add some. But this update will take only a few kilobytes so users will not even notice the upgrade process. Yet, the updated feature will be noted during gameplay!

But have no fear: we shall do everything of those step-by-step and break our tutorial into two parts - **the basics** and **the intermediate part**.
