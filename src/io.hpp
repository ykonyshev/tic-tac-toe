#include <cstdint>
#include <iostream>

namespace io {
int32_t read_int32(std::istream& from, std::string& buffer);
int32_t read_int32_in_range(std::istream& from, std::string& buffer,
                            int32_t from_inc, int32_t to_inc);
} // namespace io
