#ifndef TONE_S
#define TONE_S
#include <cinttypes>
struct tone_t{
    float freq;
    std::uint16_t dur;
};

#endif // TONE_S

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <vector>
#include <cstdlib>


class FileParser
{
public:
    inline FileParser(const char* file_path) : file_path_((char *)file_path) {};
    ~FileParser();
    std::vector<std::vector<tone_t>> parse();
private:
    char *map_file(size_t& length);
    char *mapped_loc_ = nullptr;
    size_t length_;
    char *file_path_;

    bool moved = false;
};

#endif // FILEPARSER_H
