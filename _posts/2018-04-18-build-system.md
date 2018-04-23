---
layout: post
title: Build system
date: '2018-04-18T20:11:00+10:00'
order: 10
---

## Why do we need a build system?

### Travel in time and space

Let me show you a real-world example: many IDEs (like Code::Blocks and VisualStudio) let you to set project-wide or system-wide settings like paths to header and library files of different libraries. Earlier on we used to configure those to build all our solutions. And here comes the challenge: code you write today might be picked up by a different person (even The Future You), who might want to compile and run it on his hardware and OS. That person might not have the IDE and OS you've tied your solution to. So compiling and running the code will take a while just because of complex environment configuration process.

### Dependency management

Most likely you will want to use a few third-party libraries in your code when developing an application, because - why inventing a wheel? This is a very common and generally good practice, which allows you to reduce the time invested in development and possibly the amount of bugs you could've introduced when creating a solution 100% from scratch. The problem here is that a library your program depends on (hence a "dependency") may rely on other library itself. And the more complex library you use the wider dependency tree is.

<img src="{{ site.baseurl }}/images/dependency_tree.jpg" alt="Dependency tree">

Long time ago the way we managed those dependencies was a nightmare, given the previous paragraph showing how we've built the code. Imagine building a complex solution with tons of dependencies and configuring your IDE for each of them. Hard? Now think of how hard it was to configure it in order to build everything on a different machine and OS.

> why it is shit in a world of C++, how build process looks like on different platforms and what are the challenges of multi-platform code. Why Bazel, how it compares to CMake and Buck, what challenges it has (like passing --apple_platform on OSX and lack of precompiled_header rules)

### Multi-platform code challenges

## Choosing the right build tool

There are three major types of build systems for C++:

1. **generators**, which do not build the code, but just generate the configuration files for other build tools
2. **build tools**, which run the commands with the options configured in the configuration file
3. **build systems**, which actually manage the dependencies and build commands / options

In a C++ world, if you want to build a cross-platform code, you'll probably stick to the most known CMake generator. If your target is a POSIX system (OSX / Linux / BSD) - then you'll probably be just fine with Make. But there are two somewhat least known build systems, which are capable of building the application for all the platforms - Buck from Facebook and Bazel from Google.

### Make

### Cmake

The common mistake vast majority of C++ developers still repeat when using CMake is using the old version of CMake. That does not mean they download the some-years-old version from the internet and run it on Windows 98. The root of this problem is somewhat subtle: CMake requires you to specify the minimal CMake version your project can be built with. And developers, following habits, tutorials, examples or whatever tend to specify version `2.6` or `2.8`, which were released between 2008 and 2013. As for 2018, the most recent version is `3.11`.

The problem with old version as the minimum required one is that you are limited by an old feature set and an old syntax, which is basically everything for CMake.

The worst part of it is that if you go to the CMake website and navigate to the ["tutorial" section](https://cmake.org/cmake-tutorial/), you will see that it guides you to use the `2.6` version.

This mistake was well discussed on both [BoostCon 2017](https://www.youtube.com/watch?v=bsXLMQ6WgIk) and [CppCon 2017](https://youtu.be/eC9-iRN2b04?t=299)

Feel the difference

### Buck

### Bazel
