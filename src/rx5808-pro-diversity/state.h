#ifndef STATE_H
#define STATE_H


namespace StateMachine {
  #define STATE_COUNT 3
  enum class State {
      BOOT,
      SCAN,
      SCREENSAVER
  };


  typedef void(*EnterFunc)();
  typedef void(*TickFunc)();


  extern State currentState;


  void registerEnterFunc(State state, EnterFunc func);
  void registerTickFunc(State state, TickFunc func);

  void switchState(State newState);

  void tick();
}


#endif
