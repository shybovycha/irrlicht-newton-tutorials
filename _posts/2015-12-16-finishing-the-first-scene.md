---
layout: post
title: Finishing the first scene
date: '2015-12-16T18:00:00+01:00'
---

If you remember, we ended our coding excercises at place, where we almost created our first
Newtonian body, but we did not actually have enough models.

We discussed collision shapes a bit. So let's create one for our brand new model!

We have a nice ramp to work with. But how we can reconstruct the same shape in the
terms of Newton? Newton offers a set of collision shapes for us:

* Sphere

    <img src="{{ site.baseurl }}/images/collision_shapes/sphere.jpg" alt="Sphere" class="img-responsive">

* Box

    <img src="{{ site.baseurl }}/images/collision_shapes/box.jpg" alt="Box" class="img-responsive">

* Cone

    <img src="{{ site.baseurl }}/images/collision_shapes/cone.jpg" alt="Cone" class="img-responsive">

* Capsule

    <img src="{{ site.baseurl }}/images/collision_shapes/capsule.jpg" alt="Capsule" class="img-responsive">

* Cylinder

    <img src="{{ site.baseurl }}/images/collision_shapes/cylinder.jpg" alt="Cylinder" class="img-responsive">

* Chamfer Cylinder

    <img src="{{ site.baseurl }}/images/collision_shapes/chamfer_cylinder.jpg" alt="Chamfer Cylinder" class="img-responsive">

* Convex Hull

    <img src="{{ site.baseurl }}/images/collision_shapes/convex_hull.jpg" alt="Convex Hull" class="img-responsive">

* Trimesh

    <img src="{{ site.baseurl }}/images/collision_shapes/trimesh.jpg" alt="Trimesh" class="img-responsive">

Obviously, not sphere, cone, capsule, nor cylinder make sense for us.
We could use box shape, but then we simply ignore our inner faces (inside walls):

<img src="{{ site.baseurl }}/images/collision_shapes/ramp_box.png" alt="Box collision shape for our ramp" class="img-responsive">

A bit better, but still the same situation with convex hull shape:

<img src="{{ site.baseurl }}/images/collision_shapes/ramp_convex.png" alt="Convex hull collision shape for our ramp" class="img-responsive">

Generally, the way we create our Newtonian body is:

1. create collision shape
2. create blank Newtonian body
3. set body properties like collision shape, mass, inertia parameters, etc.
4. store the pointer to the graphical entity for that body in the `userData` property

And then Newton Game Dynamics will take your body into account when processing other objects
in the `NewtonWorld`.

## Tree mesh collision shape

So we gonna use the triangle mesh shape. What we gonna do is loop through all the triangles
of our mesh and build its copy, but in the world of _"physic"_ bodies.

To loop through all the triangles, we need to take each 3 edges of our mesh and the corresponding
vertices _(because each edge is represented by two its vertices - their indexes in the list
of vertices)_ and create Newtonian triangle. Irrlicht stores vertices in one of three formats:

1. plain vertex, represented by its three coordinates; the `irr::video::S3DVertex` class in Irrlicht
2. vertex with texture coordinates; `irr::video::S3DVertex2TCoords` class
3. vertex with its tangent information; `irr::video::S3DVertexTangents` class

All those are represented by `irr::video::S3DVertex` class or its children. Moreover, we do not
need nothing but the information on vertex' coordinates in our case, so we may use only the
base class' properties.

The code, creating trimesh collision shape is quite simple and straightforward:

{% highlight cpp %}
void createTrimeshShape(irr::scene::IMeshBuffer *meshBuffer, NewtonCollision *treeCollision,
                                     irr::core::vector3df scale = irr::core::vector3df(1, 1, 1)) {
    irr::core::vector3df vArray[3];

    irr::video::S3DVertex *mb_vertices = (irr::video::S3DVertex *) meshBuffer->getVertices();

    u16 *mb_indices = meshBuffer->getIndices();

    for (unsigned int j = 0; j < meshBuffer->getIndexCount(); j += 3) {
        int v1i = mb_indices[j + 0];
        int v2i = mb_indices[j + 1];
        int v3i = mb_indices[j + 2];

        vArray[0] = mb_vertices[v1i].Pos * scale.X;
        vArray[1] = mb_vertices[v2i].Pos * scale.Y;
        vArray[2] = mb_vertices[v3i].Pos * scale.Z;

        NewtonTreeCollisionAddFace(treeCollision, 3, &vArray[0].X, sizeof(irr::core::vector3df), 1);
    }
}
{% endhighlight %}

