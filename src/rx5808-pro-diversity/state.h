#ifndef STATE_H
#define STATE_H


namespace StateMachine {
    #define STATE_COUNT 5
    enum class State {
        BOOT,
        MANUAL,
        SCAN,
        AUTO,
        SCREENSAVER
    };

    class StateHandler {
        public:
            virtual void onEnter() {};
            virtual void onTick() {};
            virtual void onExit() {};
    };

    extern State currentState;
    extern State lastState;

    void switchState(State newState);
    void tick();
}


#endif
