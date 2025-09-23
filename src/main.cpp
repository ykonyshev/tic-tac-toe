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
#include <timer.hpp>
#include <unordered_map>

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
    std::println("Select the difficulty:");
    for (size_t i = 0; i < BOT_DIFFICULTIES.size(); ++i) {
        std::cout << i + 1 << ". " << BOT_DIFFICULTIES[i].description << '\n';
    }

    int32_t difficulty_selection =
        io::read_int32_in_range(std::cin, buffer, 1, BOT_DIFFICULTIES.size());
    BotDifficulty difficulty = BOT_DIFFICULTIES[difficulty_selection - 1];

    BotFunction bot_function = difficulty.func;

    Board board;
    std::cout << board;

    bool is_x_turn = true;
    while (true) {
        if (is_x_turn) {
            std::cout << "Player X, make your move." << '\n';
            Board::Index move_index = get_move_index(board, buffer);
            board.make_move(move_index, Field::PLAYER_X);
        } else {
            Timer timer;
            timer.start();
            Board::Index move_index = bot_function(board, Field::PLAYER_O);
            double elapsed_milllis = timer.end();

            std::println("Took {:.2f} ms to compute the next move.",
                         elapsed_milllis);

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

GameState simulate_game(BotFunction x_bot, BotFunction o_bot) {
    Board board;

    bool is_x_turn = true;
    while (true) {
        if (is_x_turn) {
            Board::Index move_index = x_bot(board, Field::PLAYER_O);
            board.make_move(move_index, Field::PLAYER_X);
        } else {
            Board::Index move_index = o_bot(board, Field::PLAYER_O);
            board.make_move(move_index, Field::PLAYER_O);
        }

        GameState game_state = board.get_game_state();

        if (game_state != GameState::PLAYING) {
            return game_state;
        }

        is_x_turn = !is_x_turn;
    }
}

const size_t SIMULATED_GAMES_COUNT = 1'000;

void simulate_bot_games(std::string& /*buffer*/) {
    for (BotDifficulty bot_difficulty : BOT_DIFFICULTIES) {
        std::println("{} bot (X) vs random bot (O)",
                     bot_difficulty.description);
        std::unordered_map<GameState, int32_t> outcomes{
            {GameState::PLAYER_X_WON, 0},
            {GameState::PLAYER_O_WON, 0},
            {GameState::TIE, 0},
        };

        for (size_t i = 0; i < SIMULATED_GAMES_COUNT; ++i) {
            GameState outcome = simulate_game(bot_difficulty.func, random_bot);
            outcomes[outcome]++;
        }

        std::println("The outcomes for {} game(s):", SIMULATED_GAMES_COUNT);
        std::println("\tX won: {}", outcomes[GameState::PLAYER_X_WON]);
        std::println("\tO won: {}", outcomes[GameState::PLAYER_O_WON]);
        std::println("\tTies: {}", outcomes[GameState::TIE]);

        std::println();
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
     {.func = play_against_bot, .description = "Play against a bot."},
     {.func = simulate_bot_games,
      .description = "Simulate a number of bot games and see the outcomes."}});

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
