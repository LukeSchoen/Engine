#include "Audio.h"
#include <windows.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <stdio.h>

//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")

static char cmd[256];
static char nameBuff[256];

const int channelCount = 32;
static int currentChannel = 0;

void Audio::PlayMP3(char *path, int volume, char *name, bool repeat)
{
  if (!name)
  {
    sprintf(nameBuff, "channel%d", currentChannel++);
    name = nameBuff;
    StopMP3(name);
    if (currentChannel == channelCount)
      currentChannel = 0;
  }

  sprintf(cmd, "open \"%s\" type mpegvideo alias %s", path, name);
  mciSendStringA(cmd, 0, 0, 0);

  sprintf(cmd, "setaudio %s volume to %d", name, volume);
  mciSendStringA(cmd, 0, 0, 0);

  if(repeat)
    sprintf(cmd, "play %s repeat", name);
  else
    sprintf(cmd, "play %s", name);
  mciSendStringA(cmd, 0, 0, 0);
}

void Audio::StopMP3(char *name)
{
  sprintf(cmd, "stop %s", name);
  mciSendStringA(cmd, 0, 0, 0);

  sprintf(cmd, "close %s", name);
  mciSendStringA(cmd, 0, 0, 0);
}

void Audio::ChangeVolume(char *name, int volume)
{
  sprintf(cmd, "setaudio %s volume to %d", name, volume);
  mciSendStringA(cmd, 0, 0, 0);
}