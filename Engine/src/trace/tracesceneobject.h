#ifndef TRACESCENEOBJECT_H
#define TRACESCENEOBJECT_H

#include "tracelight.h"

#include <scene/components/geometry.h>

// Anything that might be intersected within the scene bounds
class TraceSceneObject
{
public:
    virtual bool Intersect(const Ray&r, Intersection&i) = 0;

    BoundingBox* world_bbox;
};

// A simple object without hierarchical transformations and only a Geometry for fast tracing
// It renders using a Material with a trace-compatible shader and our shading model
class TraceGeometry : public TraceSceneObject
{
public:
    TraceGeometry(Geometry* geometry_);
    TraceGeometry(Geometry* geometry_, glm::mat4 transform_);
    ~TraceGeometry();

    virtual bool Intersect(const Ray&r, Intersection&i);

    Geometry* geometry;
    bool identity_transform;
    glm::mat4 transform; //local2world
    glm::mat4 inverse_transform;
    glm::mat3 normals_transform; //local2world
};

// This object visually represents a light source (and can possibly light the scene through refracting objects)
// It renders as basically how much light goes straight from the light source into the camera
class TraceFlare : public TraceSceneObject
{
public:
    TraceFlare(TraceLight* light);
    ~TraceFlare();

    virtual bool Intersect(const Ray&r, Intersection&i);

    glm::vec3 GetIntensity(const Ray&r);

    TraceLight* trace_light;
    glm::vec3 center;
    float radius;
    float area;
};

#endif // TRACESCENEOBJECT_H
