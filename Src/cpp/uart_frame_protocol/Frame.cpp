//
// Created by elkhay01 on 4/9/23.
//

#include "cpp/uart_frame_protocol/Frame.h"
#include <iostream>
Frame::Frame(const std::string& data)
{
    // Start of frame
    m_bytes.push_back(0xab);
    m_bytes.push_back(0xcd);

    // Length of data
    size_t dataLength = data.length();
    m_bytes.push_back(dataLength & 0xFF);

    // Data payload
    for (size_t i = 0; i < dataLength; i++) {
        m_bytes.push_back(data[i]);
    }

    // End of frame
    m_bytes.push_back(0xe1);
    m_bytes.push_back(0xe2);
}

std::vector<uint8_t> Frame::getBytes() const
{
    return m_bytes;
}