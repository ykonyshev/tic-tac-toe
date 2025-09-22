#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <ostream>
#include <print>
#include <random>
#include <string>

#include <board.hpp>
#include <io.hpp>

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
    board.print();

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

        board.print();

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

static std::random_device RANDOM_DEVICE;
static std::mt19937 RNG(RANDOM_DEVICE());

Board::Index random_bot(Board& board, Field /*plays_as*/) {
    std::uniform_int_distribution<int32_t> range(
        0, (int32_t)(board.m_empty_fields.size() - 1));
    Board::Index random_index = range(RNG);

    auto it = board.m_empty_fields.begin();
    std::advance(it, random_index);
    return *it;
}

GameState get_win_state_for_field(Field for_) {
    return for_ == Field::PLAYER_X ? GameState::PLAYER_X_WON
                                   : GameState::PLAYER_O_WON;
}

Board::Index normal_bot(Board& board, Field plays_as) {
    GameState plays_as_win_state = get_win_state_for_field(plays_as);
    Field opponent = plays_as == Field::PLAYER_X ? Field::PLAYER_O
                                                 : Field::PLAYER_X;
    GameState opponent_win_state = get_win_state_for_field(opponent);

    // If there is only one move to make, make that move
    if (board.m_empty_fields.size() == 1) {
        return *board.m_empty_fields.begin();
    }

    // Attempting to find a move where the game is one in one move
    for (Board::Index empty_index : board.m_empty_fields) {
        board.m_state[empty_index] = plays_as;
        if (board.get_game_state() == plays_as_win_state) {
            return empty_index;
        }

        board.m_state[empty_index] = Field::EMPTY;
    }

    // Making sure that the opponent does not win in one move
    for (Board::Index empty_index : board.m_empty_fields) {
        board.m_state[empty_index] = opponent;
        if (board.get_game_state() == opponent_win_state) {
            return empty_index;
        }

        board.m_state[empty_index] = Field::EMPTY;
    }

    // If none of the heursitics could be applied, just make a random move
    return random_bot(board, plays_as);
}

Board::Index hard_bot(Board& board, Field plays_as) {
    // TODO: Implement
    return 0;
}

using BotFunction = uint8_t (*)(Board&, Field);

struct BotDifficulty {
    const char* description;
    BotFunction func;
};

const auto DIFFICULTY = std::to_array<BotDifficulty>({
    {.description = "Easy", .func = random_bot},
    {.description = "Normal", .func = normal_bot},
    {.description = "Hard", .func = hard_bot},
});

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
    board.print();

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

        board.print();

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

struct PlayMode {
    void (*func)(std::string&);
    const char* description;
};

static const auto MODES = std::to_array<PlayMode>(
    {{.func = play_against_person,
      .description = "Play against another person."},
     {.func = play_against_bot, .description = "Play against a bot."}});

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
