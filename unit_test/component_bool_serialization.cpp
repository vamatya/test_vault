//  Copyright (c) 2015 Vinay C Amatya
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/hpx_fwd.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <iostream>

using boost::program_options::variables_map;
using boost::program_options::options_description;
using boost::program_options::value;

struct temp_obj
{
    temp_obj()
        :int_val1(NULL)
    {}
    ~temp_obj(){}

    bool bool_val1;
    int int_val1;

    friend class hpx::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & bool_val1;
        ar & int_val1;
    }
};

struct test_server
    : hpx::components::simple_component_base < test_server >
{
    test_server(){}
    ~test_server(){}

    temp_obj invalid_type;

    void pass_boolean(bool passed_boolean)
    {
        passed_boolean_ = passed_boolean;
    }

    HPX_DEFINE_COMPONENT_ACTION(test_server, pass_boolean, pass_boolean_action);

    void init(hpx::id_type id, bool a, int b)
    {
        my_id_ = id;
        obj_.bool_val1 = a;
        obj_.int_val1 = b;
    }

    HPX_DEFINE_COMPONENT_ACTION(test_server, init, init_action);

    void pass_object(temp_obj passed_obj)
    {
        obj_ = passed_obj;
    }
    HPX_DEFINE_COMPONENT_ACTION(test_server, pass_object, pass_object_action);

    void pass_loc_object(hpx::id_type remote_id)
    {
        //temp_obj invalid_object = temp_obj();
        if (obj_.int_val1 != NULL)
        {
            hpx::future<void> fut =
                hpx::async<test_server::pass_object_action>(remote_id, obj_);
            fut.get();
        }
    }
    HPX_DEFINE_COMPONENT_ACTION(test_server, pass_loc_object, pass_loc_object_action);

    temp_obj get_obj()
    {
        return obj_;
    }
    HPX_DEFINE_COMPONENT_ACTION(test_server, get_obj, get_obj_action);
    
private:
    hpx::id_type my_id_;
    std::size_t rank_;
    bool original_bolean_;
    bool passed_boolean_;
    temp_obj obj_;
    temp_obj passed_obj_;

//     friend class hpx::serialization::access;
//     template<class Archive>
//     void serialize(Archive & ar, const unsigned int version)
//     {
//         ar & rank_;
//         ar & original_boolean_;
//         ar & passed_boolean_;
//     }

};

typedef hpx::components::simple_component<test_server> server_type;
HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(server_type, test_test_server);

typedef test_server::pass_boolean_action pb_action;
HPX_REGISTER_ACTION_DECLARATION(pb_action);
HPX_REGISTER_ACTION(pb_action);

typedef test_server::init_action init_action;
HPX_REGISTER_ACTION_DECLARATION(init_action);
HPX_REGISTER_ACTION(init_action);

typedef test_server::pass_object_action po_action;
HPX_REGISTER_ACTION_DECLARATION(po_action);
HPX_REGISTER_ACTION(po_action);

typedef test_server::pass_loc_object_action plo_action;
HPX_REGISTER_ACTION_DECLARATION(plo_action);
HPX_REGISTER_ACTION(plo_action);

typedef test_server::get_obj_action get_obj_action;
HPX_REGISTER_ACTION_DECLARATION(get_obj_action);
HPX_REGISTER_ACTION(get_obj_action);

bool test_component_serialization(hpx::id_type loc1, hpx::id_type loc2)
{
    hpx::future<hpx::id_type> f_comp1 = hpx::new_<test_server>(loc1);
    hpx::future<hpx::id_type> f_comp2 = hpx::new_<test_server>(loc2);

    hpx::id_type comp1 = f_comp1.get();
    hpx::id_type comp2 = f_comp2.get();


    hpx::future<void> fut_a = hpx::async<init_action>(comp1, comp1, true, 5);
    fut_a.get();
    hpx::future<void> fut_b = hpx::async<init_action>(comp2, comp2, false, 10);
    fut_b.get();
    hpx::future<void> fut2 = hpx::async<plo_action>(comp1, comp2);
    fut2.get();
    hpx::future<temp_obj> fut3 = hpx::async<get_obj_action>(comp2);
    temp_obj obj_inst = fut3.get();

    if (obj_inst.int_val1 == 5)
        return true;
    else
        return false;
}


int hpx_main(variables_map & vm)
{
    {
        std::size_t num_comp = vm["num"].as<std::size_t>();
        std::vector<hpx::id_type> localities = hpx::find_all_localities();

        std::vector<hpx::id_type>::iterator itrb = localities.begin();
        if (localities.size() > 1)
        {
            HPX_TEST(test_component_serialization(localities[0], localities[1]));
//             for (hpx::id_type const& id : localities)
//             {
//                 HPX_TEST(test_migrate_component(hpx::find_here(), id));
//                 HPX_TEST(test_migrate_component(id, hpx::find_here()));
//                 HPX_TEST(test_bulk_migrate(hpx::find_here(), id, num_comp));
//             }
        }
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    options_description cmdline("Usage: " HPX_APPLICATION_STRING " [options]");

    cmdline.add_options()
        ("num", value<std::size_t>()->default_value(1000), "Num components to be bulk migrated.")
        ;

    return hpx::init(cmdline, argc, argv);
}
