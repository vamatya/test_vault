//  Copyright (c) 2014 Hartmut Kaiser
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

//#include <hpx/components/distributing_factory/distributing_factory.hpp>

#include <iostream>

using boost::program_options::variables_map;
using boost::program_options::options_description;
using boost::program_options::value;

///////////////////////////////////////////////////////////////////////////////
// namespace hpx {
//     namespace components{
//         template < typename Process> //typename Wrapper, typename CtorPolicy, typename DtorPolicy >
//         class process_base
//             : public managed_component_tag, boost::noncopyable
//         {
// 
//                // Process Base Class
//         };
// 
//     }
// }

// 
// Abstract Class for different localities(device)
// template <typename T>
// struct abstract_locality 
// {
//     //
//     virtual hpx::id_type get_locality_id() = 0;
//     
//     //
// };

// template <typename P>
// struct abstract_policy
// {
//     virtual ~abstract_policy() = 0;
// };
// 
// 
// struct locality_base
//     : abstract_locality < locality_base >
// {
//     hpx::id_type get_locality_id()
//     {
//         hpx::id_type id;
//         return id;
//     }
// private:
//     // data related to locality
// };
// 
// struct policy_base
//     : abstract_policy < policy_base >
// {
// 
// };



// template <typename Locality, typename Policy>
// struct agas_client
//     : hpx::components::process_base<agas_client<Locality, Policy> >
// {
// 
//     //
//     // Client AGAS attributes and functions
// };

// template <typename Locality>
// struct agas_server  //Factory
//     : hpx::components::process_base < agas_server<Locality> >
// {
//     //
//     // Central Coordinating Server
// private:
//     typename std::vector<Locality> localities_;
// 
// };
// 
// 
// template <typename T>
// struct agas_server_m
//     : hpx::components::process_base < agas_server_m<T> >
// {
//     // 
// };


template <typename Component>
std::vector<hpx::id_type>
spawn_components(hpx::id_type host, std::size_t num)
{
    typedef std::pair<std::size_t, std::vector<hpx::id_type> > result_type;

    result_type res;

    typedef std::vector<hpx::id_type> id_vector_type;
    
//     hpx::components::component_type c_type =
//         hpx::components::get_component_type<Component>();

    typedef hpx::future<std::vector<hpx::naming::gid_type> > future_type;

    hpx::future<id_vector_type> fut = hpx::new_<Component[]>(host, num);

    res.first = num;
    res.second = fut.get();

    id_vector_type comps;

    comps.reserve(num);

    return res.second;//comps;
}

struct test_objects
{
    test_objects()
        : test_pass(false)
    {}
    ~test_objects()
    {}

    hpx::id_type this_one;
    hpx::id_type that_one;
    bool test_pass;

    friend class hpx::serialization::access;
    template <typename Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & this_one;
        ar & that_one;
        ar & test_pass;
    }
};

struct test_server
  : hpx::components::migration_support<
        hpx::components::simple_component_base<test_server>
    >
{
    typedef hpx::components::migration_support<
        hpx::components::simple_component_base< test_server>
    >
        base_type;

    test_server() {}
    ~test_server() {}

    //test_server(hpx::shared_future<hpx::id_type> const& id) :base_type(id.get())
    //{}

    hpx::id_type call() const
    {
        return hpx::find_here();
    }

    // Components which should be migrated using hpx::migrate<> need to
    // be Serializable and CopyConstructable. Components can be
    // MoveConstructable in which case the serialized  is moved into the
    // components constructor.
    test_server(test_server const& rhs) {}
    test_server(test_server && rhs) {}

    test_server& operator=(test_server const &) { return *this; }
    test_server& operator=(test_server &&) { return *this; }

    HPX_DEFINE_COMPONENT_ACTION(test_server, call, call_action);

private:
    std::vector<hpx::id_type> ids_temp_;
    test_objects t_obj_;

    friend class hpx::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & ids_temp_;
        ar & t_obj_;
    }
    ;
};

typedef hpx::components::simple_component<test_server> server_type;
HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(server_type, test_server);

typedef test_server::call_action call_action;
HPX_REGISTER_ACTION_DECLARATION(call_action);
HPX_REGISTER_ACTION(call_action);

struct log_server
    : hpx::components::simple_component_base < log_server >
{
    log_server(){}
    ~log_server(){}

    
    
    typedef std::vector<hpx::id_type> id_vector_type;

    void init(std::size_t rank)
    {
        rank_ = rank;
    }
    HPX_DEFINE_COMPONENT_ACTION(log_server, init, init_action);

    void populate(hpx::id_type id)
    {
        comps_.push_back(id);
    }
    HPX_DEFINE_COMPONENT_ACTION(log_server, populate, populate_action);

    void populate_n(id_vector_type ids)
    {
        comps_ = ids;
    }
    HPX_DEFINE_COMPONENT_ACTION(log_server, populate_n, populate_n_action);

    void de_populate()
    {
        comps_.clear();
    }
    HPX_DEFINE_COMPONENT_ACTION(log_server, de_populate, de_populate_action);

    void print_stat()
    {
        std::cout << "Rank:" << rank_ << ", comp this loc:" << comps_.size() << std::endl;
    }
    HPX_DEFINE_COMPONENT_ACTION(log_server, print_stat, print_stat_action);

    id_vector_type get_components()
    {
        return comps_;
    }
    HPX_DEFINE_COMPONENT_ACTION(log_server, get_components, get_components_action);

private:
    id_vector_type comps_;
    std::size_t rank_;
};



typedef hpx::components::simple_component<log_server> log_server_type;
HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(log_server_type, log_server_component_reg);

