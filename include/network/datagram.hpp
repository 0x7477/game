#pragma once
#include <tuple>
#include <type_traits>
#include <limits>
#include <cassert>
#include <span>
#include <cstring>
#include <iostream>
namespace network
{
    typedef uint16_t message_length_t;
    typedef uint16_t vector_length_t;

    class SerializedDatagram
    {
    public:
        explicit SerializedDatagram(const std::string_view &data) : string{data} {}
        operator std::string() const
        {
            return string;
        }

        operator std::string_view() const
        {
            return {string};
        }

        std::size_t size() const
        {
            return string.size();
        }

        const char *data() const
        {
            return string.data();
        }

    private:
        std::string string;
    };
    template <typename element>
    typename std::enable_if<std::is_trivially_copyable<element>::value>::type
    deserialize_element(std::string_view &buffer, const element &value)
    {
        std::memcpy((void *)&value, buffer.data(), sizeof(value));
        buffer.remove_prefix(sizeof(value));
    }

    void deserialize_element(std::string_view &buffer, char *&string)
    {
        const vector_length_t length = *(vector_length_t *)buffer.data();
        string = new char[length];
        std::memcpy(string, buffer.data(), length);
        buffer.remove_prefix(length + sizeof(vector_length_t));
    }

    void deserialize_element(std::string_view &buffer, std::string &string)
    {
        const vector_length_t length = *(vector_length_t *)buffer.data();
        buffer.remove_prefix(sizeof(vector_length_t));
        string = std::string{buffer.data(), (std::size_t)length};
        buffer.remove_prefix(length);
    }

    template <typename element>
    typename std::enable_if<std::is_array<element>::value>::type
    deserialize_element(std::string_view &buffer, const element &value)
    {
        using container_element = typename std::remove_extent<element>::type;
        
        const vector_length_t length = *(vector_length_t *)buffer.data();
        value.reserve(length);
        buffer.remove_prefix(sizeof(vector_length_t));

        for (vector_length_t i{0u}; i < length; i++)
        {
            container_element element_instance;
            deserialize_element(buffer, element_instance);
            value.push_back(element_instance);
        }
    }

    template <typename T>
    void deserialize_element(std::string_view &buffer, std::vector<T> &value)
    {
        const vector_length_t length = *(vector_length_t *)buffer.data();

        buffer.remove_prefix(sizeof(vector_length_t));

        for (vector_length_t i{0u}; i < length; i++)
        {
            T element_instance;
            deserialize_element(buffer, element_instance);
            value.push_back(element_instance);
        }
    }

    template <typename element>
    typename std::enable_if<std::is_trivially_copyable<element>::value>::type
    serialize_element(std::vector<char> &buffer, const element &value)
    {
        buffer.reserve(sizeof(value));
        const char *data_ptr = (char *)&value;
        for (auto i{0u}; i < sizeof(value); i++)
            buffer.push_back(data_ptr[i]);
    }

    void serialize_element(std::vector<char> &buffer, const char *string)
    {
        assert(strlen(string) <= std::numeric_limits<vector_length_t>::max());
        const vector_length_t length = (vector_length_t)strlen(string);
        buffer.push_back(length);

        for (vector_length_t i{0u}; i < length; i++)
            buffer.push_back(string[i]);
    }

    void serialize_element(std::vector<char> &buffer, const std::string &string)
    {
        assert(string.size() <= std::numeric_limits<vector_length_t>::max());
        const vector_length_t length = (vector_length_t)string.size();
        serialize_element(buffer, length);

        for (vector_length_t i{0u}; i < length; i++)
            serialize_element(buffer, string[i]);
    }

    template <typename element>
    typename std::enable_if<std::is_array<element>::value>::type
    serialize_element(std::vector<char> &buffer, const element &value)
    {
        assert(value.size() <= std::numeric_limits<vector_length_t>::max());
        const vector_length_t length = (vector_length_t)value.size();
        serialize_element(buffer, length);

        for (vector_length_t i{0u}; i < length; i++)
            serialize_element(buffer, value[i]);
    }

    template <typename T>
    void serialize_element(std::vector<char> &buffer, const std::vector<T> &value)
    {
        assert(value.size() <= std::numeric_limits<vector_length_t>::max());
        const vector_length_t length = (vector_length_t)value.size();

        serialize_element(buffer, length);

        for (vector_length_t i{0u}; i < length; i++)
            serialize_element(buffer, value[i]);
    }

    template <typename... Args>
    class Datagram
    {
    public:
        Datagram(Args... args) : data{args...} {}

        Datagram(const SerializedDatagram &input)
        {
            std::string_view data{input};
            deserialize(data);
        }

        SerializedDatagram serialize() const
        {
            std::vector<char> buffer;
            buffer.reserve(sizeof(data));
            serialize(buffer);

            return SerializedDatagram{std::string(buffer.data(), buffer.size())};
        }

        template <std::size_t I = 0>
        inline typename std::enable_if<I == sizeof...(Args), void>::type
        deserialize(std::string_view &buffer) {}

        template <std::size_t I = 0>
            inline typename std::enable_if < I<sizeof...(Args), void>::type
                                             deserialize(std::string_view &buffer)
        {
            deserialize_element(buffer, std::get<I>(data));
            deserialize<I + 1>(buffer);
        }

        template <std::size_t I = 0>
        inline typename std::enable_if<I == sizeof...(Args), void>::type
        serialize(std::vector<char> &buffer) const {}

        template <std::size_t I = 0>
            inline typename std::enable_if < I<sizeof...(Args), void>::type
                                             serialize(std::vector<char> &buffer) const
        {
            serialize_element(buffer, std::get<I>(data));
            serialize<I + 1>(buffer);
        }

        template <std::size_t I = 0>
        inline typename std::enable_if<I == sizeof...(Args), void>::type
        print() { std::cout << "\n"; }

        template <std::size_t I = 0>
            inline typename std::enable_if < I<sizeof...(Args), void>::type
                                             print()
        {
            std::cout << std::get<I>(data) << " ";
            print<I + 1>();
        }

        template <size_t N = 0>
        auto get() const
        {
            return std::get<N>(data);
        }

    private:
        std::tuple<Args...> data;
    };
}