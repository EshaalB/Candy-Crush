#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
// Use SFML and standard namespaces
using namespace std;
using namespace sf;

// Constants for game configuration
const int BOARD_SIZE = 7;  // Size of the game board (7x7 grid)
const int MAX_TIME = 60;   // Maximum game time in seconds

// Structure to hold game state information
struct GameState {
    int board[BOARD_SIZE][BOARD_SIZE];         // Game board grid
    int score;                                // Player's current score
    int moves;                                // Number of moves made
    int max_moves;                            // Maximum allowed moves
    int selected_i, selected_j;                // Current selected candy coordinates
    int prev_i, prev_j;                        // Previously selected candy coordinates
    int clicks;                               // Number of clicks (for selection)
    int hint_i1, hint_j1, hint_i2, hint_j2;    // Coordinates for hint candies
    bool show_hint;                           // Flag to display hint
    bool show_invalid;                        // Flag to show invalid move
    Clock hint_clock;                         // Timer for hint display
    Clock game_clock;                         // Timer for game duration
    Clock invalid_clock;                       // Timer for invalid move display
    Clock match_clock;                        // Timer for match animation
    Clock fall_clock;                         // Timer for falling candies
    bool game_started;                        // Flag for game start
    bool game_over;                           // Flag for game over
    bool is_swapping;                         // Flag for swap animation
    bool is_matching;                         // Flag for match animation
    bool is_falling;                          // Flag for falling animation
    bool paused;                              // Flag for game pause
    float fade_alpha;                         // Alpha value for fading matched candies
    int match_count;                          // Number of matched candies
    int match_rows[100];                      // Rows of matched candies
    int match_cols[100];                      // Columns of matched candies
    int specials[BOARD_SIZE][BOARD_SIZE];      // Special candy markers
    Time accumulated_time;                     // Time accumulated when paused

    // Constructor to initialize game state

    GameState() : score(0), moves(0), max_moves(15), selected_i(-1), selected_j(-1),
        prev_i(-1), prev_j(-1), clicks(0), hint_i1(-1), hint_j1(-1),
        hint_i2(-1), hint_j2(-1), show_hint(false), show_invalid(false),
        game_started(false), game_over(false), is_swapping(false), is_matching(false),
        is_falling(false), paused(false), fade_alpha(255.0f), match_count(0),
        accumulated_time(Time::Zero) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                board[i][j] = 0;
                specials[i][j] = 0;
            }
        }
    }
};

// Finds matching candies on the board
void findMatches(GameState& state) {
    state.match_count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            state.specials[i][j] = 0;
        }
    }

    // Horizontal matches
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE - 2; j++) {
            int candy = state.board[i][j];
            if (candy == -1) continue;

            if (candy == state.board[i][j + 1] && candy == state.board[i][j + 2]) {
                int len = 3;
                while (j + len < BOARD_SIZE && state.board[i][j + len] == candy) {
                    len++;
                }

                if (len >= 5) {
                    int center = j + len / 2;
                    state.specials[i][center] = 1;
                }

                for (int k = 0; k < len; k++) {
                    state.match_rows[state.match_count] = i;
                    state.match_cols[state.match_count] = j + k;
                    state.match_count++;
                }
                j += len - 1;
            }
        }
    }

    // Vertical matches
    for (int j = 0; j < BOARD_SIZE; j++) {
        for (int i = 0; i < BOARD_SIZE - 2; i++) {
            int candy = state.board[i][j];
            if (candy == -1) continue;

            if (candy == state.board[i + 1][j] && candy == state.board[i + 2][j]) {
                int len = 3;
                while (i + len < BOARD_SIZE && state.board[i + len][j] == candy) {
                    len++;
                }

                if (len >= 5) {
                    int center = i + len / 2;
                    state.specials[center][j] = 1;
                }

                for (int k = 0; k < len; k++) {
                    state.match_rows[state.match_count] = i + k;
                    state.match_cols[state.match_count] = j;
                    state.match_count++;
                }
                i += len - 1;
            }
        }
    }
}

// Removes matched candies from board
void removeMatches(GameState& state) {
    for (int idx = 0; idx < state.match_count; idx++) {
        int i = state.match_rows[idx];
        int j = state.match_cols[idx];
        if (state.specials[i][j]) {
            state.board[i][j] = 6;
        }
        else {
            state.board[i][j] = -1;
        }
    }
}

