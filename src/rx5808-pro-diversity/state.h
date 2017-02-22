#ifndef STATE_H
#define STATE_H


namespace StateMachine {
    #define STATE_COUNT 6
    enum class State {
        BOOT,
        SEARCH,
        BANDSCAN,
        SCREENSAVER,
        MENU,
        SETTINGS
    };

    class StateHandler {
        public:
            virtual void onInitialDraw() {};
            virtual void onUpdateDraw() {};

            virtual void onEnter() {};
            virtual void onTick() {};
            virtual void onExit() {};

            virtual void onButtonChange() {};
    };

    extern State currentState;
    extern State lastState;

    void setup();
    void tick();

    void switchState(State newState);
}


#endif
