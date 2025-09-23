#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <print>
#include <string>

#include <board.hpp>
#include <bots.hpp>
#include <io.hpp>

namespace {
Board::Index get_move_index(const Board& board, std::string& buffer) {
    while (true) {
        auto move_index = (Board::Index)io::read_int32_in_range(
            std::cin, buffer, 0, Board::SIZE - 1);

        if (board[move_index] != Field::EMPTY) {
            std::cout << "Invalid move index, the field at the index is "
                         "already occupied."
                      << '\n';
            continue;
        }

        return move_index;
    }
}

void play_against_person(std::string& buffer) {
    Board board;
    std::cout << board;

    bool is_x_turn = true;
    while (true) {
        if (is_x_turn) {
            std::cout << "Player X, make your move." << '\n';
        } else {
            std::cout << "Player O, make your move." << '\n';
        }

        int32_t move_index = get_move_index(board, buffer);
        if (is_x_turn) {
            board.make_move(move_index, Field::PLAYER_X);
        } else {
            board.make_move(move_index, Field::PLAYER_O);
        }

        std::cout << board;

        GameState game_state = board.get_game_state();
        switch (game_state) {
        case GameState::PLAYER_X_WON:
            std::cout << "Player X won!" << '\n';

            break;
        case GameState::PLAYER_O_WON:
            std::cout << "Player O won!" << '\n';

            break;
        case GameState::TIE:
            std::cout << "Tie!" << '\n';

            break;
        case GameState::PLAYING:
            break;
        }

        if (game_state != GameState::PLAYING) {
            break;
        }

        is_x_turn = !is_x_turn;
    }
}

// TODO: An option for the player to choose who to play as
void play_against_bot(std::string& buffer) {
    std::println("Select the diffuctly:");
    for (size_t i = 0; i < DIFFICULTY.size(); ++i) {
        std::cout << i + 1 << ". " << DIFFICULTY[i].description << '\n';
    }

    int32_t difficulty_selection =
        io::read_int32_in_range(std::cin, buffer, 1, DIFFICULTY.size());
    BotDifficulty diffuctly = DIFFICULTY[difficulty_selection - 1];

    BotFunction bot_function = diffuctly.func;

    Board board;
    std::cout << board;

    bool is_x_turn = true;
    while (true) {
        if (is_x_turn) {
            std::cout << "Player X, make your move." << '\n';
            Board::Index move_index = get_move_index(board, buffer);
            board.make_move(move_index, Field::PLAYER_X);
        } else {
            Board::Index move_index = bot_function(board, Field::PLAYER_O);
            board.make_move(move_index, Field::PLAYER_O);
        }

        std::cout << board;

        GameState game_state = board.get_game_state();
        switch (game_state) {
        case GameState::PLAYER_X_WON:
            std::cout << "Player X won!" << '\n';

            break;
        case GameState::PLAYER_O_WON:
            std::cout << "Player O won!" << '\n';

            break;
        case GameState::TIE:
            std::cout << "Tie!" << '\n';

            break;
        case GameState::PLAYING:
            break;
        }

        if (game_state != GameState::PLAYING) {
            break;
        }

        is_x_turn = !is_x_turn;
    }
}
} // namespace

struct PlayMode {
    void (*func)(std::string&);
    const char* description;
};

static const auto MODES = std::to_array<PlayMode>(
    {{.func = play_against_person,
      .description = "Play against another person."},
     {.func = play_against_bot, .description = "Play against a bot."}});

// TODO: Multiplayer over the network
// TODO: Refactor all of the `std::cout` usages to `std::print` or
// `std::println`
int main() {
    std::string buffer;

    std::cout << "Hello to the game of tic-tac-toe!" << '\n';
    std::cout << "Select a desired playmode from the list below:" << '\n';
    for (size_t i = 0; i < MODES.size(); ++i) {
        std::cout << i + 1 << ". " << MODES[i].description << '\n';
    }

    int32_t mode_index =
        io::read_int32_in_range(std::cin, buffer, 1, MODES.size()) - 1;
    MODES[mode_index].func(buffer);

    return EXIT_SUCCESS;
}
