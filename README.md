Candy Crush Clone
A C++ implementation of a Candy Crush-like puzzle game using the SFML library. The game features a 7x7 grid where players swap candies to create matches of three or more, earning points within a limited number of moves or time. It includes features like saving/loading games, hints, special candies, and a user-friendly interface.
Features

Gameplay: Swap adjacent candies to form horizontal or vertical matches of three or more.
Special Candies: Matches of five or more candies create special candies with unique visuals.
Game Mechanics: Limited moves (15) and time (60 seconds) with pause, restart, and hint options.
Save/Load: Save progress to a file and load it to continue playing.
Visuals & Audio: Colorful candy sprites, background music, and a grid overlay for enhanced visuals.
Controls:
N: Start new game
L: Load saved game
S: Save game
H: Show hint
P: Pause/unpause
R: Restart
E: Exit
Left-click to select and swap candies



Prerequisites

C++ compiler (e.g., g++ with C++11 or later)
SFML 2.5.1 or later
Operating System: Windows, macOS, or Linux
CMake (optional, for build configuration)

Installation
1. Install SFML
SFML (Simple and Fast Multimedia Library) is required to build and run the game. Follow these steps to set it up:
Windows

Download SFML:
Visit SFML Downloads and download the latest version (e.g., SFML 2.5.1) for your compiler (e.g., MinGW for g++ or Visual Studio).


Extract SFML:
Unzip the downloaded file to a directory (e.g., C:\SFML).


Set Up Compiler:
For MinGW: Add the MinGW bin directory to your system PATH (e.g., C:\MinGW\bin).
Ensure SFML's include and lib folders are accessible (e.g., C:\SFML\include and C:\SFML\lib).


Copy DLLs:
Copy the necessary SFML DLLs (e.g., sfml-graphics-2.dll, sfml-window-2.dll, sfml-system-2.dll, sfml-audio-2.dll) from C:\SFML\bin to your project's executable directory or system PATH.



macOS

Install Homebrew (if not already installed):/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"


Install SFML:brew install sfml


Link SFML:
Homebrew installs SFML to /usr/local/Cellar/sfml or /opt/homebrew/Cellar/sfml. The compiler should automatically find the headers and libraries.



Linux (Ubuntu/Debian)

Install SFML:sudo apt-get update
sudo apt-get install libsfml-dev


Verify Installation:
SFML libraries and headers will be installed in standard system directories (e.g., /usr/include/SFML and /usr/lib).



2. Clone the Repository
git clone https://github.com/your-username/candy-crush-clone.git
cd candy-crush-clone

3. Project Structure
Ensure the following directory structure:
candy-crush-clone/
├── assets/
│   ├── fonts/
│   │   └── arial.ttf
│   ├── img/
│   │   ├── blue.png
│   │   ├── red.png
│   │   ├── purple.png
│   │   ├── yellow.png
│   │   ├── green.png
│   │   ├── orange.png
│   │   ├── special.png
│   │   ├── grid.png
│   │   └── moon.jpg
│   └── sounds/
│       └── moremore.ogg
├── src/
│   └── main.cpp
├── README.md
└── CMakeLists.txt (optional)

4. Build the Project
Using g++ (Manual Compilation)

Compile the source code:g++ -c src/main.cpp -I/path/to/sfml/include


Link the object files with SFML libraries:g++ main.o -o candy_crush -L/path/to/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

Replace /path/to/sfml with the actual path (e.g., C:\SFML on Windows or omit for Linux/macOS if installed via package manager).

Using CMake

Create a CMakeLists.txt file in the project root:cmake_minimum_required(VERSION 3.10)
project(CandyCrush)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)
add_executable(candy_crush src/main.cpp)
target_link_libraries(candy_crush sfml-system sfml-window sfml-graphics sfml-audio)


Build the project:mkdir build
cd build
cmake ..
make



5. Run the Game

Ensure the assets folder is in the same directory as the executable.
Run the executable:./candy_crush


On Windows, ensure SFML DLLs are in the executable directory or system PATH.

Troubleshooting

SFML Not Found: Verify the include and lib paths in your compiler flags or CMake configuration.
Missing Assets: Ensure the assets folder contains all required files (images, font, and sound).
DLL Errors (Windows): Copy SFML DLLs to the executable directory or add them to the system PATH.
No Sound: The background music (moremore.ogg) is optional; the game runs without it if the file is missing.

Contributing
Contributions are welcome! Please follow these steps:

Fork the repository.
Create a new branch (git checkout -b feature/your-feature).
Commit your changes (git commit -m "Add your feature").
Push to the branch (git push origin feature/your-feature).
Open a pull request.

License
This project is licensed under the MIT License. See the LICENSE file for details.
Acknowledgments

SFML for providing the multimedia library.
Inspired by the classic Candy Crush game.


Last updated: June 16, 2025