We take the edges _(indices)_, find their vertices, create a triangle - and we're done!
You may have noticed, we do not actually create the collision shape here - we take it as
an argument for our function. You will see why this is done that way in a moment.

Now it's body's turn! But we need to extend our `Entity` class with the `NewtonBody`
field so that we can seamlesly integrate it to our engine:

{% highlight cpp %}
class Entity {
private:
    scene::ISceneNode *mNode;
    NewtonBody *mBody;

public:
    Entity(scene::ISceneNode *node) : mNode(node), mBody(0) { }

    Entity(scene::ISceneNode *node, NewtonBody *body) : mNode(node), mBody(body) { }

    scene::ISceneNode *getSceneNode() const {
        return mNode;
    }

    NewtonBody *getBody() const {
        return mBody;
    }

    void setBody(NewtonBody *body) {
        mBody = body;
    }
};
{% endhighlight %}

And now we are ready to set our `NewtonBody`:

{% highlight cpp %}
void createMeshBody(const std::string name) {
    Entity *entity = entities[name];
    irr::scene::IMeshSceneNode *node = (irr::scene::IMeshSceneNode *) entity->getSceneNode();

    NewtonCollision *shape = NewtonCreateTreeCollision(newtonWorld, 0);
    NewtonTreeCollisionBeginBuild(shape);

    irr::scene::IMesh *mesh = node->getMesh();

    for (unsigned int i = 0; i < mesh->getMeshBufferCount(); i++) {
        irr::scene::IMeshBuffer *mb = mesh->getMeshBuffer(i);
        createTrimeshShape(mb, shape, node->getScale());
    }

    NewtonTreeCollisionEndBuild(shape, 1);

    float mass = 0.0f;

    dMatrix origin;
    NewtonCollisionGetMatrix(shape, &origin[0][0]);
    NewtonBody *body = NewtonCreateDynamicBody(newtonWorld, shape, &origin[0][0]);

    dVector inertia;
    NewtonConvexCollisionCalculateInertialMatrix(shape, &inertia[0], &origin[0][0]);
    NewtonBodySetMassMatrix(body, mass, mass * inertia.m_x, mass * inertia.m_y, mass * inertia.m_z);
    NewtonBodySetCentreOfMass(body, &origin[0][0]);

    NewtonDestroyCollision(shape);

    NewtonBodySetTransformCallback(body, transformCallback);
    NewtonBodySetForceAndTorqueCallback(body, applyForceAndTorqueCallback);

    NewtonBodySetUserData(body, entity);
    NewtonInvalidateCache(newtonWorld);

    entity->setBody(body);
}
{% endhighlight %}

There is an interesting piece here, though: we used recursion to create collision shape...
But if you remember, we did not create collision shape with our `createTrimeshShape` method -
all we do in that method is that we add new triangles to the existing shape. That's because meshes
in Irrlicht are stored as a tree structure - they may have sub-meshes, which are still
parts of the whole mesh. And they can have their sub-meshes too. So we created a blank
collision shape and fill it with new parts of the mesh.

Doing it that way prevents us from overcomplicating the task and building a composite mesh,
made of a set of trimeshes. That would be really hard to calculate in real-time! And our
really simple scene would work with the speed of 5x5 battle in Unreal Tournament 3...

Looking back to our list, we should now fill out all the fields for our `NewtonBody`. And since
we are making the static model, we will set its mass to zero. This is enough for Newton to
treat our body as the static one. I placed the other code to show the other fields, we need
to fill in case we have a "usual" body.

So the other fields of `NewtonBody` are:

