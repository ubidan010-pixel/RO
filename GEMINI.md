# Project Overview

This project is a C++ game engine and game built for "Rayman Origins". The engine is designed to be multi-platform, with support for Windows, Xbox, PlayStation, and Nintendo consoles.

## Core Engine (`core` and `engine` directories)

The engine is built around a core set of base classes and systems, including:

*   **`ITF::BaseObject`**: The fundamental class for all game objects, providing serialization, RTTI, and a handle-based object reference system (`ObjectRef`).
*   **`ITF::ObjectId`**: A system for generating unique object identifiers.
*   **Custom Data Types and Containers**: The engine uses its own set of data types (e.g., `u8`, `i32`, `f32`) and containers (e.g., `ITF_VECTOR`, `ITF_MAP`) for portability and control over memory management.
*   **Component-Based Architecture**: The `engine` directory is organized by functionality (e.g., `actors`, `animation`, `physics`, `sound`), suggesting a component-based approach to game object design.

## Gameplay (`gameplay` directory)

The `gameplay` directory contains the specific logic for the game, including:

*   **AI**: Artificial intelligence for non-player characters.
*   **Components**: Reusable gameplay components that can be attached to game objects.
*   **Managers**: Global systems for managing gameplay state.

## Game (`rayman` directory)

The `rayman` directory contains the main game project, which brings together the engine and gameplay modules to create the final game.

# Building and Running

TODO: The build and run commands for this project are not immediately apparent from the source code. This section should be updated with instructions on how to build and run the game on each supported platform.

# Development Conventions

*   **C++ Namespace**: All engine code is contained within the `ITF` namespace.
*   **Naming Conventions**:
    *   Member variables are prefixed with `m_`.
    *   Enums are prefixed with `e`.
*   **Debugging**: The engine uses a system of assertions (`ITF_ASSERT`) and logging macros (e.g., `LOG_FRED`, `LOG_JAY`) for debugging.
*   **Comments**: The code is well-commented, and developers should maintain this standard.
