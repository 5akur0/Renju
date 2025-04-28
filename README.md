# Renju AI Engine

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![SDL2](https://img.shields.io/badge/SDL2-Graphics-green.svg)
![License](https://img.shields.io/badge/License-Academic-red.svg)

A high-performance **Renju (Gomoku)** game system featuring a robust AI and a polished GUI. Developed for the *Introduction to Computing* course at Peking University.

## Key Features

* **Smart AI:** Implements **Minimax with Alpha-Beta Pruning** (4-layer depth) and heuristic evaluation.
* **Strict Rules:** Full implementation of Renju forbidden moves (Double 3, Double 4, Overline) for the Black player.
* **Modern GUI:** Built with **SDL2** for smooth rendering and interaction.
* **Game State:** Support for **Save/Load** functionality and **Undo** operations.

## Tech Stack

* **Language:** C++17
* **Graphics:** SDL2, SDL2_image, SDL2_ttf
* **Data:** JsonCpp
* **Algorithm:** Alpha-Beta Pruning, Heuristic Search

## Quick Start

**Platform:** macOS (ARM64 optimized)

### 1. Install Dependencies
```bash
brew install sdl2 sdl2_image sdl2_ttf jsoncpp
```

### 2. Build
```bash
make
```

### 3. Run
```bash
./renju
```

## AI & Logic

The AI decision-making process utilizes a comprehensive evaluation system:

* **Search:** Dynamic depth search with Alpha-Beta pruning to optimize performance.
* **Evaluation:** Pattern-based scoring.
* **Forbidden Moves:** Automatic detection and pruning of illegal moves for the Black player.

## Structure

The system is designed with a modular architecture separating Logic, AI, and UI:

| Module | Component | Description |
|--------|-----------|-------------|
| **Core** | `Board` | Manages the 15x15 grid state and piece validation. |
| | `GameManager` | Handles game loop, undo/redo, and file I/O (Save/Load). |
| **Intelligence** | `AIAlgorithms` | Implements **Minimax** with **Alpha-Beta Pruning**. |
| | `Evaluate` | Calculates board value using tuple-matching for patterns. |
| | `ForbiddenMove` | Enforces Renju rules (Double-3, Double-4) for Black. |
| **Interface** | `GameUI` | Wraps **SDL2** calls for rendering graphics and handling events. |

---

> Note: The source code comments are in Chinese as this was originally a course assignment at Peking University (Fall 2024).

**Built at Peking University**