1. `massMatrix`, which determines how the mass is spread along the body
2. `transformCallback` and `forceAndTorqueCallback` are two mandatory fields, required by Newton
3. `userData`, which will hold the pointer to the whole entity

`massMatrix` could be calculated automatically from the collision shape, like in our case.
Without digging much into details, we will simply set it so the mass of our body is distributed
uniformely.

`transformCallback` is the function, which will be called for our body each time it changes its
position due to the interaction with other bodies inside `NewtonWorld`.

`forceAndTorqueCallback` is the function, which applies forces and torques to our body. This is
a bit tricky, but you need to keep track of each force and torque by yourself and then apply
them in a way that they summ up and create the final force, influencing the body. We will
talk about it later, when we will deal with impulses.

So, the `transformCallback`:

{% highlight cpp %}
static void transformCallback(const NewtonBody *body, const dFloat *matrix, int threadIndex) {
    Entity *entity = (Entity *) NewtonBodyGetUserData(body);
    scene::ISceneNode *node = entity->getSceneNode();

    if (!node)
        return;

    core::matrix4 transform;
    transform.setM(matrix);

    node->setPosition(transform.getTranslation());
    node->setRotation(transform.getRotationDegrees());
}
{% endhighlight %}

Nothing tricky here.

To put everything in place, let's add a sphere to our scene. The process is totally same,
except of the collision shape creation - in case of primitives like box, sphere or cylinder,
it is much more easy than with trimeshes - you do not need to loop through any indices or
vertices - just set shape params like dimensions or radius. And body creation process is
totally same.

{% highlight cpp %}
void createSphereNode(const std::string name, const std::string textureFile) {
    scene::ISceneNode *node = smgr->addSphereSceneNode();

    if (node) {
        node->setMaterialTexture(0, driver->getTexture(textureFile.c_str()));
        node->setMaterialFlag(video::EMF_LIGHTING, false);
    }

    entities[name] = new Entity(node);
}

NewtonCollision *createSphereCollisionShape(scene::ISceneNode *node, float radius) {
    dQuaternion q(node->getRotation().X, node->getRotation().Y, node->getRotation().Z, 1.f);
    dVector v(node->getPosition().X, node->getPosition().Y, node->getPosition().Z);
    dMatrix origin(q, v);

    int shapeId = 0;

    return NewtonCreateSphere(newtonWorld, radius, shapeId, &origin[0][0]);
}

void createSphereBody(const std::string name, float radius, float mass) {
    Entity *entity = entities[name];
    scene::ISceneNode *node = entity->getSceneNode();

    dQuaternion q(node->getRotation().X, node->getRotation().Y, node->getRotation().Z, 1.f);
    dVector v(node->getPosition().X, node->getPosition().Y, node->getPosition().Z);
    dMatrix origin(q, v);

    int shapeId = 0;

    NewtonCollision *shape = NewtonCreateSphere(newtonWorld, radius, shapeId, &origin[0][0]);

    dMatrix origin;
    NewtonCollisionGetMatrix(shape, &origin[0][0]);
    NewtonBody *body = NewtonCreateDynamicBody(newtonWorld, shape, &origin[0][0]);

    dVector inertia;
    NewtonConvexCollisionCalculateInertialMatrix(shape, &inertia[0], &origin[0][0]);
    NewtonBodySetMassMatrix(body, mass, mass * inertia.m_x, mass * inertia.m_y, mass * inertia.m_z);
    NewtonBodySetCentreOfMass(body, &origin[0][0]);

    NewtonDestroyCollision(shape);

    NewtonBodySetTransformCallback(body, transformCallback);
    NewtonBodySetForceAndTorqueCallback(body, applyForceAndTorqueCallback);

    NewtonBodySetUserData(body, entity);
    NewtonInvalidateCache(newtonWorld);

    entity->setBody(body);
}
{% endhighlight %}

Have no fear about code duplication - we will remove it later. When you are done, you should
get picture like this one:

<img src="{{ site.baseurl }}/images/collision_shapes/ramp_with_ball.png" alt="First completed dynamic scene" class="img-responsive">

Congrats! That's our first completed dynamic scene!