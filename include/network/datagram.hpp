#pragma once
#include <tuple>
#include <type_traits>
#include <limits>
#include <cassert>
#include <span>
#include <cstring>
#include <iostream>
#include <vector>
namespace network
{
    typedef uint16_t message_length_t;
    typedef uint16_t vector_length_t;

    class SerializedDatagram
    {
    public:
        explicit SerializedDatagram(const std::string_view &data);
        operator std::string() const;
        operator std::string_view() const;
        std::size_t size() const;
        const char *data() const;

    private:
        std::string string;
    };


    template<typename T, typename = void>
    struct HasDeserialize : std::false_type {};

    template<typename T>
    struct HasDeserialize<T, decltype(&T::deserialize, void())> : std::true_type {};


    template<typename T, typename = void>
    struct HasSerialize : std::false_type {};

    template<typename T>
    struct HasSerialize<T, decltype(&T::serialize, void())> : std::true_type {};

    template <typename element>
    typename std::enable_if<std::is_trivially_copyable<element>::value>::type
    deserialize_element(std::string_view &buffer, const element &value)
    {
        std::memcpy((void *)&value, buffer.data(), sizeof(value));
        buffer.remove_prefix(sizeof(value));
    }

    void deserialize_element(std::string_view &buffer, char *&string);

    void deserialize_element(std::string_view &buffer, std::string &string);

    template <typename T>
    typename std::enable_if<HasDeserialize<T>::value>::type
    deserialize_element(std::string_view &buffer, T &value)
    {
        T::deserialize(buffer, value);
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

    void serialize_element(std::vector<char> &buffer, const char *string);

    void serialize_element(std::vector<char> &buffer, const std::string &string);

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
    typename std::enable_if<HasSerialize<T>::value>::type
    serialize_element(std::vector<char> &buffer, const T &value)
    {
        T::serialize(buffer, value);
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
        constexpr inline typename std::enable_if<I == sizeof...(Args), void>::type
        deserialize(std::string_view &) {}

        template <std::size_t I = 0>
        constexpr inline typename std::enable_if < I<sizeof...(Args), void>::type
                                             deserialize(std::string_view &buffer)
        {
            deserialize_element(buffer, std::get<I>(data));
            deserialize<I + 1>(buffer);
        }

        template <std::size_t I = 0>
        inline typename std::enable_if<I == sizeof...(Args), void>::type
        constexpr serialize(std::vector<char> &) const {}

        template <std::size_t I = 0>
        constexpr inline typename std::enable_if < I<sizeof...(Args), void>::type
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

        template <std::size_t N = 0>
        constexpr auto& get() const
        {
            return std::get<N>(data);
        }

                
        constexpr auto& getData() const
        {
            return data;
        }

    private:
        std::tuple<Args...> data;
    };
}