#ifndef STATE_H
#define STATE_H


namespace StateMachine {
  #define STATE_COUNT 4
  enum class State {
      BOOT,
      MANUAL,
      SCAN,
      SCREENSAVER
  };


  typedef void(*HookFunc)();


  extern State currentState;
  extern State lastState;


  void registerEnterFunc(State state, HookFunc func);
  void registerExitFunc(State state, HookFunc func);
  void registerTickFunc(State state, HookFunc func);

  void switchState(State newState);

  void tick();
}


#endif
