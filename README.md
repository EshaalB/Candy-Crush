 # 🍬 Candy Crush Clone (C++ & SFML)

A C++ implementation of a **Candy Crush-style puzzle game** using the **SFML** library. Players swap candies on a 7×7 grid to create matches of three or more, earning points with limited moves or time. Includes game-saving, hints, special candies, and a user-friendly interface.

---

## 🎮 Features

- **Core Gameplay**: Swap adjacent candies to form horizontal/vertical matches of 3+.
- **Special Candies**: Create special candies with matches of 5+ that trigger unique effects.
- **Game Mechanics**:  
  - 15 moves per game  
  - 60-second timer  
  - Options: Pause, Restart, Show Hint  
- **Save/Load Game**: Save your progress and continue later.
- **Visuals & Audio**:  
  - Colorful candy sprites  
  - Grid overlay  
  - Background music (optional)

---

## 🕹️ Controls

| Key | Action              |
|-----|---------------------|
| `N` | Start New Game       |
| `L` | Load Saved Game      |
| `S` | Save Game            |
| `H` | Show Hint            |
| `P` | Pause/Unpause        |
| `R` | Restart              |
| `E` | Exit                 |

- **Left-Click**: Select and swap candies

---

## ⚙️ Prerequisites

- C++ Compiler (e.g., g++ with C++11 or later)
- SFML 2.5.1 or later
- CMake (optional)
- Windows / macOS / Linux

---

## 🧱 Installation

### 1. Install SFML

#### Windows

1. **Download SFML**  
   From: [https://www.sfml-dev.org/download.php](https://www.sfml-dev.org/download.php)  
   Choose the version for your compiler (MinGW or Visual Studio).

2. **Extract SFML**  
   Example: `C:\SFML`

3. **Set up Compiler**  
   - Add MinGW `bin` folder to PATH (e.g., `C:\MinGW\bin`)
   - Ensure `C:\SFML\include` and `C:\SFML\lib` are accessible

4. **Copy DLLs**  
   Copy SFML `.dll` files from `C:\SFML\bin` to your project executable folder.

#### macOS

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install sfml
