#include <board.hpp>
#include <iostream>

std::ostream& operator<<(std::ostream& out, const Field& value) {
    switch (value) {
    case Field::PLAYER_X:
        out << "X";
        break;
    case Field::PLAYER_O:
        out << "O";
        break;
    case Field::EMPTY:
        out << "-";
        break;
    }

    return out;
}

void Board::make_move(Board::Index index, Field new_value) {
    m_state[index] = new_value;
    m_empty_fields.erase(index);
}

Board::Board() : m_state(std::array<Field, SIZE>()) {
    m_state.fill(Field::EMPTY);

    for (Board::Index i = 0; i < SIZE; ++i) {
        m_empty_fields.insert(i);
    }
}

std::ostream& operator<<(std::ostream& out, const Board& object) {
    out << '\n';
    for (size_t row = 0; row < Board::DIM; ++row) {
        for (size_t col = 0; col < Board::DIM; ++col) {
            size_t index = (row * Board::DIM) + col;
            const Field value = object.m_state[index];
            if (col == 0) {
                out << " ";
            } else {
                out << " | ";
            }

            if (value != Field::EMPTY) {
                out << value;
            } else {
                out << index;
            }
        }

        out << '\n';

        if (row < Board::DIM - 1) {
            for (size_t i = 0; i < Board::DIM - 1; ++i) {
                out << "----";
            }

            out << "---" << '\n';
        }
    }

    out << '\n';
    return out;
}

GameState Board::get_game_state() {
    if (is_winner(Field::PLAYER_X)) {
        return GameState::PLAYER_X_WON;
    }

    if (is_winner(Field::PLAYER_O)) {
        return GameState::PLAYER_O_WON;
    }

    auto* it = std::ranges::find(m_state, Field::EMPTY);
    if (it == m_state.end()) {
        return GameState::TIE;
    }

    return GameState::PLAYING;
}

bool Board::is_winner(Field player_field) {
    return
        // Horizontals
        (m_state[0] == m_state[1] && m_state[1] == m_state[2] &&
         m_state[2] == player_field) ||
        (m_state[3] == m_state[4] && m_state[4] == m_state[5] &&
         m_state[5] == player_field) ||
        (m_state[6] == m_state[7] && m_state[7] == m_state[8] &&
         m_state[8] == player_field)

        // Verticals
        || (m_state[0] == m_state[3] && m_state[3] == m_state[6] &&
            m_state[6] == player_field) ||
        (m_state[1] == m_state[4] && m_state[4] == m_state[7] &&
         m_state[7] == player_field) ||
        (m_state[2] == m_state[5] && m_state[5] == m_state[8] &&
         m_state[8] == player_field)

        // Diagonals
        || (m_state[0] == m_state[4] && m_state[4] == m_state[8] &&
            m_state[8] == player_field) ||
        (m_state[2] == m_state[4] && m_state[4] == m_state[6] &&
         m_state[6] == player_field);
}
