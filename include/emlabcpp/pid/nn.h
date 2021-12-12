
#pragma once

namespace emlabcpp
{

struct nnneuron
{
        struct p_core
        {
                constexpr float operator()( float i )
                {
                        return std::clamp( i, -1, 1 );
                }
        };

        struct i_core
        {
                float last_res = 0.f;

                constexpr float operator()( float i )
                {
                        if ( i < -1 ) {
                                return -1;
                        }
                        if ( i > 1 ) {
                                return 1;
                        }
                        float res = i + last_res;
                        last_res  = res;
                        return res;
                }
        };

        struct d_core
        {
                float last_i = 0.f;

                constexpr float operator()( float i )
                {
                        if ( i < -1 ) {
                                return -1;
                        }
                        if ( i > 1 ) {
                                return 1;
                        }
                        float res = i - last_i;
                        last_i    = i;
                        return res;
                }
        };

        using core_variant = std::variant< p_core, i_core, d_core >;

        core_variant core_var;

        constexpr float operator()( float i )
        {
                return visit(
                    [&]( auto& core ) {
                            return core( i );
                    },
                    core_var );
        }
}

template < std::size_t I, std::size_t N >
struct nnlayer
{
        std::array< std::array< float, I >, N > edges;
        std::array< nnneuron, N >               neurons;

        std::array< float, N > inpt_;
        std::array< float, N > outpt_;

        std::array< float, N > inpt_last_;
        std::array< float, N > outpt_last_;

        float eps = 0.001;

        constexpr nnlayer( std::array< std::array< float, I >, N > weights )
          : edges( weights )
        {
        }

        constexpr const std::array< float, N >& operator()( const std::array< float, I >& inpt )
        {
                inpt_last_  = inpt_;
                outpt_last_ = outpt_;
                inpt_       = inpt;
                for ( std::size_t i : range( N ) ) {
                        float input = sum( range( I ), [&]( std::size_t j ) {
                                return edges[i][j] * inpt_[j];
                        } );
                        outpt_[i]   = neurons[i]( input );
                }
                return &outpt_;
        }

        constexpr std::array< float, N >
        train( const std::array< float, N >& err, const std::array< float, I >& par_outpt )
        {
                std::array< float, N > res;
                for ( std::size_t j : range( N ) ) {
                        for ( std::size_t i : range( I ) ) {
                                res[i] = -err[j] * ( outpt_[j] - outpt_last_[j] ) /
                                         ( inpt_[j] - inpt_last_[j] );
                                edges[j][i] -= eps * res[i] * par_outpt[i];
                        }
                }
                return res;
        }
};

class nnpid
{
        nnlayer< 2, 3 > inner_{ { 1.f, -1.f }, { 1.f, -1.f }, { 1.f, -1.f } };
        nnlayer< 3, 1 > out_{ { 0.5f }, { 0.001f }, { 0.1f } };

        constexpr float update( float measured, float expected )
        {
                return out_( inner_( { measured, expected } ) )[0];
        }
        constexpr void train( float measured, float expected )
        {
                auto changes = out_.train( { expected - measured }, out_.outpt_ );
                inner_.train( changes, { measured, expected } );
        }
}

}  // namespace emlabcpp
