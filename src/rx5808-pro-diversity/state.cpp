#include <stddef.h>
#include "state.h"


static StateMachine::HookFunc enterFuncs[STATE_COUNT] = { nullptr };
static StateMachine::HookFunc tickFuncs[STATE_COUNT] = { nullptr };
static StateMachine::HookFunc exitFuncs[STATE_COUNT] = { nullptr };


namespace StateMachine {
    State currentState = State::MANUAL;
    State lastState = currentState;


    void registerEnterFunc(State state, HookFunc func) {
        enterFuncs[static_cast<size_t>(state)] = func;
    }

    void registerTickFunc(State state, HookFunc func) {
        tickFuncs[static_cast<size_t>(state)] = func;
    }

    void registerExitFunc(State state, HookFunc func) {
        exitFuncs[static_cast<size_t>(state)] = func;
    }


    void switchState(State newState) {
        lastState = currentState;
        currentState = newState;

        const HookFunc exitFunc = exitFuncs[static_cast<size_t>(newState)];
        const HookFunc enterFunc = enterFuncs[static_cast<size_t>(newState)];

        if (exitFunc)
            exitFunc();

        if (enterFunc)
            enterFunc();
    }


    void tick() {
        const HookFunc func = tickFuncs[static_cast<size_t>(currentState)];
        if (func)
            func();
    }
}
