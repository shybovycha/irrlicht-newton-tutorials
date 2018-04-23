---
layout: post
title: Scripting
date: '2018-04-23T20:16:00+10:00'
order: 40
---

## Top of the iceberg

In a very broad meaning, the application should consist of modules that we can easily and, most important, independently modify them or even replace them without changing the overall application functionality. That is the core principle of any well-designed system.

There are different levels of application design:

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

For the moment being we have nothing but a very simple sample application. It makes no sense to over-engineer it by introducing complex design solutions. So let's split the application into two major layers - the API, which will define what application is capable of doing and the game logic, which will interoperate with the API and define what the game will actually do.

From such a perspective, the API is a very big set of functionality, whilst logic might use just a tiny bit of it. So packing everything into one binary file will have tons of unused stuff. Luckily, we have the support of C++ compiler, which will eliminate major unused pieces.

From the developer level, we need to choose the tools - libraries, engines and languages, which will allow us to create this flexible solution.

Of course, we can put the logic into the same binary as the API, but then all the flexibility and the design benefits will be lost.

So we'd better use the non-intrusive way of describing logic. Scripting languages are serve best that purpose. And here we have a variety of choice - from Python, through well-known Lua and up to the least known AngelScript and ChaiScript.

### Scripting languages

#### Python

Python is a powerfull language, but with great power comes great responsibility. And in terms of a scripting language for a C++ application, Python comes with a huge virtual machine and a very steep integration curve.

*Sample Python script:*

```python
def multiply(a, b):
    print "Will compute", a, "times", b
    c = 0

    for i in range(0, a):
        c = c + b

    return c
```

*Python integration:* ([Python docs on embedding](https://docs.python.org/2/extending/embedding.html), [CodeProject](https://www.codeproject.com/Articles/11805/Embedding-Python-in-C-C-Part-I))

```cpp
#include <Python.h>

int main(int argc, char *argv[]) {
    PyObject *pName, *pModule, *pDict, *pFunc;
    PyObject *pArgs, *pValue;
    int i;

    if (argc < 3) {
        fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
        return 1;
    }

    Py_Initialize();
    pName = PyString_FromString(argv[1]);
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, argv[2]);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(argc - 3);

            for (i = 0; i < argc - 3; ++i) {
                pValue = PyInt_FromLong(atoi(argv[i + 3]));

                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }

            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyInt_AsLong(pValue));
                Py_DECREF(pValue);
            } else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
        } else {
            if (PyErr_Occurred()) {
                PyErr_Print();
            }

            fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
        }

        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
        return 1;
    }

    Py_Finalize();

    return 0;
}
```

#### ChaiScript

ChaiScript is extremely easy to integrate with C++, since it is just a single header file. And it has C-like syntax.

*Sample ChaiScript script:* ([ChaiScript documentation](http://chaiscript.com/examples.html))

```c
def multiply(a, b) {
    puts(helloWorld("user"));
    puts("Will compute " + to_string(a) + " times " + to_string(b));

    var c = 0;

    for (var i = 0; i < a; i++) {
        c = c + b;
    }

    return c;
}
```

*ChaiScript integration:*

```cpp
#include <chaiscript/chaiscript.hpp>
#include <fstream>
#include <string>

std::string helloWorld(const std::string &t_name) {
  return "Hello " + t_name + "!";
}

int main() {
  chaiscript::ChaiScript chai;
  chai.add(chaiscript::fun(&helloWorld), "helloWorld");

  std::ifstream ifs("module.chai");
  std::string script(
    (std::istreambuf_iterator<char>(ifs)),
    (std::istreambuf_iterator<char>())
  );

  chai.eval(script);
}
```

#### Lua

Lua is a very simple language what means everyone can learn to use it. On the other hand, integration with C++ is somewhat more complex, since we must explicitly share the state and methods to operate on it between both Lua and C++.

*Sample Lua script:* ((LuaCppInterface documentation)[https://github.com/davidsiaw/luacppinterface])

```lua
function multiply(a, b)
  local c = 0

  print(helloWorld('user'))
  print('Will compute ', a, ' times ', b)

  for i = 0, a do
    c = c + i
  end

  return c
end
```

*Lua integration:*

```cpp
#include <string>
#include <luacppinterface.h>

int main() {
  Lua lua;
  LuaTable global = lua.GetGlobalEnvironment();

  // A function that prints a message
  auto helloWorldFn = [&](std::string str) {
    return "Hello " + str + "!";
  };

  auto helloWorld = lua.CreateFunction<std::string(std::string)>(helloWorldFn);

  global.Set("helloWorld", helloWorld);

  std::ifstream ifs("module.lua");
  std::string script(
    (std::istreambuf_iterator<char>(ifs)),
    (std::istreambuf_iterator<char>())
  );

  lua.RunScript(script);

  return 0;
}
```

#### AngelScript

AngelScript gives you the very precise control of what you are doing, including the type checks and conversion (which neither of the scripting languages mentioned above does). But it has its impact on the integration complexity.

*Sample AngelScript script:* ((AngelScript getting started)[http://www.angelcode.com/angelscript/sdk/docs/manual/doc_hello_world.html], (Using function context)[http://www.angelcode.com/angelscript/sdk/docs/manual/doc_call_script_func.html])

```cpp
int multiply(int a, int b) {
    print(helloWorld("user"));
    print("Will calculate " + a + " times " + b);

    int c = 0;

    for (int i = 0; i < b; i++) {
        c = c + a;
    }

    return c;
}
```

*AngelScript integration:*

```cpp
#include <iostream>
#include <string>

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

void print(const std::string &s) {
    std::cout << s << std::endl;
}

int main() {
    asIScriptEngine *engine = asCreateScriptEngine();

    int r;

    // AngelScript does not have the built-int string type
    RegisterStdString(engine);

    // register function with a signature and a reference to a C++ function
    r = engine->RegisterGlobalFunction("const string helloWorld(const string &name)", asFUNCTION(helloWorld), asCALL_CDECL);
    assert(r >= 0);

    // register one more function
    r = engine->RegisterGlobalFunction("void print(const string &s)", asFUNCTION(print), asCALL_CDECL);
    assert(r >= 0);

    // CScriptBuilder is a helper class to build modules with ease
    CScriptBuilder builder;
    r = builder.StartNewModule(engine, "MyModule");
    assert(r >= 0);

    // load code from a file
    r = builder.AddSectionFromFile("module.as");
    assert(r >= 0);

    // compile a module
    r = builder.BuildModule();
    assert(r >= 0);

    // get function from a module
    asIScriptModule *mod = engine->GetModule("MyModule");

    asIScriptFunction *func = mod->GetFunctionByDecl("int multiply(int, int)");
    assert(func != 0);

    // create a context to run a function
    asIScriptContext *ctx = engine->CreateContext();
    ctx->Prepare(func);

    // set first and second arguments for a function to be called
    ctx->SetArgDWord(0, 4);
    ctx->SetArgDWord(1, 12);

    // execute a function
    r = ctx->Execute();
    assert(r != asEXECUTION_EXCEPTION);

    // get the returned data as asDWord value
    asDWORD ret = ctx->GetReturnDWord();

    // free the context
    ctx->Release();

    engine->ShutDownAndRelease();

    return 0;
}
```

