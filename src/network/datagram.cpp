#include <network/datagram.hpp>

network::SerializedDatagram::SerializedDatagram(const std::string_view &data)
    : string{data} {}

network::SerializedDatagram::operator std::string() const
{
    return string;
}

network::SerializedDatagram::operator std::string_view() const
{
    return {string};
}

std::size_t network::SerializedDatagram::size() const
{
    return string.size();
}

const char *network::SerializedDatagram::data() const
{
    return string.data();
}

void network::deserialize_element(std::string_view &buffer, char *&string)
{
    const vector_length_t length = *(vector_length_t *)buffer.data();
    string = new char[length];
    std::memcpy(string, buffer.data(), length);
    buffer.remove_prefix(length + sizeof(vector_length_t));
}

void network::deserialize_element(std::string_view &buffer, std::string &string)
{
    const vector_length_t length = *(vector_length_t *)buffer.data();
    buffer.remove_prefix(sizeof(vector_length_t));
    string = std::string{buffer.data(), (std::size_t)length};
    buffer.remove_prefix(length);
}

void network::serialize_element(std::vector<char> &buffer, const char *string)
{
    assert(strlen(string) <= std::numeric_limits<vector_length_t>::max());
    const vector_length_t length = (vector_length_t)strlen(string);
    buffer.push_back(length);

    for (vector_length_t i{0u}; i < length; i++)
        buffer.push_back(string[i]);
}

void network::serialize_element(std::vector<char> &buffer, const std::string &string)
{
    assert(string.size() <= std::numeric_limits<vector_length_t>::max());
    const vector_length_t length = (vector_length_t)string.size();
    serialize_element(buffer, length);

    for (vector_length_t i{0u}; i < length; i++)
        serialize_element(buffer, string[i]);
}
