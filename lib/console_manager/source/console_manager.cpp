#include "console_manager.h"

#include <log.h>

#include <iostream>
#include <string_view>
#include <algorithm>

// Platform-specific includes for input handling
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#endif

// #define DISABLE_CONSOLE_CLEAR

constexpr char kEscKey = 27; // ESC key
constexpr char kExtendedKeyArrowUp = 72;
constexpr char kExtendedKeyArrowDown = 80;
constexpr char kExtendedKeyArrowLeft = 75;
constexpr char kExtendedKeyArrowRight = 77;

class ConsoleManagerImpl : public IConsoleManager {
public:
    ConsoleManagerImpl() {
        LOG_D("Console Manager created\n");
    }

    ~ConsoleManagerImpl() override {
        LOG_D("Console Manager destroyed\n");
    }

    int getKey() {
#ifdef _WIN32
        return _getch();
#else
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
#endif
    }

    void clearConsole() override {
    #ifndef DISABLE_CONSOLE_CLEAR
        // ANSI escape code for clearing screen
        std::cout << "\033[2J\033[1;1H";
    #endif
    }

    void pauseConsole() override {
        std::cout << "Press any key to continue..." << std::endl;
        getKey();
    }

    void printBoard(const Board::BoardType& board) override {
        constexpr std::string_view kCellContent =       " ";
        constexpr std::string_view kColumnSeparator =   "||";
        constexpr std::string_view kEdgeSeparator =     "====";
        constexpr std::string_view kMiddleSeparator =   "=====";

        for (const auto &row : board) {
            for (const auto &cell : row) {
                const auto cell_char = Board::convertBoardFieldToChar(cell);
                std::cout << kCellContent << cell_char << kCellContent;
                if (&cell != &row.back()) {
                    std::cout << kColumnSeparator;
                }
            }
            if (&row != &board.back()) {
                std::cout << std::endl << kEdgeSeparator;
                for (size_t sep = 0; sep < board.size() - 2; ++sep) {
                    std::cout << kMiddleSeparator;
                }
                std::cout << kEdgeSeparator;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }

    void printGameStats(int host_score, int guest_score, size_t round) override {
        std::cout << "Round: " << round << " Host Score: " << host_score
                  << " Guest Score: " << guest_score << std::endl;
    }

    void printRoundEndMessage(const Board::BoardType& board, RoundResult result, size_t round) override {
        clearConsole();
        printBoard(board);
        std::cout << "Round " << round << " ended. Result: ";
        switch (result) {
            case RoundResult::HostWin:
                std::cout << "Host wins!\n";
                break;
            case RoundResult::GuestWin:
                std::cout << "Guest wins!\n";
                break;
            case RoundResult::Draw:
                std::cout << "It's a draw!\n";
                break;
            default:
                std::cout << "Unknown result!\n";
                break;
        }
        pauseConsole();
    }

    void gameEndMessage() override {

        std::cout << "\nThank you for game!" << std::endl;
        pauseConsole();
        std::cout << "Goodbye!\n";
    }

    std::pair<int, int> getPlayerMove(const Board::BoardType& board) override {
        Board::Board board_copy {board};
        constexpr int kCellWidth = 5;
        constexpr int kCellHeight = 2;
        constexpr char kPlayerSymbol = 'X';
        const int kBoardSize = static_cast<int>(board.size());

        int current_col = 0;
        int current_row = 0;

        printBoard(board);
        std::cout << "\nInstructions: Use W/A/S/D to move, Enter to confirm\n"
                  << "Press ESC to exit" << std::endl;

        auto move_up = [](int &row) {
            row = (std::max)(row - 1, 0);
        };
        auto move_down = [](int &row, int max_row) {
            row = (std::min)(row + 1, max_row - 1);
        };
        auto move_left = [](int &col) {
            col = (std::max)(col - 1, 0);
        };
        auto move_right = [](int &col, int max_col) {
            col = (std::min)(col + 1, max_col - 1);
        };

        while (true) {
            // Calculate cursor position using ANSI escape codes
            int cursor_x = current_col * kCellWidth + 1;
            int cursor_y = current_row * kCellHeight + 1;
            std::cout << "\033[" << (cursor_y + 1) << ";" << (cursor_x + 1) << "H" << std::flush;

            // Handle input with cross-platform key detection
            int input = getKey();
            int extended_input = 0;

            // Process escape sequences for arrow keys
            if (input == 27) { // ESC character
                int next = getKey();
                if (next == '[') { // CSI sequence
                    int code = getKey();
                    switch (code) {
                        case 'A': extended_input = kExtendedKeyArrowUp; break;
                        case 'B': extended_input = kExtendedKeyArrowDown; break;
                        case 'C': extended_input = kExtendedKeyArrowRight; break;
                        case 'D': extended_input = kExtendedKeyArrowLeft; break;
                        default: break;
                    }
                    input = 0xE0; // Simulate Windows extended key
                }
            } else if (input == 0x00 || input == 0xE0) { // Extended key prefix
                extended_input = getKey();
            }

            // Calculate message position
            int message_y = kCellHeight * kBoardSize + 2;
            std::cout << "\033[" << (message_y + 1) << ";1H" << std::flush;

            switch (std::tolower(input)) {
                case kEscKey: // ESC key
                if (extended_input == 0) {
                    std::cout << "Game interrupted by user. Exiting...\n";
                    throw std::runtime_error("Game interrupted by user.");
                }
                [[fallthrough]];
                case 0x00: // Extended key
                case 0xE0: // Extended key
                    switch (extended_input) {
                        case kExtendedKeyArrowUp:
                            move_up(current_row);
                            break;
                        case kExtendedKeyArrowDown:
                            move_down(current_row, kBoardSize);
                            break;
                        case kExtendedKeyArrowLeft:
                            move_left(current_col);
                            break;
                        case kExtendedKeyArrowRight:
                            move_right(current_col, kBoardSize);
                            break;
                        default:
                            std::cout << "Unknown command. Use W/A/S/D to move.        ";
                            break;
                    }
                    break;
                case 'w':
                    move_up(current_row);
                    break;
                case 'a':
                    move_left(current_col);
                    break;
                case 's':
                    move_down(current_row, kBoardSize);
                    break;
                case 'd':
                    move_right(current_col, kBoardSize);
                    break;
                    case '\n':  // Linux support
                    case '\r': { // Enter key
                    if (board_copy.is_valid_move(current_row, current_col)) {
                        std::cout << "\033[" << (cursor_y + 1) << ";" << (cursor_x + 1)
                                  << "H" << kPlayerSymbol << std::flush;
                        std::cout << "\033[" << (message_y + 1) << ";1H" << "Move confirmed.                              " << std::flush;
                        return std::make_pair(current_row, current_col);
                    }
                    std::cout << "Position occupied! Choose another location.    ";
                    break;
                }
                default:
                    std::cout << "Unknown command. Use W/A/S/D to move.        ";
                    break;
            }
        }
    }

};

ConsoleManager::ConsoleManager() :
    impl_(std::make_unique<ConsoleManagerImpl>()) {}
