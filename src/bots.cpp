#include <print>
#include <random>

#include <board.hpp>
#include <bots.hpp>
#include <timer.hpp>

namespace {
static std::random_device RANDOM_DEVICE;
std::mt19937 RNG(RANDOM_DEVICE());

inline GameState get_win_state_for_field(Field for_) { return (GameState)for_; }

inline Field get_opponent(Field for_) {
    return for_ == Field::PLAYER_X ? Field::PLAYER_O : Field::PLAYER_X;
}
} // namespace

Board::Index random_bot(Board& board, Field /*plays_as*/) {
    std::uniform_int_distribution<int32_t> range(
        0, (int32_t)(board.m_empty_fields.size() - 1));
    Board::Index random_index = range(RNG);

    auto it = board.m_empty_fields.begin();
    std::advance(it, random_index);
    return *it;
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

namespace {
struct ScoredMove {
    int8_t score;
    Board::Index move;

    friend bool operator<(const ScoredMove& lhs, const ScoredMove& rhs) {
        return lhs.score < rhs.score;
    }
};

// https://en.wikipedia.org/wiki/Minimax
ScoredMove minimax(Board& board, Field current_player, Field opponent,
                   Field maximizing_player) {
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

        return ScoredMove{
            .score = score,
            .move = Board::NONE_INDEX,
        };
    }

    std::vector<ScoredMove> move_scores;
    move_scores.reserve(board.m_empty_fields.size());

    for (Board::Index valid_move : board.m_empty_fields) {
        Board board_copy = board;
        board_copy.make_move(valid_move, current_player);

        ScoredMove from_this_move_scored =
            minimax(board_copy, opponent, current_player, maximizing_player);

        move_scores.push_back(ScoredMove{.score = from_this_move_scored.score,
                                         .move = valid_move});
    }

    // Returning the move that yields the highest score for the current player
    if (maximizing_player == current_player) {
        ScoredMove max_score_move =
            *std::max_element(move_scores.begin(), move_scores.end());
        return max_score_move;
    }

    // Returning the move that yields the lowest score for the opponent
    ScoredMove min_score_move =
        *std::min_element(move_scores.begin(), move_scores.end());
    return min_score_move;
}
} // namespace

Board::Index hard_bot(Board& board, Field plays_as) {
    Board board_copy = board;
    Field opponent = get_opponent(plays_as);

    Timer timer;
    timer.start();
    ScoredMove best_move_pair =
        minimax(board_copy, plays_as, opponent, plays_as);
    double elapsed_milllis = timer.end();

    std::println("Took {:.2f} ms to compute the next move.", elapsed_milllis);

    return best_move_pair.move;
}
