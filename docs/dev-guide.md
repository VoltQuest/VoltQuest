# ⚙️ VoltQuest Developer Guide

This guide helps contributors get started with building and modifying VoltQuest. It covers setup, build instructions, code style, and contribution notes.

---

## 🧰 Setup Instructions

### 🔧 Prerequisites

- C++17-compatible compiler (GCC, Clang, or MSVC)
- CMake ≥ 3.16
- Git
- Raylib (fetched automatically via CMake)

---

### 🚀 Build Steps

```bash
git clone https://github.com/VoltQuest/VoltQuest.git
cd VoltQuest

cmake -S . -B build
cmake --build build

```
----------

## 🧭 Code Style Guide

VoltQuest is gradually evolving toward a consistent style inspired by modern C++ practices and convention standards.

## ✅ General Code Style Guidelines

| Element           | Convention              | Example                         |
|------------------|--------------------------|---------------------------------|
| **Files**         | `snake_case`            | `level_manager.cpp`             |
| **Variables**     | `snake_case`            | `current_level_index`           |
| **Functions**     | `camelCase`             | `loadLevel()`, `drawUI()`       |
| **Classes/Structs** | `PascalCase`          | `WindowManager`                 |
| **Constants**     | `UPPER_SNAKE_CASE`      | `DEFAULT_SCALE`                 |
| **Enums**         | `PascalCase` enum class | `ComponentType`                 |


### 🧼 Best Practices

-   Prefer `const` and references when possible
    
-   Keep functions short and modular
    
-   Avoid magic numbers — define constants instead
    
-   Group related functionality into separate files
    

----------

## 📬 Contribution Notes

Currently, the project is maintained solo, and changes are pushed directly.

If others join in the future, the recommended contribution flow will be:

1.  Fork the repository
    
2.  Create a new branch: `feature/your-feature`
    
3.  Make and test your changes
    
4.  Push to your fork and open a Pull Request
    
5.  Add a clear description and attach screenshots/GIFs if relevant
    

----------