// Fills empty spaces on board
void fillBoard(GameState& state) {
    // Move candies down
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = BOARD_SIZE - 1; j >= 0; j--) {
            if (state.board[i][j] == -1) {
                for (int k = j - 1; k >= 0; k--) {
                    if (state.board[i][k] != -1) {
                        state.board[i][j] = state.board[i][k];
                        state.board[i][k] = -1;
                        break;
                    }
                }
            }
        }
    }

    // Fill top with new candies
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state.board[i][j] == -1) {
                state.board[i][j] = rand() % 6;
            }
        }
    }
}

// Checks if valid moves exist
bool hasMoves(GameState& state) {
    int temp[BOARD_SIZE][BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            temp[i][j] = state.board[i][j];
        }
    }

    // Horizontal swaps
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE - 1; j++) {
            swap(temp[i][j], temp[i][j + 1]);
            findMatches(state);
            if (state.match_count > 0) {
                return true;
            }
            swap(temp[i][j], temp[i][j + 1]);
        }
    }

    // Vertical swaps
    for (int i = 0; i < BOARD_SIZE - 1; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            swap(temp[i][j], temp[i + 1][j]);
            findMatches(state);
            if (state.match_count > 0) {
                return true;
            }
            swap(temp[i][j], temp[i + 1][j]);
        }
    }

    return false;
}

// Finds a valid move to use as a hint
bool findHint(GameState& state) {
    // Create a temporary copy of the game state
    GameState tempState;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            tempState.board[i][j] = state.board[i][j];
        }
    }

    // Check horizontal swaps
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE - 1; j++) {
            swap(tempState.board[i][j], tempState.board[i][j + 1]);
            findMatches(tempState);
            if (tempState.match_count > 0) {
                state.hint_i1 = i;
                state.hint_j1 = j;
                state.hint_i2 = i;
                state.hint_j2 = j + 1;
                return true;
            }
            swap(tempState.board[i][j], tempState.board[i][j + 1]);
        }
    }

    // Check vertical swaps
    for (int j = 0; j < BOARD_SIZE; j++) {
        for (int i = 0; i < BOARD_SIZE - 1; i++) {
            swap(tempState.board[i][j], tempState.board[i + 1][j]);
            findMatches(tempState);
            if (tempState.match_count > 0) {
                state.hint_i1 = i;
                state.hint_j1 = j;
                state.hint_i2 = i + 1;
                state.hint_j2 = j;
                return true;
            }
            swap(tempState.board[i][j], tempState.board[i + 1][j]);
        }
    }

    return false;
}

// Initializes board with random candies
void shuffleBoard(GameState& state) {
    int attempts = 0;
    do {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                state.board[i][j] = rand() % 6;
            }
        }

        findMatches(state);
        while (state.match_count > 0) {
            for (int idx = 0; idx < state.match_count; idx++) {
                int i = state.match_rows[idx];
                int j = state.match_cols[idx];
                state.board[i][j] = -1;
            }
            fillBoard(state);
            findMatches(state);
        }
        attempts++;
    } while (!hasMoves(state) && attempts < 50);
}

// Saves game state
void saveGame(GameState& state) {
    ofstream file("savegame.txt");
    if (file.is_open()) {
        file << state.score << " " << state.moves << "\n";
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                file << state.board[i][j] << " ";
            }
            file << "\n";
        }
        file.close();
    }
}

// Loads game state
bool loadGame(GameState& state) {
    ifstream file("savegame.txt");
    if (file.is_open()) {
        file >> state.score >> state.moves;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                file >> state.board[i][j];
            }
        }
        file.close();
        state.game_started = true;
        return true;
    }
    return false;
}

