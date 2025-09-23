#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <set>

// The values for the `Field` of a given player and the corresponding player's
// win `GameState` must be the same to enabled casting between one another
enum class GameState : int8_t {
    PLAYING = 3,
    PLAYER_X_WON = -1,
    PLAYER_O_WON = 1,
    TIE = 0,
};

enum class Field : int8_t {
    EMPTY = 0,
    PLAYER_X = -1,
    PLAYER_O = 1,
};

std::ostream& operator<<(std::ostream& out, const Field& value);

struct Board {
public:
    using Index = uint8_t;

    constexpr static Index DIM = 3;
    constexpr static Index SIZE = DIM * DIM;
    constexpr static Index NONE_INDEX = SIZE;

    std::set<Index> m_empty_fields;

    Board();

    const Field& operator[](Index index) const { return m_state[index]; };
    void make_move(Index index, Field new_value);

    friend Index normal_bot(Board& board, Field plays_as);
    friend Index hard_bot(Board& board, Field plays_as);

    friend std::ostream& operator<<(std::ostream& out, const Board& object);
    GameState get_game_state();
    bool is_winner(Field player_field);

private:
    std::array<Field, SIZE> m_state;
};
