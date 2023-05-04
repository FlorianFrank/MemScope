//
// Created by elkhay01 on 4/9/23.
//

#ifndef MEMORYCONTROLLER_ELF_FRAME_H
#define MEMORYCONTROLLER_ELF_FRAME_H

#include <string>
#include <vector>


class Frame {
public:
    Frame(const std::string& data);
    std::vector<uint8_t> getBytes() const;

private:
    std::vector<uint8_t> m_bytes;
};


#endif //MEMORYCONTROLLER_ELF_FRAME_H
