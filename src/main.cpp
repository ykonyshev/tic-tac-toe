#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

enum class Field : uint8_t {
    PLAYER_X,
    PLAYER_O,
    EMPTY,
};

enum class GameState : uint8_t {
    PLAYING,
    PLAYER_X_WON,
    PLAYER_O_WON,
    TIE,
};

static std::ostream& operator<<(std::ostream& out, const Field& value) {
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

struct Board {
public:
    const static size_t DIM = 3;

    Board() : m_state(std::array<Field, DIM * DIM>()) {
        m_state.fill(Field::EMPTY);
    }

    Field& operator[](size_t index) { return m_state[index]; };

    void print() const {
        std::cout << '\n';
        for (size_t row = 0; row < DIM; ++row) {
            for (size_t col = 0; col < DIM; ++col) {
                size_t index = (row * DIM) + col;
                const Field value = m_state[index];
                if (col == 0) {
                    std::cout << " ";
                } else {
                    std::cout << " | ";
                }

                if (value != Field::EMPTY) {
                    std::cout << value;
                } else {
                    std::cout << index;
                }
            }

            std::cout << '\n';

            if (row < DIM - 1) {
                for (size_t i = 0; i < DIM - 1; ++i) {
                    std::cout << "----";
                }

                std::cout << "---" << '\n';
            }
        }

        std::cout << '\n';
    }

    GameState get_game_state() {
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

    bool is_winner(Field player_field) {
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

private:
    std::array<Field, DIM * DIM> m_state;
};

void two_player_game() {
    std::cout << "Hello to the game of tic-tac-toe!" << '\n';

    Board board;
    board.print();

    bool is_x_turn = true;
    std::string read_buffer;
    while (true) {
        if (is_x_turn) {
            std::cout << "Player X, make your move." << '\n';
        } else {
            std::cout << "Player O, make your move." << '\n';
        }

        int move_index = -1;
        while (true) {
            std::cout << ">>> " << std::flush;
            std::getline(std::cin, read_buffer);
            try {
                move_index = std::stoi(read_buffer);
            } catch (std::exception& _) {
                std::cout << "Invalid value, please try again." << '\n';
                continue;
            }

            if (move_index < 0 ||
                (size_t)move_index >= Board::DIM * Board::DIM) {
                std::cout << "Invalid move index, please try entering a "
                             "different value."
                          << '\n';
                continue;
            }

            if (board[move_index] != Field::EMPTY) {
                std::cout
                    << "Invalid move index, the field is already occupied."
                    << '\n';
                continue;
            }

            break;
        }

        if (is_x_turn) {
            board[move_index] = Field::PLAYER_X;
        } else {
            board[move_index] = Field::PLAYER_O;
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

int main() {
    two_player_game();

    return EXIT_SUCCESS;
}
