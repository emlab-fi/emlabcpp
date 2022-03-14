#include "emlabcpp/static_vector.h"
#include "emlabcpp/testing/base.h"
#include "emlabcpp/testing/comm_interface.h"

#pragma once

namespace emlabcpp
{
class testing_record
{
        testing_test_id                 tid_;
        testing_run_id                  rid_;
        testing_reactor_comm_interface& comm_;
        bool                            errored_ = false;

public:
        testing_record(
            testing_test_id                 tid,
            testing_run_id                  rid,
            testing_reactor_comm_interface& comm )
          : tid_( tid )
          , rid_( rid )
          , comm_( comm )
        {
        }

        testing_arg_variant get_arg( std::string_view key )
        {
                return get_arg( testing_key_to_buffer( key ) );
        }

        testing_arg_variant get_arg( testing_key key )
        {
                comm_.reply< TESTING_ARG >( rid_, key );

                std::optional< testing_arg_variant > res;

                std::optional< testing_controller_reactor_variant > opt_var = comm_.read_variant();
                EMLABCPP_ASSERT( opt_var );
                apply_on_match(
                    *opt_var,
                    [&](
                        tag< TESTING_ARG >, testing_run_id, testing_key, testing_arg_variant var ) {
                            res = var;
                    },
                    [&]< auto ID >( tag< ID >, auto... ){
                        // TODO: add error handling
                    } );
                EMLABCPP_ASSERT( res );
                return *res;
        }

        bool errored()
        {
                return errored_;
        }

        void collect( testing_key key, testing_arg_variant arg )
        {
                comm_.reply< TESTING_COLLECT >( rid_, key, arg );
        }

        void collect( std::string_view key, testing_arg_variant arg )
        {
                collect( testing_key_to_buffer( key ), arg );
        }

        void fail()
        {
                errored_ = true;
        }

        void success()
        {
        }

        void expect( bool val )
        {
                val ? success() : fail();
        }
};
}  // namespace emlabcpp
