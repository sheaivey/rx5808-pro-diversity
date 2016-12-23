#include <stddef.h>
#include "state.h"


static StateMachine::EnterFunc enterFuncs[STATE_COUNT] = { nullptr };
static StateMachine::TickFunc tickFuncs[STATE_COUNT] = { nullptr };


namespace StateMachine {
    State currentState = State::BOOT;


    void registerEnterFunc(State state, EnterFunc func) {
        enterFuncs[static_cast<size_t>(state)] = func;
    }

    void registerTickFunc(State state, TickFunc func) {
        tickFuncs[static_cast<size_t>(state)] = func;
    }


    void switchState(State newState) {
        const EnterFunc func = enterFuncs[static_cast<size_t>(newState)];
        if (func)
            func();

        currentState = newState;
    }


    void tick() {
        const TickFunc func = tickFuncs[static_cast<size_t>(currentState)];
        if (func)
            func();
    }
}
