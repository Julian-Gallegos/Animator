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

static glm::vec3 ReflectVec(glm::vec3 V, glm::vec3 N, double restitution);

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
    if (num_particles_ == MAX_PARTICLES) {
        delete particles_.front().release();
        particles_.pop_front();
        num_particles_--;
    }
    glm::vec3 position = glm::vec3(model_matrix_*glm::vec4(0,0,0,1.f)); // I suppose the local position should be (0,0,0)?
    glm::vec3 velocity = glm::vec3(model_matrix_*glm::vec4(InitialVelocity.Get(), 0.f));
    glm::vec3 rotation = glm::vec3(model_matrix_*glm::vec4(0,0,0,1.f)); // I don't think we're passed in any info for angle at this point?

    particles_.push_back(std::unique_ptr<Particle>(new Particle(Mass.Get(), position, velocity, rotation)));
    num_particles_++;

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
    Particle *p;
    std::list<std::unique_ptr<Particle>>::iterator iter;
    for (iter = particles_.begin(); iter != particles_.end(); iter++) {
        p = iter->get();

        // I think this is how we get these?
        glm::vec3 gravity = constant_force_.GetForce(*p),
                  drag = drag_force_.GetForce(*p),
                  net_force = gravity + drag;
        p->Velocity += net_force/p->Mass * delta_t;
        p->Position = p->Position + p->Velocity*delta_t;

        // Collision code might look something like this:
        for (auto& kv : colliders) {
            SceneObject* collider_object = kv.first;
            glm::mat4 collider_model_matrix = kv.second;

            static const double EPSILON = 0.1;
            float particle_radius = 0.5f;

            // When checking collisions, remember to bring particles from world space to collider local object space
            // The trasformation matrix can be derived by taking invese of collider_model_matrix
            glm::mat4 inverse = glm::inverse(collider_model_matrix);
            glm::vec3 local_position = glm::vec3(inverse * glm::vec4(p->Position, 1.f)); // update p->Position to local coords, is currently world coords
            glm::vec3 local_vel = glm::vec3(inverse * glm::vec4(p->Velocity, 0.f));

            if (SphereCollider* sphere_collider = collider_object->GetComponent<SphereCollider>()) {
                 // Check for Sphere Collision
                 double sphere_radius = sphere_collider->Radius.Get();
                 double mag = sqrt(local_position.x*local_position.x + local_position.y*local_position.y + local_position.z*local_position.z);
                 if (mag <= particle_radius + sphere_radius + EPSILON) {
                     // collision
                     glm::vec3 norm = glm::normalize(local_position);
                     glm::vec3 bounce = ReflectVec(local_vel, norm, sphere_collider->Restitution.Get());
                     p->Velocity = glm::vec3(collider_model_matrix * glm::vec4(bounce, 0.f));
                     p->Position = p->Position + p->Velocity*delta_t + p->Velocity*static_cast<float>(EPSILON);

                 }
            } else if (PlaneCollider* plane_collider = collider_object->GetComponent<PlaneCollider>()) {
                 // Check for Plane Collision
                 // plane stretches across X-Y plane, centered at origin
                 // Width is the size of its x range
                 // Height is the size of its y range
                 glm::vec3 plane_norm(0.f, 0.f, 1.f);
                 if (glm::dot(local_position, plane_norm) <= particle_radius + EPSILON &&
                        abs(local_position.x) < plane_collider->Width.Get()/2.f &&
                        abs(local_position.y) < plane_collider->Height.Get()/2.f &&
                        local_vel.z < 0) {
                     glm::vec3 bounce = ReflectVec(local_vel, plane_norm, plane_collider->Restitution.Get());
                     p->Velocity = glm::vec3(collider_model_matrix * glm::vec4(bounce, 0.f));
                     p->Position = p->Position + p->Velocity*delta_t + p->Velocity*static_cast<float>(EPSILON);
                 }
            }
            // one of the above should always be true in the current version, I suppose.
            // Even though there's also a cylindercollider
        }
    }
}

void ParticleSystem::StopSimulation() {
    simulating_ = false;
}

void ParticleSystem::ResetSimulation() {
    // Clear all particles
    std::list<std::unique_ptr<Particle>>::iterator iter;
    for (iter = particles_.begin(); iter != particles_.end(); iter++) {
        delete iter->release();
    }
    particles_.clear();
    num_particles_ = 0;
    time_to_emit_ = Period.Get();
}

bool ParticleSystem::IsSimulating() {
    return simulating_;
}


void ParticleSystem::OnGeometrySet(int c) {
    GeomChanged.Emit(ParticleGeometry.GetChoices()[c]);
}

static glm::vec3 ReflectVec(glm::vec3 V, glm::vec3 N, double restitution) {
    glm::vec3 Vn, Vt;
    Vn = glm::dot(N, V)*N;
    Vt = V-Vn;
    return Vt - glm::vec3(restitution*glm::dvec3(Vn));
}
