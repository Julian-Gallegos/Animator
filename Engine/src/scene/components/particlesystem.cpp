/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "particlesystem.h"
#include <scene/sceneobject.h>

REGISTER_COMPONENT(ParticleSystem, ParticleSystem)

ParticleSystem::ParticleSystem() :
    ParticleGeometry({"Sphere"}, 0),
    ParticleMaterial(AssetType::Material),
    InitialVelocity(glm::vec3(5.0f, 5.0f, 0.0f)),
    Mass(0.1f, 0.0f, 10.0f, 0.1f),
    Period(0.5f, 0.0f, 1.0f, 0.01f),
    ConstantF(glm::vec3(0.0f, -9.8f, 0.0f)),
    DragF(0.0f, 0.0f, 10.0f, 0.01f),
    constant_force_(ConstantF.Get()),
    // REQUIREMENT:
    // init drag force with DragF -- refer to how we deal with constant_force_
    // remember  (f = -k_d * v), where DragF represents k_d
    drag_force_(DragF.Get()),
    num_particles_(0),
    particle_index_(0),
    simulating_(false)
{
    AddProperty("Geometry", &ParticleGeometry);
    AddProperty("Material", &ParticleMaterial);
    AddProperty("Initial Velocity", &InitialVelocity);
    AddProperty("Mass", &Mass);
    AddProperty("Period (s)", &Period);
    AddProperty("Constant Force", &ConstantF);
    AddProperty("Drag Coefficient", &DragF);

    ParticleGeometry.ValueSet.Connect(this, &ParticleSystem::OnGeometrySet);

    forces_.push_back(std::shared_ptr<Force>(&constant_force_));

    // REQUIREMENT: 
    //    add viscous drag force into forces_ array if your drag force also inherit from class Force
    //    If not, you could use your own way to prepare your drag force
    forces_.push_back(std::shared_ptr<Force>(&drag_force_)); // Assuming this works like the one for constant_force_ might need to review smart pointers.
}

void ParticleSystem::UpdateModelMatrix(glm::mat4 model_matrix) {
   model_matrix_ = model_matrix;
}

void ParticleSystem::EmitParticles() {
    if (!simulating_) return;

    // REQUIREMENT:
    // Create some particles!
    //    - We have designed a class Particle for you
    //    - We've provided some UI controls for you
    //          -- Mass.Get() defines particle mass, and
    //          -- InitialVelocity.Get() defines particle init velocity in local object space
    //    - Notice particles should be created in world space. (use model_matrix_ to transform particles from local object space to world space)
    // Store particles in the member variable particles_
    // For performance reasons, limit the amount of particles that exist at the same time
    // to some finite amount (MAX_PARTICLES). Either delete or recycle old particles as needed.

    // Not sure if I'm supposed to be creating "MAX_PARTICLES" particles all at once here or not
    // Also not sure if there's sopme case here I'm not accounting for, like how would I check if I even need to "delete or recycle old particles as needed."
    while (num_particles_ < MAX_PARTICLES) {
        glm::vec3 position = glm::vec3(model_matrix_*glm::vec4(0,0,0,1)); // I suppose the local position should be (0,0,0)?
        glm::vec3 velocity = glm::vec3(model_matrix_*glm::vec4(InitialVelocity.Get(), 1));
        glm::vec3 rotation = glm::vec3(model_matrix_*glm::vec4(0,0,0,1)); // I don't think we're passed in any info for angle at this point?

        particles_.push_back(std::unique_ptr<Particle>(new Particle(Mass.Get(), position, velocity, rotation))); // Once again, double check that I'm using smart pointer correctly.
        num_particles_++;
    }

    // Reset the time
    time_to_emit_ = Period.Get();
}

std::vector<Particle*> ParticleSystem::GetParticles() {
    // Return a vector of particles (used by renderer to draw them)
    std::vector<Particle*> particles;
    for (auto& particle : particles_) particles.push_back(particle.get());
    return particles;
}

void ParticleSystem::StartSimulation() {
    simulating_ = true;
    constant_force_.SetForce(ConstantF.Get());
    // REQUIREMENT:
    // Set your added drag force as DragF.Get() -- Refer to what we did on constact_force_
    drag_force_.SetForce(DragF.Get());
    ResetSimulation();
}

void ParticleSystem::UpdateSimulation(float delta_t, const std::vector<std::pair<SceneObject*, glm::mat4>>& colliders) {
    if (!simulating_) return;

    // Emit Particles
    time_to_emit_ -= delta_t;
    if (time_to_emit_ <= 0.0) EmitParticles();

    // REQUIREMENT:
    // For each particle ...
    //      Calculate forces
    //      Solve the system of forces using Euler's method
    //      Update the particle
    //      Check for and handle collisions



   // Collision code might look something like this:
   for (auto& kv : colliders) {
       SceneObject* collider_object = kv.first;
       glm::mat4 collider_model_matrix = kv.second;

       static const double EPSILON = 0.1;
       float particle_radius = 0.5f;

       // When checking collisions, remember to bring particles from world space to collider local object space
       // The trasformation matrix can be derived by taking invese of collider_model_matrix
       if (SphereCollider* sphere_collider = collider_object->GetComponent<SphereCollider>()) {
           // Check for Sphere Collision
       } else if (PlaneCollider* plane_collider = collider_object->GetComponent<PlaneCollider>()) {
           // Check for Plane Collision
       }
       // one of the above should always be true in the current version, I suppose.
       // Even though there's also a cylindercollider

       // When updating particle velocity, remember it's in the world space.
   }
}

void ParticleSystem::StopSimulation() {
    simulating_ = false;
}

void ParticleSystem::ResetSimulation() {
    // Clear all particles
    time_to_emit_ = Period.Get();
}

bool ParticleSystem::IsSimulating() {
    return simulating_;
}


void ParticleSystem::OnGeometrySet(int c) {
    GeomChanged.Emit(ParticleGeometry.GetChoices()[c]);
}
