
#if !defined(COMP_DEP_A_HPP)
#define COMP_DEP_A_HPP


#include <hpx/hpx_fwd.hpp>
#include <hpx/runtime/components/server/simple_component_base.hpp>
#include <hpx/runtime/components/server/locking_hook.hpp>
#include <hpx/runtime/actions/component_action.hpp>

namespace components { namespace server {

    struct comp_a
        : public hpx::components::simple_component_base<comp_a>
    {

        void call_b()
        {

        }
        HPX_DEFINE_COMPONENT_ACTION(comp_a, call_b);
    };

} }

HPX_REGISTER_ACTION_DECLARATION(
    components::server::comp_a::call_b_action,
    comp_a_call_b_action);


#endif //COMP_DEP_A_HPP