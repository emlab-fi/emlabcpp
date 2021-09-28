#include "emlabcpp/either.h"
#include "emlabcpp/protocol/item.h"

#pragma once

namespace emlabcpp
{

template < typename Map >
struct protocol_register_handler
{
        using map_type = Map;
        using key_type = typename map_type::key_type;

        static constexpr std::size_t max_size = map_type::max_value_size;

        using message_type = typename map_type::message_type;

        template < key_type Key >
        static message_type serialize( typename map_type::reg_value_type< Key > val )
        {
                using pitem =
                    protocol_item< typename map_type::reg_def_type< Key >, PROTOCOL_BIG_ENDIAN >;

                std::array< uint8_t, max_size > buffer;
                static_assert( pitem::max_size <= max_size );
                bounded used = pitem::serialize_at(
                    std::span< uint8_t, pitem::max_size >( buffer.begin(), pitem::max_size ), val );
                EMLABCPP_ASSERT( *used <= max_size );

                return *message_type::make( view_n( buffer.begin(), *used ) );
        }

        template < key_type Key >
        static either< typename map_type::reg_value_type< Key >, protocol_error_record >
        extract( const message_type& msg )
        {
                using pitem =
                    protocol_item< typename map_type::reg_def_type< Key >, PROTOCOL_BIG_ENDIAN >;

                return pitem::deserialize( msg ).convert_left( [&]( auto sub_res ) {
                        return sub_res.val;
                } );
        }
};

}  // namespace emlabcpp