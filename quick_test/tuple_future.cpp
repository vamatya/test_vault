//  Copyright (c) 2015 Vinay C Amatya
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#include <hpx/hpx_init.hpp>
#include <hpx/util/tuple.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/util/lightweight_test.hpp>

#include "boost/type_traits/is_const.hpp"
#include "boost/ref.hpp"
#include <string>
#include <utility>

#include <iostream>

///////////////////////////////////////////////////////////////////////////////

//typedef hpx::util::tuple<bool, int> tup_type;

bool is_even(int);
HPX_DEFINE_PLAIN_ACTION(is_even, is_even_action);
HPX_REGISTER_PLAIN_ACTION_DECLARATION(is_even_action);
HPX_REGISTER_PLAIN_ACTION(is_even_action);

bool is_even(int x)
{
    if (x % 2 == 0)
        return true;
    else
        return false;
}

void tuple_future_test()
{
    typedef hpx::util::tuple<hpx::future<bool>, int> tup_fut_type;
    typedef std::vector<tup_fut_type> tup_vec_type;

    tup_vec_type tup_vec;
    tup_vec_type::iterator itr_tup = tup_vec.begin();

    hpx::id_type loc_id = hpx::find_here();

    for (int i = -100; i < 100; ++i)
    {
        hpx::util::get<0>(*itr_tup) = hpx::async<is_even_action>(loc_id);
        hpx::util::get<1>(*itr_tup) = i;
        ++itr_tup;
    }

    hpx::wait_all(tup_vec);
}

int main(int argc, char* argv[])
{
    tuple_future_test();
    return hpx::util::report_errors();
}
