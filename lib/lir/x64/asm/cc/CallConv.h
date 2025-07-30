#pragma once


namespace call_conv {

    template<typename CC>
    concept CallConv = requires {
        { CC::GP_ARGUMENT_REGISTERS } -> std::ranges::range;
        { CC::GP_CALLER_SAVE_REGISTERS } -> std::ranges::range;
        { CC::GP_CALLEE_SAVE_REGISTERS } -> std::ranges::range;
        { CC::ALL_GP_REGISTERS } -> std::ranges::range;
    };

    static constexpr std::size_t STACK_ALIGNMENT = 16;
}
