#include "MessageLog.h"

void MessageLog::add(const std::string& message)
{
    if (m_messages.size() >= MaxMessages)
    {
        m_messages.erase(m_messages.begin());
    }

    m_messages.push_back(message);
}

const std::vector<std::string>& MessageLog::getMessages() const
{
    return m_messages;
}