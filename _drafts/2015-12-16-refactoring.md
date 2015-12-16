---
layout: post
title: Refactoring
categories: []
tags: []
published: True
order: 70
---

# Refactoring

Currently we have one `.cpp` file, containing all the application core and consisting of...
Nearly 800 lines of code! That's way too much for one file! We definitely need to do
something with that.

First thing, which comes to mind is to split all the code into classes and extract each
class to a separate file. But that's not all. We may go further and refactor the architecture
itself. For example, we can create one class, `Application`, which will interact with
`ScriptManager` and `PhysicsManager` in a way, where one may not know about another. Like this:

{% highlight cpp %}
class Application {
private:
    IrrlichtDevice *device;
    ScriptManager *scriptMgr;
    PhysicsManager *physicsMgr;
    SceneManager *sceneMgr;

protected:
    virtual void handleFrame(float deltaTime) {
        physicsMgr->update(deltaTime);
    }

    virtual void initialize() {
        physicsMgr->initialize();
    }

    virtual void cleanup() {
        physicsMgr->cleanup();
        scriptMgr->cleanup();
    }

    virtual void beforeFrame() {};
    virtual void afterFrame() {};

public:
    Application(...) { ... }

    void run() {
        initialize();

        while (device->run()) {
            beforeFrame();

            handleFrame();

            afterFrame();
        }

        cleanup();
    }
};
{% endhighlight %}

So that we coul derive our own class, optionally override some of the methods like this:

{% highlight cpp %}
class MyApplication : public Application {
protected:
    void initialize() {
        Application::initialize();

        scriptMgr->load("media/scripts/main.lua");
    }

    void handleFrame(float deltaTime) {
        Application::handleFrame(deltaTime);

        scriptMgr->callFunction("handleFrame");
    }
public:
    MyApplication(...) : Application(...) {}
}
{% endhighlight %}

And use our instance in the `main()` function:

{% highlight cpp %}
int main() {
    MyApplication *app = new MyApplication(...);

    app->run();

    return 0;
}
{% endhighlight %}

Simple, as never before, isn't it?

But creating such an architecture requires us to answer a few tricky questions. For example,
_How should we expose PhysicsManager methods to our scripts?_. We can solve this by, for example,
extending the `PhysicsManager` class with one method, which will take `ScriptManager` instance
and expose one's method to it.
