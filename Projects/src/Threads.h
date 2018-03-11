#ifndef Threads_h__
#define Threads_h__

struct Threads
{
  static void SetFastMode();
  static void SetSlowMode();

  static void SetThreadAffinity(int core);
  static void SetThreadAffinityToAll();
};

#endif // Threads_h__
