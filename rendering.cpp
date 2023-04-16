#include "rendering.h"
#include "reactphysics3d/reactphysics3d.h"
#include "rendering_engine.h"
#include "InstanceData.h"
#include "physics.h"
#include "game_engine.h"
#include <glm/ext/matrix_transform.hpp>

void GetTransform(flecs::entity e, Transform& transform, Renderer& renderer)
{
    flecs::entity current = e;
    rp3d::Transform t = transform.LocalTransform();

    while (current.has_relation(flecs::ChildOf))
    {
        flecs::entity parent = current.parent();
        t = parent.get<Transform>()->LocalTransform() * t;
        current = parent;
    }
    renderer.GlobalTransform = t;
}

void CopyToRenderingEngine(flecs::entity e, Renderer renderer)
{
    // std::cout << renderer.GlobalTransform.to_string() << std::endl;
    float matrix[16];
    renderer.GlobalTransform.getOpenGLMatrix(matrix);
    GPUInstanceData d = {};
    d.ModelId = renderer.Mesh;
    d.TextureIndex = RenderContext::GetInstance().Models[renderer.Mesh].TextureId;
    d.ModelMatrix = glm::mat4(
        matrix[0], matrix[1], matrix[2], matrix[3],
        matrix[4], matrix[5], matrix[6], matrix[7],
        matrix[8], matrix[9], matrix[10], matrix[11],
        matrix[12], matrix[13], matrix[14], matrix[15]
    );
    RenderingEngine::GetInstance().GetCurrentFrameData()->Objects.InstanceDataStore.Add(renderer.Mesh, d);

    if (GameEngine::GetInstance().ShowBoundingBoxes && e.has<Collider>())
    {
        auto collider = e.get<Collider>();
        GPUInstanceData colliderInstance = {};
        glm::vec3 size;
        switch (collider->Type)
        {
            case rp3d::CollisionShapeName::SPHERE:
            {
                colliderInstance.ModelId = Models::SPHEREBB;
                size = glm::vec3(collider->Size.x);
                break;
            }
            case rp3d::CollisionShapeName::CAPSULE:
            {
                colliderInstance.ModelId = Models::CAPSULEBB;
                size = glm::vec3(collider->Size.x, collider->Size.y, collider->Size.x);
                break;
            }
            case rp3d::CollisionShapeName::BOX:
            {
                colliderInstance.ModelId = Models::CUBICBB;
                size = glm::vec3(collider->Size.x, collider->Size.y, collider->Size.z);
				break;
			}
            default:
                break;
        }
        colliderInstance.TextureIndex = 0xFFFFFFFF;
        colliderInstance.ModelMatrix =  glm::scale(d.ModelMatrix, size);

        RenderingEngine::GetInstance().GetCurrentFrameData()->Objects.InstanceDataStore.Add(colliderInstance.ModelId, colliderInstance);
    }
}

Rendering::Rendering(flecs::world& world)
{
    world.module<Rendering>("Rendering");

    _Renderer = world.component<Renderer>();

    _GetTransform = world.system<Transform, Renderer>()
        .kind(flecs::PreStore)
        .each(GetTransform);
    _CopyInfos = world.system<Renderer>()
        .kind(flecs::OnStore)
        .each(CopyToRenderingEngine);
}
