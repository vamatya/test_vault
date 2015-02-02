//  Copyright (c) 2014 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <hpx/components/distributing_factory/distributing_factory.hpp>

///////////////////////////////////////////////////////////////////////////////

template <typename Component>
std::vector<hpx::id_type>
spawn_components(hpx::id_type host, std::size_t num)
{
    typedef hpx::util::remote_locality_result value_type;
    typedef std::pair<std::size_t, std::vector<value_type> > result_type;

    result_type res;

    typedef std::vector<hpx::id_type> id_vector_type;
    hpx::components::component_type c_type =
        hpx::components::get_component_type<Component>();

    typedef
        hpx::components::server::runtime_support::bulk_create_components_action
        action_type;
    typedef hpx::future<std::vector<hpx::naming::gid_type> > future_type;

    future_type f;
    {
        hpx::lcos::packaged_action < action_type
            , std::vector<hpx::naming::gid_type> > p;
        p.apply(hpx::launch::async, host, c_type, num);
        f = p.get_future();
    }

    res.first = num;
    res.second.push_back(
        value_type(host.get_gid(), c_type));
    res.second.back().gids_ = boost::move(f.get());

    id_vector_type comps;

    comps.reserve(num);

    std::vector<hpx::util::locality_result> res2;
    BOOST_FOREACH(hpx::util::remote_locality_result const& r1, res.second)
    {
        res2.push_back(r1);
    }

    BOOST_FOREACH(hpx::id_type id, hpx::util::locality_results(res2))
    {
        comps.push_back(id);
    }

    return comps;
}

// template <typename 
// std::vector<hpx::id_type>
// mv_comps(std::vector<hpx::id_type> comps, hpx::id_type target)
// {
//     std::vector<hpx::id_type> moved_components;
//     BOOST_FOREACH(hpx::id_type id, comps)
//     {
//         test_client tx(hpx::components::migrate<test_server>(
//             id, target));
//         moved_components.push_back(tx.get_gid());
//     }
//     return moved_components;
// }

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

    template <typename Archive>
    void serialize(Archive&ar, unsigned version) {}

private:
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


struct test_client
  : hpx::components::client_base<test_client, test_server>
{
    typedef hpx::components::client_base<test_client, test_server>
        base_type;

    test_client() {}
    test_client(hpx::shared_future<hpx::id_type> const& id) : base_type(id) {}

    hpx::id_type call() const { return call_action()(this->get_gid()); }
};

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
bool test_migrate_component(hpx::id_type source, hpx::id_type target)
{
    hpx::id_type u1, u2;
    // create component on given locality
    test_client t1 = test_client::create(source);
    HPX_TEST_NEQ(hpx::naming::invalid_id, t1.get_gid());

    // the new object should live on the source locality
    HPX_TEST_EQ(t1.call(), source);
    u1 = t1.get_gid();
    try {
        // migrate of t1 to the target
        
        test_client t2(hpx::components::migrate<test_server>(
            t1.get_gid(), target));
        HPX_TEST_NEQ(hpx::naming::invalid_id, t2.get_gid());
        u2 = t2.get_gid();
        // the migrated object should have the same id as before
        HPX_TEST_EQ(t1.get_gid(), t2.get_gid());

        // the migrated object should life on the target now
        HPX_TEST_EQ(t2.call(), target);

        return true;
    }
    catch (hpx::exception const&) {
        return false;
    }
}

bool test_bulk_migrate(hpx::id_type source, hpx::id_type destination)
{
    log_client l1 = log_client::create(source);
    l1.init(0);
    l1.populate_n(spawn_components<test_server>(hpx::find_here(), 100000));
    
    log_client l2 = log_client::create(destination);
    l2.init(1);

    std::vector<hpx::id_type> ids_u = l1.get_components();
    std::vector<hpx::id_type> ids_v;
    std::vector<hpx::future<hpx::id_type> > vec_futs;
    
    try {

        std::cout << "before migration:" << std::endl;
        l1.print_stat();
        l2.print_stat();

        BOOST_FOREACH(hpx::id_type id, ids_u)
        {
            vec_futs.push_back(hpx::components::migrate<test_server>(
                id, destination));
        }

        hpx::wait_all(vec_futs);

        BOOST_FOREACH(hpx::future<hpx::id_type>& fut, vec_futs)
        {
            ids_v.push_back(fut.get());
        }

        l2.populate_n(ids_v);
        
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

int main()
{
    std::vector<hpx::id_type> localities = hpx::find_remote_localities();

    std::vector<hpx::id_type>::iterator itrb = localities.begin();

    BOOST_FOREACH(hpx::id_type const& id, localities)
    {
        HPX_TEST(test_migrate_component(hpx::find_here(), id));
        HPX_TEST(test_migrate_component(id, hpx::find_here()));
        HPX_TEST(test_bulk_migrate(hpx::find_here(), id));

    }
    //std::vector<hpx::id_type> migrated_comps = mv_comps(this_comps, *itrb);

    return hpx::util::report_errors();
}
