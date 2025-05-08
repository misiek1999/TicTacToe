#include "console_manager.h"

#include <log.h>

#include <iostream>
#include <string_view>
#include <algorithm>

// TODO: Remove windows specific includes and use cross-platform alternatives
#include <conio.h>
#include <windows.h>

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

    void clearConsole() override {
        #ifndef DISABLE_CONSOLE_CLEAR
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
        #endif
    }

    void pauseConsole() override {
        std::cout << "Press any key to continue..." << std::endl;
        #ifdef _WIN32
        std::ignore = _getch();
        #else
        std::cin.get();
        #endif
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
        using Coordinates = COORD;

        int current_col = 0;
        int current_row = 0;
        Coordinates cursor_pos{};
        Coordinates message_pos{0, static_cast<SHORT>(kCellHeight * kBoardSize + 2)};

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
            // Calculate cursor position
            cursor_pos.X = static_cast<SHORT>(current_col * kCellWidth + 1);
            cursor_pos.Y = static_cast<SHORT>(current_row * kCellHeight + 1);

            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor_pos);

            // Handle input
            const int input = _getch();
            int extended_input = {};
            if (input == 0 || input == 0xE0) {
                extended_input =  _getch(); // Handle extended keys
            }

            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), message_pos);

            switch (std::tolower(input)) {
                case kEscKey: // ESC key
                if (extended_input == 0) {
                    std::cout << "Game interrupted by user. Exiting...\n";
                    throw std::runtime_error("Game interrupted by user.");
                }
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
                case '\r': { // Enter key
                    if (board_copy.is_valid_move(current_row, current_col)) {
                        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor_pos);
                        std::cout << kPlayerSymbol;

                        // Clear message area
                        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), message_pos);
                        std::cout << "Move confirmed.                              ";
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
