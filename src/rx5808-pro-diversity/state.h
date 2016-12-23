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


  typedef void(*EnterFunc)();
  typedef void(*TickFunc)();


  extern State currentState;
  extern State lastState;


  void registerEnterFunc(State state, EnterFunc func);
  void registerTickFunc(State state, TickFunc func);

  void switchState(State newState);

  void tick();
}


#endif