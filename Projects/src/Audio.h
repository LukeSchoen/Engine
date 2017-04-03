#ifndef Audio_h__
#define Audio_h__

struct Audio
{
  static void PlayMP3(char* path, int volume = 1000, char *name = nullptr, bool repeat = false);

  static void ChangeVolume(char *name, int volume = 1000);

  static void StopMP3(char *name);
};

#endif // Audio_h__
