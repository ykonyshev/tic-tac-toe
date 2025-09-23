#include "timer.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <print>
#include <random>
#include <string>

#include <board.hpp>
#include <io.hpp>
#include <tuple>
#include <utility>

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

inline GameState get_win_state_for_field(Field for_) { return (GameState)for_; }

inline Field get_opponent(Field for_) {
    return for_ == Field::PLAYER_X ? Field::PLAYER_O : Field::PLAYER_X;
}

Board::Index normal_bot(Board& board, Field plays_as) {
    GameState plays_as_win_state = get_win_state_for_field(plays_as);
    Field opponent = get_opponent(plays_as);
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

struct ScorePair {
    int8_t score;
    Board::Index move;

    friend bool operator<(const ScorePair& lhs, const ScorePair& rhs) {
        return lhs.score < rhs.score;
    }
};

ScorePair minimax(Board& board, Field current_player, Field opponent,
                  Field maximizing_player, Field minimizing_player) {
    // Checking whether this is the end of this branch
    // A system of scores for each of the possible outcomes is defined:
    // A tie, meaning, neither player wins: 0
    // A win of the player that we are maximizing the scores for: 1
    // A loss of the player that we are maximizing the scores for: -1
    GameState state = board.get_game_state();
    if (state != GameState::PLAYING) {
        // Using a cast to an integer thanks to the value assigned to enum
        // variants to avoid comparison
        auto score = (int8_t)state;
        if (maximizing_player != Field::PLAYER_O) {
            // Need to flip the value if the assumption that the bot plays for
            // "O" is incorrect
            score *= -1;
        }

        return ScorePair{
            .score = score,
            .move = Board::NONE_INDEX,
        };
    }

    std::vector<ScorePair> move_scores;
    move_scores.reserve(board.m_empty_fields.size());

    for (Board::Index valid_move : board.m_empty_fields) {
        Board board_copy = board;
        board_copy.make_move(valid_move, current_player);

        ScorePair this_move_score_pair =
            minimax(board_copy, opponent, current_player, maximizing_player,
                    minimizing_player);

        move_scores.push_back(
            ScorePair{.score = this_move_score_pair.score, .move = valid_move});
    }

    // Returning the move that yields the highest score for the current player
    if (maximizing_player == current_player) {
        ScorePair max_score_move =
            *std::max_element(move_scores.begin(), move_scores.end());
        return max_score_move;
    }

    // Returning the move that yields the lowest score for the opponent
    ScorePair min_score_move =
        *std::min_element(move_scores.begin(), move_scores.end());
    return min_score_move;
}

Board::Index hard_bot(Board& board, Field plays_as) {
    Board board_copy = board;
    Field opponent = get_opponent(plays_as);

    Timer timer;
    timer.start();
    ScorePair best_move_pair =
        minimax(board_copy, plays_as, opponent, plays_as, opponent);
    double elapsed_milllis = timer.end();

    std::println("Took {:.3f} ms to compute the next move.", elapsed_milllis);

    return best_move_pair.move;
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
