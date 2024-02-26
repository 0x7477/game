#include <network/client_info.hpp>

void network::ClientInfo::append(const std::string_view &data)
{
    packets.push_back({data});
    list_byte_size += data.size();
}
auto network::ClientInfo::getView() const
{
    auto packet_to_subview = [](const std::string_view &packet)
    {
        return packet;
    };

    return packets | std::views::transform(packet_to_subview) | std::views::join;
}

void network::ClientInfo::removeBytes(const std::size_t &number_of_bytes)
{
    std::size_t remaining_bytes_to_delete{number_of_bytes};
    list_byte_size -= number_of_bytes;
    while (remaining_bytes_to_delete > 0)
    {
        if (packets.front().view.size() <= remaining_bytes_to_delete)
        {
            remaining_bytes_to_delete -= packets.front().view.size();
            packets.pop_front();
        }
        else
        {
            packets.front().view.remove_prefix(remaining_bytes_to_delete);
            remaining_bytes_to_delete = 0;
        }
    }
}

std::optional<std::string> network::ClientInfo::createMessage()
{
    const auto message_length = getMessageLength();
    if (!message_length)
        return {};
    if (*message_length > list_byte_size)
        return {};

    auto view = getView() | std::views::take(*message_length);

    std::string result;
    result.reserve(*message_length);

    for (const char &c : view)
        result.push_back(c);

    removeBytes(*message_length);

    return {result};
}

std::optional<network::message_length_t> network::ClientInfo::getMessageLength()
{
    if (list_byte_size < (sizeof(message_length_t)))
        return {};

    char buffer[sizeof(message_length_t)];
    auto view{getView().begin()};
    for (auto i{0u}; i < sizeof(message_length_t); i++)
    {
        buffer[i] = *view;
        view++;
    }

    return *(message_length_t *)buffer;
}