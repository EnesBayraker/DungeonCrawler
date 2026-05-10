#pragma once

#include <cstddef>
#include <string>
#include <vector>

class MessageLog
{
public:
    void add(const std::string& message);
    const std::vector<std::string>& getMessages() const;

private:
    static constexpr std::size_t MaxMessages = 5;

    std::vector<std::string> m_messages;
};