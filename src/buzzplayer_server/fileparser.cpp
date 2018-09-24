#include "fileparser.h"

#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <cstring>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdexcept>


FileParser::~FileParser()
{
    if(mapped_loc_ != nullptr)
        munmap(mapped_loc_, length_);
}

std::vector<std::vector<tone_t> > FileParser::parse()
{
    if(moved) throw std::invalid_argument("Unable to call parse() twice");
    std::vector<std::vector<tone_t> > tones;
    mapped_loc_ = map_file(length_);
    auto f = mapped_loc_;
    auto l = f + length_;
    char *end_l, *end_block;
    size_t line = 0;
    while (f && f<l){
        if ((end_l = static_cast<char*>(memchr(f, '\n', l-f)))){
            tones.push_back(std::vector<tone_t>());
            do{
            end_block = static_cast<char*>(memchr(f, ';', l-f));
            size_t len;
            float freq; int dur;
            freq = std::stof(f, &len);
            f = f+len+1;
            dur = std::stoi(f, &len);
            f = end_block + 1;
            tones[line].push_back(tone_t{freq, (std::uint16_t)dur});
            } while(end_block != NULL && end_block+1 < end_l);
            f = end_l;
            line++, f++;
        }
    }
    moved = true;
    return std::move(tones);
}

char *FileParser::map_file(size_t &length_)
{
    int fd = open(file_path_, O_RDONLY);
    if (fd == -1){
        throw std::invalid_argument("Unable to open file.");
    }

    // obtain file size
    struct stat sb;
    if (fstat(fd, &sb) == -1){
        close(fd);
        throw std::invalid_argument("Unable to obtain file size.");
    }

    length_ = sb.st_size;

    char* addr = static_cast<char*>(mmap(NULL, length_, PROT_READ, MAP_PRIVATE, fd, 0u));
    if (addr == MAP_FAILED){
        close(fd);
        throw std::invalid_argument("Unable to map file.");
    }
    close(fd);
    return addr;
}
