#include <cstdlib>
#include <io.hpp>
#include <print>

int32_t io::read_int32(std::istream& from, std::string& buffer) {
    while (true) {
        std::print(">>> ");
        std::getline(from, buffer);

        // Exit if Ctrl+D (unix-like) or Ctrl+Z (DOS) was pressed
        if (from.eof()) {
            std::exit(1);
        }

        try {
            return std::stoi(buffer);
        } catch (std::exception& _) {
            std::println("Invalid value, please try again.");
            continue;
        }
    }
}

int32_t io::read_int32_in_range(std::istream& from, std::string& buffer,
                                int32_t from_inc, int32_t to_inc) {
    while (true) {
        int32_t value = read_int32(from, buffer);
        if (value < from_inc || value > to_inc) {
            std::println("The provided value is outside the required range: "
                         "from {} inclusive to {} inclusive",
                         from_inc, to_inc);
            continue;
        }

        return value;
    }
}
