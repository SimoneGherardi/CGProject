# CGProject

## Team
- [Enrico Barbieri](https://github.com/EnricoBarbieri1997)
- [Simone Gherardi](https://github.com/SimoneGherardi)
- [Gabriele Masini](https://github.com/gmasini97)

## Libraries
- [tiny_gltf](https://github.com/syoyo/tinygltf), [tiny_obj](https://github.com/tinyobjloader/tinyobjloader): model loading
- [reactphysics3d](https://www.reactphysics3d.com/): physics engine
- [flecs](https://www.flecs.dev/): entity component system

// ...existing code...

## Project Overview

### What is CGProject?

CGProject is a **real-time 3D rendering engine** built from scratch using the **Vulkan graphics API** in **C++17**. It implements a full Vulkan rendering pipeline, an Entity Component System (ECS) architecture, physics simulation, and an editor GUI. The project was developed as part of a Computer Graphics course.

---

### Architecture & Key Systems

#### 1. Vulkan Rendering Pipeline

The core of the project is a manually constructed Vulkan rendering pipeline. Unlike OpenGL, Vulkan requires explicit management of nearly every GPU resource. The project implements:

- **Instance & Device Management** — [`instance.h`](instance.h) / [`instance.cpp`](instance.cpp), [`physical_device.h`](physical_device.h) / [`physical_device.cpp`](physical_device.cpp), [`logical_device.h`](logical_device.h) / [`logical_device.cpp`](logical_device.cpp): Vulkan instance creation, physical device selection (GPU enumeration, queue family discovery), and logical device setup with required extensions and features.

- **Swap Chain** — [`swap_chain.h`](swap_chain.h) / [`swap_chain.cpp`](swap_chain.cpp): Manages the set of framebuffer images that are presented to the screen. Handles surface format selection, present mode (V-Sync), and extent calculation. Swap chain recreation on window resize is a notable Vulkan challenge handled here.

- **Render Pass** — [`render_pass.h`](render_pass.h) / [`render_pass.cpp`](render_pass.cpp): Defines the framebuffer attachment configuration (color, depth), subpasses, and dependencies. This is Vulkan's way of describing the rendering operations structure.

- **Graphics Pipeline** — [`graphics_pipeline.h`](graphics_pipeline.h) / [`graphics_pipeline.cpp`](graphics_pipeline.cpp): The fixed-function and programmable stage configuration — vertex input, input assembly, viewport, rasterizer, multisampling, depth/stencil, color blending, and shader stages. Vulkan pipelines are immutable once created, which is a key architectural difference from OpenGL.

- **Command Buffers & Pools** — [`command_buffers.h`](command_buffers.h) / [`command_buffers.cpp`](command_buffers.cpp), [`command_pool.h`](command_pool.h) / [`command_pool.cpp`](command_pool.cpp), [`CommandBuffer.h`](CommandBuffer.h) / [`CommandBuffer.cpp`](CommandBuffer.cpp): Record GPU commands (draw calls, pipeline binds, descriptor set binds) into command buffers allocated from pools. Commands are recorded ahead of time and submitted to queues.

- **Descriptor Sets & Layouts** — [`descriptor_set_layout.h`](descriptor_set_layout.h) / [`descriptor_set_layout.cpp`](descriptor_set_layout.cpp), [`descriptor_pool.h`](descriptor_pool.h) / [`descriptor_pool.cpp`](descriptor_pool.cpp): Vulkan's mechanism for passing uniform buffers, samplers, and other resources to shaders. Descriptor set layouts define the binding interface; descriptor pools allocate the actual descriptor sets.

- **Framebuffers** — [`framebuffers.h`](framebuffers.h) / [`framebuffers.cpp`](framebuffers.cpp): Bind swap chain image views to render pass attachments.

- **Synchronization** — [`sync_objects.h`](sync_objects.h) / [`sync_objects.cpp`](sync_objects.cpp): Semaphores and fences for CPU-GPU and GPU-GPU synchronization (image available, render finished, in-flight frames).

- **Debug Messenger** — [`debug_messenger.h`](debug_messenger.h) / [`debug_messenger.cpp`](debug_messenger.cpp): Vulkan validation layer callback for development-time error reporting.

- **Surface** — [`surface.h`](surface.h) / [`surface.cpp`](surface.cpp): Window surface abstraction (GLFW-Vulkan integration).

- **Image Views** — [`image_views.h`](image_views.h) / [`image_views.cpp`](image_views.cpp): Wrappers around VkImageView for accessing swap chain and texture images.

#### 2. Resource Management

- **Vertex & Index Buffers** — [`vertex_buffer.h`](vertex_buffer.h) / [`vertex_buffer.cpp`](vertex_buffer.cpp), [`index_buffer.h`](index_buffer.h) / [`index_buffer.cpp`](index_buffer.cpp): GPU memory allocation and data transfer for mesh geometry. Likely uses staging buffers (host-visible → device-local transfer) for optimal performance.

- **Uniform Buffers** — [`uniform_buffers.h`](uniform_buffers.h) / [`uniform_buffers.cpp`](uniform_buffers.cpp): Per-frame uniform data (MVP matrices, lighting parameters) uploaded to the GPU.

- **Texture Management** — [`texture.h`](texture.h) / [`texture.cpp`](texture.cpp), [`texture_image.h`](texture_image.h) / [`texture_image.cpp`](texture_image.cpp), [`texture_sampler.h`](texture_sampler.h) / [`texture_sampler.cpp`](texture_sampler.cpp): Image loading, VkImage creation, memory allocation, layout transitions, mipmap generation, and sampler configuration (filtering, addressing modes).

- **Depth Resources** — [`depth_resources.h`](depth_resources.h) / [`depth_resources.cpp`](depth_resources.cpp): Depth buffer image and view for depth testing.

- **Cleanup Stack** — [`cleanup_stack.h`](cleanup_stack.h) / [`cleanup_stack.cpp`](cleanup_stack.cpp): RAII-style or deferred resource destruction pattern — critical in Vulkan where resources must not be destroyed while in use by the GPU.

#### 3. Scene & Asset Pipeline

- **Asset Types & Loading** — [`asset_types.hpp`](asset_types.hpp) / [`asset_types.cpp`](asset_types.cpp), [`assets.h`](assets.h): Model loading via **tiny_gltf** (glTF 2.0 format) and **tiny_obj** (OBJ format). Handles vertex positions, normals, texture coordinates, and potentially skeletal animation data.

- **Camera** — [`camera.h`](camera.h) / [`camera.cpp`](camera.cpp): View and projection matrix computation. Likely implements FPS-style or orbit camera with keyboard/mouse input.

- **Angles** — [`Angles.h`](Angles.h): Euler angle or quaternion utilities for rotation representation.

#### 4. Entity Component System (ECS)

The project uses **[flecs](https://www.flecs.dev/)**, a high-performance ECS framework. This is a data-oriented architecture where:
- **Entities** are lightweight IDs
- **Components** are plain data structs (transform, mesh reference, physics body, etc.)
- **Systems** iterate over entities with specific component combinations

This is architecturally significant because it decouples data from behavior, enables cache-friendly iteration, and is the pattern used by modern game engines (Unity DOTS, Bevy).

#### 5. Physics Engine

**[ReactPhysics3D](https://www.reactphysics3d.com/)** integration provides:
- Rigid body dynamics
- Collision detection (broad phase + narrow phase)
- Collision shapes (boxes, spheres, convex meshes)
- Physics world stepping synchronized with the render loop

#### 6. Editor GUI

- [`EditorGUI.h`](EditorGUI.h) / [`EditorGUI.cpp`](EditorGUI.cpp): Built with **Dear ImGui** (immediate mode GUI). Provides runtime inspection and manipulation of scene objects, likely including transform editing, entity inspection, and debug visualization. ImGui integrates with Vulkan via its backend in `libraries/imgui/backends`.

#### 7. Shader Pipeline

- [`shaders/`](shaders/) directory: GLSL shaders compiled to SPIR-V (Vulkan's shader bytecode format). The [`_maker.ps1`](_maker.ps1) PowerShell script likely automates shader compilation using `glslc` or `glslangValidator`.

#### 8. Configuration & Build

- **Build System**: Visual Studio 2022 (`v143` toolset), targeting Windows x64. The [`CGProject.vcxproj`](CGProject.vcxproj) defines Debug, Release, and **Build Assets** configurations.
- **C++17** standard with C11 for C files.
- **Dependencies**: Vulkan SDK 1.3.224.1, GLFW 3.3.8 (pre-built), GLM (header-only math), ReactPhysics3D (built from source via CMake), flecs, ImGui.
- **Defines** — [`defines.h`](defines.h) / [`defines.cpp`](defines.cpp): Global constants and configuration macros.
- **Conventions** — [`conventions.txt`](conventions.txt): Coding standards documentation.