typedef log_server::init_action init_action;
HPX_REGISTER_ACTION_DECLARATION(init_action);
HPX_REGISTER_ACTION(init_action);

typedef log_server::populate_action populate_action;
HPX_REGISTER_ACTION_DECLARATION(populate_action);
HPX_REGISTER_ACTION(populate_action);

typedef log_server::populate_n_action populate_n_action;
HPX_REGISTER_ACTION_DECLARATION(populate_n_action);
HPX_REGISTER_ACTION(populate_n_action);

typedef log_server::de_populate_action de_populate_action;
HPX_REGISTER_ACTION_DECLARATION(de_populate_action);
HPX_REGISTER_ACTION(de_populate_action);

typedef log_server::print_stat_action print_stat_action;
HPX_REGISTER_ACTION_DECLARATION(print_stat_action);
HPX_REGISTER_ACTION(print_stat_action);

typedef log_server::get_components_action get_components_action;
HPX_REGISTER_ACTION_DECLARATION(get_components_action);
HPX_REGISTER_ACTION(get_components_action);


// struct test_client
//   : hpx::components::client_base<test_client, test_server>
// {
//     typedef hpx::components::client_base<test_client, test_server>
//         base_type;
// 
//     test_client() {}
//     test_client(hpx::shared_future<hpx::id_type> const& id) : base_type(id) {}
// 
//     hpx::id_type call() const { return call_action()(this->get_gid()); }
// };

struct log_client
    : hpx::components::client_base < log_client, log_server >
{
    typedef hpx::components::client_base < log_client, log_server >
        base_type;

    log_client(){}
    //log_client(hpx::id_type const& id) : base_type(id) {}
    log_client(hpx::shared_future<hpx::id_type> const& id) : base_type(id) {}

    void init(std::size_t rank) const
    {
        init_action()(this->get_gid(), rank);
    }

    void populate_n(std::vector<hpx::id_type> ids) const 
    { 
        populate_n_action()(this->get_gid(), ids); 
    }

    void populate(hpx::id_type id) const
    {
        populate_action()(this->get_gid(), id);
    }

    void de_populate() const
    {
        de_populate_action()(this->get_gid());
    }

    void print_stat() const
    {
        print_stat_action()(this->get_gid());
    }

    std::vector<hpx::id_type> get_components() const
    {
        return get_components_action()(this->get_gid());
    }

private:
    ;
};

///////////////////////////////////////////////////////////////////////////////
// bool test_migrate_component(hpx::id_type source, hpx::id_type target)
// {
//     hpx::id_type u1, u2;
//     // create component on given locality
//     test_client t1 = test_client::create(source);
//     HPX_TEST_NEQ(hpx::naming::invalid_id, t1.get_gid());
// 
//     // the new object should live on the source locality
//     HPX_TEST_EQ(t1.call(), source);
//     u1 = t1.get_gid();
//     try {
//         // migrate of t1 to the target
//         
//         test_client t2(hpx::components::migrate<test_server>(
//             t1.get_gid(), target));
//         HPX_TEST_NEQ(hpx::naming::invalid_id, t2.get_gid());
//         u2 = t2.get_gid();
//         // the migrated object should have the same id as before
//         HPX_TEST_EQ(t1.get_gid(), t2.get_gid());
// 
//         // the migrated object should life on the target now
//         HPX_TEST_EQ(t2.call(), target);
// 
//         return true;
//     }
//     catch (hpx::exception const&) {
//         return false;
//     }
// }

bool test_bulk_migrate(hpx::id_type source, hpx::id_type destination, std::size_t num_comp)
{
    log_client l1 = log_client::create(source);
    l1.init(0);
    l1.populate_n(spawn_components<test_server>(hpx::find_here(), num_comp));
    
    log_client l2 = log_client::create(destination);
    l2.init(1);

    std::vector<hpx::id_type> ids_u = l1.get_components();
    std::vector<hpx::id_type> ids_v;
    std::vector<hpx::future<hpx::id_type> > vec_futs;
    
    try {

        std::cout << "before migration:" << std::endl;
        l1.print_stat();
        l2.print_stat();

        for (hpx::id_type id : ids_u)
        {
            vec_futs.push_back(hpx::components::migrate<test_server>(
                id, destination));
        }

        hpx::wait_all(vec_futs);

        for (hpx::future<hpx::id_type>& fut: vec_futs)
        {
            ids_v.push_back(fut.get());
        }

        l2.populate_n(ids_v);
        
        std::vector<hpx::id_type>::iterator itr_u, itr_v;

        itr_u = ids_u.begin();
        itr_v = ids_v.begin();

        HPX_TEST_EQ(ids_u.size(), ids_v.size());

        for (std::size_t i = 0; i < num_comp; ++i)
        {
            HPX_TEST_EQ(*itr_u, *itr_v);
            ++itr_u;
            ++itr_v;
        }

        //HPX_TEST_EQ(ids_u, ids_v);

        l1.de_populate();

        std::cout << "after migration:" << std::endl;
        l1.print_stat();
        l2.print_stat();

        return true;
    }
    catch(hpx::exception const&){
        return false;
    }
}

int hpx_main(variables_map & vm)
{
    {
        std::size_t num_comp = vm["num"].as<std::size_t>();
        std::vector<hpx::id_type> localities = hpx::find_remote_localities();

        std::vector<hpx::id_type>::iterator itrb = localities.begin();

        for (hpx::id_type const& id : localities)
        {
            //HPX_TEST(test_migrate_component(hpx::find_here(), id));
            //HPX_TEST(test_migrate_component(id, hpx::find_here()));
            HPX_TEST(test_bulk_migrate(hpx::find_here(), id, num_comp));
        }
        //return hpx::util::report_errors();
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