int main() {
    srand(time(0)); // Seed random number generator
    RenderWindow window(VideoMode(1920, 1080), "Candy Crush", Style::Default);
    Music bgMusic;

    // Try to load sounds (continue if some fail)
    
   // if (bgMusic.openFromFile("assets/sounds/moremore.ogg")) {
     //   bgMusic.setLoop(true);
       // bgMusic.play();
    //}

    // Load background
    Texture back_img;
    if (!back_img.loadFromFile("assets/img/moon.jpg")) {
        return 1;
    }
    Sprite background(back_img);
    background.setScale(1920.0f / back_img.getSize().x, 1080.0f / back_img.getSize().y);

    // Load candy textures
    Texture candyTextures[7];
    string files[] = {
        "assets/img/blue.png", "assets/img/red.png", "assets/img/purple.png",
        "assets/img/yellow.png", "assets/img/green.png", "assets/img/orange.png",
        "assets/img/special.png"
    };

    for (int i = 0; i < 7; i++) {
        if (!candyTextures[i].loadFromFile(files[i])) {
            return 1;
        }
    }

    // Load grid overlay
    Texture gridTexture;
    if (!gridTexture.loadFromFile("assets/img/grid.png")) {
        // Fallback to generated grid if texture missing
    }

    // Load font
    Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        return 1;
    }

    GameState state;
    int squareSize = 100; // Larger candy size
    int boardWidth = BOARD_SIZE * squareSize;
    int boardLeft = (1920 - boardWidth) / 2; // Center board horizontally
    int boardTop = (1080 - boardWidth) / 2; // Center board vertically

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::S && state.game_started && !state.game_over) {
                    saveGame(state);
                }
                if (event.key.code == Keyboard::L && !state.game_over) {
                    loadGame(state);
                }
                if (event.key.code == Keyboard::N) {
                    if (!state.game_started || state.game_over) {
                        // New game
                        state = GameState();
                        shuffleBoard(state);
                        state.game_started = true;
                    }
                }
                if (event.key.code == Keyboard::E) {
                    window.close();
                }
                if (event.key.code == Keyboard::H && state.game_started && !state.game_over) {
                    if (findHint(state)) {
                        state.show_hint = true;
                        state.hint_clock.restart();
                    }
                }
                if (event.key.code == Keyboard::P && state.game_started) {
                    state.paused = !state.paused;
                    if (state.paused) {
                        // Pause: store current game time
                       // bgMusic.pause();
                        state.accumulated_time += state.game_clock.getElapsedTime();
                    }
                    else {
                        // Unpause: restart clock
                      //  bgMusic.play();
                        state.game_clock.restart();
                    }
                }
                if (event.key.code == Keyboard::R) {
                    // Restart game
                    state = GameState();
                    shuffleBoard(state);
                    state.game_started = true;
                }
            }

            // Handle candy selection
            if (!state.is_swapping && !state.is_matching && !state.is_falling &&
                state.game_started && !state.game_over && !state.paused) {
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    int col = (mouseX - boardLeft) / squareSize;
                    int row = (mouseY - boardTop) / squareSize;

                    if (col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE) {
                        if (state.clicks == 0) {
                            state.selected_i = col;
                            state.selected_j = row;
                            state.clicks = 1;
                        }
                        else {
                            int new_col = col;
                            int new_row = row;

                            if ((abs(state.selected_i - new_col) == 1 && state.selected_j == new_row) ||
                                (abs(state.selected_j - new_row) == 1 && state.selected_i == new_col)) {

                                state.prev_i = state.selected_i;
                                state.prev_j = state.selected_j;
                                state.selected_i = new_col;
                                state.selected_j = new_row;
                                state.is_swapping = true;
                                state.match_clock.restart();

                                // Count move on swap attempt
                                state.moves++;

                                swap(state.board[state.prev_i][state.prev_j],
                                    state.board[state.selected_i][state.selected_j]);
                            }
                            else {
                                state.selected_i = col;
                                state.selected_j = row;
                            }
                        }
                    }
                }
            }
        }

        // Handle game states
        if (state.paused || state.game_over) {
            // Skip logic when paused/game over
           // bgMusic.stop();
        }
        else if (state.game_started) {
            // Handle swap animation
            if (state.is_swapping) {
                if (state.match_clock.getElapsedTime().asSeconds() >= 0.3f) {
                    state.is_swapping = false;
                    findMatches(state);
                    if (state.match_count > 0) {
                        state.is_matching = true;
                        state.match_clock.restart();
                        state.fade_alpha = 255.0f; // Start match animation
                    }
                    else {
                        // Revert invalid swap
                        swap(state.board[state.prev_i][state.prev_j],
                            state.board[state.selected_i][state.selected_j]);
                        state.show_invalid = true;
                        state.invalid_clock.restart();
                        state.clicks = 0;
                    }
                }
            }

            // Handle match animation
            if (state.is_matching) {
                float elapsed = state.match_clock.getElapsedTime().asSeconds();
                state.fade_alpha = 255.0f * (1.0f - elapsed / 0.5f);

                if (elapsed >= 0.5f) {
                    removeMatches(state);
                    state.score += state.match_count * 10;
                    state.is_matching = false;
                    state.is_falling = true;
                    state.fall_clock.restart();
                }
            }

            // Handle falling animation
            if (state.is_falling) {
                if (state.fall_clock.getElapsedTime().asSeconds() >= 0.4f) {
                    fillBoard(state);
                    state.is_falling = false;
                    state.clicks = 0;

                    // Check for new matches
                    findMatches(state);
                    if (state.match_count > 0) {
                        state.is_matching = true;
                        state.match_clock.restart();
                        state.fade_alpha = 255.0f; // Handle falling candies
                    }
                }
            }

            // Update timers
            if (state.show_hint && state.hint_clock.getElapsedTime().asSeconds() > 2.0f) {
                state.show_hint = false;
            }
            if (state.show_invalid && state.invalid_clock.getElapsedTime().asSeconds() > 0.5f) {
                state.show_invalid = false;
            }

            // Calculate game time (accounting for pauses)
            Time game_time = state.accumulated_time;
            if (!state.paused) {
                game_time += state.game_clock.getElapsedTime();
            }

            // Check game over
            if (game_time.asSeconds() >= MAX_TIME || state.moves >= state.max_moves) {
                state.game_over = true;
            }
        }

        // Rendering
        window.clear();
        window.draw(background);

        if (!state.game_started) {
            // Start screen
            Text title;
            title.setFont(font);
            title.setCharacterSize(80);
            title.setFillColor(Color::White);
            title.setOutlineColor(Color::Black);
            title.setOutlineThickness(3);
            title.setString("CANDY CRUSH");
            title.setPosition(1920 / 2 - title.getLocalBounds().width / 2, 300);
            window.draw(title);

            Text start;
            start.setFont(font);
            start.setCharacterSize(50);
            start.setFillColor(Color::Yellow);
            start.setOutlineColor(Color::Black);
            start.setOutlineThickness(2);
            start.setString("Press N to Start New Game\nPress L to Load Game\nPress E to Exit");
            start.setPosition(1920 / 2 - start.getLocalBounds().width / 2, 500);
            window.draw(start);
        }
        else if (state.game_over) {
            // Game over screen
            Text gameOver;
            gameOver.setFont(font);
            gameOver.setCharacterSize(100);
            gameOver.setFillColor(Color::Red);
            gameOver.setOutlineColor(Color::Black);
            gameOver.setOutlineThickness(4);
            gameOver.setString("GAME OVER");
            gameOver.setPosition(1920 / 2 - gameOver.getLocalBounds().width / 2, 300);
            window.draw(gameOver);

            Text score;
            score.setFont(font);
            score.setCharacterSize(70);
            score.setFillColor(Color::White);
            score.setOutlineColor(Color::Black);
            score.setOutlineThickness(3);
            score.setString("Final Score: " + to_string(state.score));
            score.setPosition(1920 / 2 - score.getLocalBounds().width / 2, 450);
            window.draw(score);

            Text restart;
            restart.setFont(font);
            restart.setCharacterSize(50);
            restart.setFillColor(Color::Yellow);
            restart.setOutlineColor(Color::Black);
            restart.setOutlineThickness(2);
            restart.setString("Press N to Play Again\nPress E to Exit");
            restart.setPosition(1920 / 2 - restart.getLocalBounds().width / 2, 600);
            window.draw(restart);
        }
        else {
            // Draw board background
            RectangleShape boardBackground(Vector2f(boardWidth, boardWidth));
            boardBackground.setPosition(boardLeft, boardTop);
            boardBackground.setFillColor(Color(50, 50, 50, 180));
            window.draw(boardBackground);

            // Draw grid overlay
            if (gridTexture.getSize().x > 0) {
                Sprite gridSprite(gridTexture);
                gridSprite.setPosition(boardLeft, boardTop);
                gridSprite.setScale(
                    static_cast<float>(boardWidth) / gridTexture.getSize().x,
                    static_cast<float>(boardWidth) / gridTexture.getSize().y
                );
                window.draw(gridSprite);
            }

            // Draw candies
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    int posX = boardLeft + i * squareSize;
                    int posY = boardTop + j * squareSize;

                    if (state.board[i][j] != -1) {
                        int candyType = state.board[i][j];
                        Sprite candy(candyTextures[candyType]);
                        candy.setPosition(posX, posY);
                        candy.setScale(
                            static_cast<float>(squareSize) / candyTextures[candyType].getSize().x,
                            static_cast<float>(squareSize) / candyTextures[candyType].getSize().y
                        );

                        // Fade matched candies
                        if (state.is_matching) {
                            for (int idx = 0; idx < state.match_count; idx++) {
                                if (state.match_rows[idx] == i && state.match_cols[idx] == j) {
                                    if (candyType != 6) {
                                        candy.setColor(Color(255, 255, 255, state.fade_alpha));
                                    }
                                    break;
                                }
                            }
                        }
                        window.draw(candy);
                    }

                    // Highlight selected candy
                    if (i == state.selected_i && j == state.selected_j && state.clicks == 1) {
                        RectangleShape highlight(Vector2f(squareSize, squareSize));
                        highlight.setFillColor(Color(255, 255, 255, 100));
                        highlight.setPosition(posX, posY);
                        window.draw(highlight);
                    }
                }
            }

            // Draw hint boxes
            if (state.show_hint) {
                // Draw purple outline around first hint candy
                RectangleShape hint1(Vector2f(squareSize - 4, squareSize - 4));
                hint1.setPosition(boardLeft + state.hint_i1 * squareSize + 2,
                    boardTop + state.hint_j1 * squareSize + 2);
                hint1.setFillColor(Color::Transparent);
                hint1.setOutlineThickness(4);
                hint1.setOutlineColor(Color(255, 0, 255)); // Purple color
                window.draw(hint1);

                // Draw purple outline around second hint candy
                RectangleShape hint2(Vector2f(squareSize - 4, squareSize - 4));
                hint2.setPosition(boardLeft + state.hint_i2 * squareSize + 2,
                    boardTop + state.hint_j2 * squareSize + 2);
                hint2.setFillColor(Color::Transparent);
                hint2.setOutlineThickness(4);
                hint2.setOutlineColor(Color(255, 0, 255)); // Purple color
                window.draw(hint2);
            }

            // Draw UI panel
            RectangleShape panel(Vector2f(400, 300));
            panel.setFillColor(Color(0, 0, 0, 180));
            panel.setPosition(50, 50);
            window.draw(panel);

            // Calculate game time
            Time game_time = state.accumulated_time;
            if (!state.paused) {
                game_time += state.game_clock.getElapsedTime();
            }
            int timeLeft = MAX_TIME - static_cast<int>(game_time.asSeconds());
            if (timeLeft < 0) timeLeft = 0;

            // Draw UI text
            Text scoreText;
            scoreText.setFont(font);
            scoreText.setCharacterSize(40);
            scoreText.setFillColor(Color::White);
            scoreText.setString("SCORE: " + to_string(state.score));
            scoreText.setPosition(70, 70);
            window.draw(scoreText);

            Text movesText;
            movesText.setFont(font);
            movesText.setCharacterSize(40);
            movesText.setFillColor(Color::White);
            movesText.setString("MOVES: " + to_string(state.moves) + "/" + to_string(state.max_moves));
            movesText.setPosition(70, 120);
            window.draw(movesText);

            Text timeText;
            timeText.setFont(font);
            timeText.setCharacterSize(40);
            timeText.setFillColor(Color::White);
            timeText.setString("TIME: " + to_string(timeLeft) + "s");
            timeText.setPosition(70, 170);
            window.draw(timeText);

            Text controls;
            controls.setFont(font);
            controls.setCharacterSize(30);
            controls.setFillColor(Color::Yellow);
            controls.setString("H: Hint  S: Save  L: Load\nP: Pause  R: Restart");
            controls.setPosition(70, 230);
            window.draw(controls);

            // Pause screen
            if (state.paused) {
                RectangleShape overlay(Vector2f(1920, 1080));
                overlay.setFillColor(Color(0, 0, 0, 150));
                window.draw(overlay);

                Text pausedText;
                pausedText.setFont(font);
                pausedText.setCharacterSize(100);
                pausedText.setFillColor(Color::Yellow);
                pausedText.setString("PAUSED");
                pausedText.setPosition(1920 / 2 - pausedText.getLocalBounds().width / 2, 1080 / 2 - 100);
                window.draw(pausedText);
            }
        }

        window.display();
    }

    return 0;
}