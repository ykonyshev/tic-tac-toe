#pragma once

#include <board.hpp>

Board::Index random_bot(Board& board, Field /*plays_as*/);
Board::Index normal_bot(Board& board, Field plays_as);
Board::Index hard_bot(Board& board, Field plays_as);

using BotFunction = uint8_t (*)(Board&, Field);

struct BotDifficulty {
    const char* description;
    BotFunction func;
};

const auto BOT_DIFFICULTIES = std::to_array<BotDifficulty>({
    {.description = "Easy", .func = random_bot},
    {.description = "Normal", .func = normal_bot},
    {.description = "Hard", .func = hard_bot},
});
