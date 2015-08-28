---
layout: post
date: '2015-08-28T18:02:00+01:00'
---

# Irrlicht basics

Before we start doing some game design or gamedev magick, we'd better know something about our rendering engine. Just very basics - how to create an application, which will react to **user input** and is able to load and draw some **content**.

If you think you already know everything you want about Irrlicht (or any other rendering engine you shall use - it's simple, though!) - just skip this section.

## Install Irrlicht

This section depends on the operating system you are using. But no matter which one you use, first of all - [**go and get the latest version of Irrlicht!**](http://irrlicht.sourceforge.net/downloads/).

### Linux

To get started under Linux, you should compile Irrlicht. Unfortunately, it is bundled with already compiled libraries for Windows, but not for Linux.

But don't be scared! It's simple! Yet, there is one caveat: **do not run `make install` on Linux! Ever!**. If you do, it will be hard for you to get rid of an old version of the software you installed in such a way. For example, when you'll make an update of that software.

To bypass this, you'd better use `checkinstall` utilitie. It creates Debian package from sources and installs it natively instead of just copying files to their places, keeping no trace of that.

So, unzip the `irrlicht-*.*.*.zip`, go to the `irrlicht-*.*.*/sources/Irrlicht` and run `make`. And, after many output, if you do not see any error messages - congratulations! You've compiled Irrlicht!

Now, and that's important, here's how you shall compile your Irrlicht applications:

{% highlight bash %}
    g++ <source file> -o <output executable file> -L<absolute path to Irrlicht lib/Linux dir> -I<absolute path to Irrlicht include dir> -lIrrlicht -lGL
{% endhighlight %}

For example, if your program is stored at `main.cpp` file and you've compiled Irrlicht at `/home/moo_user/irrlicht-1.8/` directory, your compiler command-line will look like this:

{% highlight bash %}
    g++ main.cpp -o application -L/home/moo_user/irrlicht-1.8/lib/Linux -I/home/moo_user/irrlicht-1.8/include/ -lIrrlicht -lGL
{% endhighlight %}

**Note:** there is no space between `-L` or `-I` and the path! As well, as between `-l` and the library name! That is **GNU C++** compiler command-line options format.

_Add some screenshots and QtCreator configuration example_

### Windows

If you are using Windows, it's better for you to use **Visual Studio**. Personally I prefer **MS VS 2008 Express** because of its user interface peculiarities.

So, the main idea here is to set the project you are using to write the code within to use the Irrlicht **include** and **library** directories.

And here is why I prefer **MS VS 2008** - that was the last Visual Studio version where all the libraries could be specified for all the projects, in one place. So you do not need to configure each solution and each project separately, from scratch.

So, you just set the **include directories** to include the `<Irrlicht dir>/include` directory and **library directories** to include the `<Irrlicht dir>/lib/Win**-visualStudio` directory (depending on your OS architecture version it may vary between `Win32-...` and `Win64-...`).

_Add some screenshots_

### MacOS X

_Work In Progress_
