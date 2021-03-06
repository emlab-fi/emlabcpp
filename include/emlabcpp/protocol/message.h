#include "emlabcpp/concepts.h"
#include "emlabcpp/view.h"

#include <array>
#include <optional>

#pragma once

namespace emlabcpp
{

// Protocol library has custom type that represents message, however this is just simple overaly
// over std::array that remembers how many bytes are used.
template < std::size_t N >
class protocol_message
{

public:
        using value_type     = uint8_t;
        using iterator       = uint8_t*;
        using const_iterator = const uint8_t*;

        static constexpr std::size_t max_size = N;

        static std::optional< protocol_message > make( const range_container auto& cont )
        {
                if ( cont.size() > N ) {
                        return {};
                }
                return { protocol_message( cont.begin(), cont.end() ) };
        }

        protocol_message() = default;

        template < std::size_t M >
        explicit protocol_message( const protocol_message< M >& other ) noexcept
          : protocol_message( other.begin(), other.end() )
        {
                static_assert( M <= N );
        }

        template < std::size_t M >
        explicit protocol_message( const std::array< uint8_t, M >& inpt ) noexcept
          : protocol_message( inpt.begin(), inpt.begin() + M )
        {
                static_assert( M <= N );
        }

        template < std::convertible_to< uint8_t >... Ts >
        explicit protocol_message( Ts... inpt ) noexcept
          : data_{ static_cast< uint8_t >( inpt )... }
          , used_( sizeof...( Ts ) )
        {
        }

        [[nodiscard]] std::size_t size() const
        {
                return used_;
        }

        [[nodiscard]] uint8_t front() const
        {
                return data_.front();
        }

        [[nodiscard]] uint8_t& front()
        {
                return data_.front();
        }

        [[nodiscard]] uint8_t back() const
        {
                return data_[used_ - 1];
        }

        [[nodiscard]] uint8_t& back()
        {
                return data_[used_ - 1];
        }

        [[nodiscard]] const_iterator begin() const
        {
                return data_.begin();
        }

        [[nodiscard]] iterator begin()
        {
                return data_.begin();
        }

        [[nodiscard]] const_iterator end() const
        {
                return data_.begin() + used_;
        }

        [[nodiscard]] iterator end()
        {
                return data_.begin() + used_;
        }

        uint8_t operator[]( std::size_t i ) const
        {
                return data_[i];
        }

        uint8_t& operator[]( std::size_t i )
        {
                return data_[i];
        }

        template < std::size_t M >
        explicit operator std::array< uint8_t, M >() const
        {
                static_assert( M >= N );
                std::array< uint8_t, M > res{};
                std::copy( data_.begin(), data_.end(), res.begin() );
                std::fill( res.begin() + N, res.end(), 0 );
                return res;
        }

        friend auto operator==( const protocol_message& lh, const protocol_message& rh )
        {
                return view_n( lh.data_.begin(), lh.used_ ) == view_n( rh.data_.begin(), rh.used_ );
        }

private:
        std::array< uint8_t, N > data_ = { 0 };
        std::size_t              used_ = { 0 };

protected:
        template < typename Iterator >
        protocol_message( Iterator begin, Iterator end ) noexcept
          : used_( static_cast< std::size_t >( std::distance( begin, end ) ) )
        {
                std::copy( begin, end, data_.begin() );
        }
};

// Sizeless message is class that behaves in a same way as normal protocol_message, however it is
// serialized differently. Protocol message stores how many bytes it's made of before the data
// itself in the final message, sizless message does not and greedely tries to parse rest of the
// buffer during the parsing process.
template < std::size_t N >
class protocol_sizeless_message : public protocol_message< N >
{
public:
        using protocol_message< N >::protocol_message;

        static std::optional< protocol_sizeless_message > make( const range_container auto& cont )
        {
                if ( cont.size() > N ) {
                        return {};
                }
                return { protocol_sizeless_message( cont.begin(), cont.end() ) };
        }

        template < std::size_t M >
        explicit protocol_sizeless_message( const protocol_message< M >& other )
          : protocol_message< N >( other.begin(), other.end() )
        {
                static_assert( M <= N );
        }
};

namespace detail
{
        template < std::size_t N >
        constexpr bool protocol_message_derived_test( const protocol_message< N >& )
        {
                return true;
        }
}  // namespace detail

// concept matches any type that is protocol_message or derives from it.
template < typename T >
concept protocol_message_derived = requires( T val )
{
        detail::protocol_message_derived_test( val );
};

}  // namespace emlabcpp
