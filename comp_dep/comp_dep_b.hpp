
#if !defined(COMP_DEP_B_HPP)
#define COMP_DEP_B_HPP

#include <hpx/hpx_fwd.hpp>
#include <hpx/runtime/components/server/simple_component_base.hpp>
#include <hpx/runtime/components/server/locking_hook.hpp>
#include <hpx/runtime/actions/component_action.hpp>

namespace components {
    namespace server {

        struct comp_b
            : public hpx::components::simple_component_base < comp_b >
        {
            void call_a()
            {}
            HPX_DEFINE_COMPONENT_ACTION(comp_b, call_a);
        };

    }
}

HPX_REGISTER_ACTION_DECLARATION(
    components::server::comp_b::call_a_action,
    comp_b_call_a_action);

#endif //COMP_DEP_B_HPP