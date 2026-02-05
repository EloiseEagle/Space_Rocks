#pragma once

// Modified 7/6/24
// LoadSound and LoadMusic will try to load from "assets\"+file if it fails,
// in case programmer just forgot to add the folder path.


#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <map>
#include "Result.h"
#pragma comment(lib, "SDL2_mixer")

// This is like a typedef, but does not allow implicit cast to/from int
// This is to prevent programmers mixing up the SoundIndex with
// the channel numbers
enum class SoundIndex : int {};
const SoundIndex NO_SOUND_INDEX = (SoundIndex) - 1;


class HtAudio
{
public:
    static const SoundIndex NOSOUND = SoundIndex(-1);

    // Initialises the sound system. Called during startup
    Result Initialise();

    // Loads a sound file. It should cope with .wav and .mp3 and various
    // other formats, but if your sound wav has an unusual format or
    // sample frequency, that might cause this to fail.
    // The function returns a SoundIndex, which you can use to play
    // the sound when needed. If you try to load a file that has
    // already been loaded, this function will not load again. Instead
    // it will return the previous SoundIndex.
    // Returns -1 on error.
    SoundIndex LoadSound(std::string filename);

    // Plays the sound with the specified SoundIndex. If "playLooping" is
    // set to true, the sound will loop indefinitely. 
    // Returns the "channel" number for the sound, which can be used to
    // stop the sound, or change the frequency.
    // This is particularly important for a looping sound, as it is 
    // required to stop the sound.
    // Note that this can only play a limit number of channels at once
    // (currently 8). Playing too many sounds at the same time will cause
    // an error.
    // Returns -1 on error.

    int Play(SoundIndex sound, bool playLooping = false);

    // Stops playing the specified channel
    // Returns SUCCESS if the channel stopped.
    // False otherwise
    Result Stop(int channel);

    // Stops playing all sound effects (not music)
    void StopAllChannels();

    // Sets the volume for the specified sound.
    // volume from 0.0 (silent) to 1.0 (full volume)
    // Default is full volume. Note that this will only affect the volume
    // the next time the sound is played. If you want to change the volume
    // of a sound that is already playing, using SetChannelVolume().
    Result SetSoundVolume(SoundIndex sound, double volume);

    // Sets the volume for the specified channel.
    // volume from 0.0 (silent) to 1.0 (full volume)
    // Default is full volume.
    Result SetChannelVolume(int channel, double volume);

    // Sets the volume for all sounds BUT NOT MUSIC!!!!
    // volume from 0.0 (silent) to 1.0 (full volume)
    // Default is full volume.
    // This is cumulative with the individual volume for sounds.
    // So if a sound is a 0.5 volume and the master volume is 0.5
    // that sound will play at 0.25 volume
    // Note this does not affect the system volume settings.
    // And it does not affect music.
    Result SetMasterVolume(double volume);

    // Pans the specified channel from left to right on a stereo system.
    // pan is from -1.0 (full left) to 1.0 (full right)
    // Default is 0.0, which is centred.
    // Note that the audio file may already contain stereo values.
    Result SetPan(int channel, double pan);

    // Loads a music file with the specified filename, and also unloads
    // any music that is already loaded.
    Result LoadMusic(std::string filename);

    // Plays the currently loaded music, if any.
    // If loop is true, the music will repeat indefinitely.
    // If loop is false, the music will stop when complete.
    // Note this will re-start the music if it is already playing.
    Result PlayMusic(bool loop = true);

    // Stops music playing, if any music is currently playing.
    // If no music is playing, has no effect.
    void StopMusic();

    // Returns true if music is currently playing. False otherwise.
    bool IsMusicPlaying() const;

    // Pauses the music, so it can be resumed from the current position
    // later, using ResumeMusic().
    void PauseMusic();

    // Resumes the paused music. Has no effect if music is not playing
    // or not paused.
    void ResumeMusic();

    // Sets the volume of the music.
    // volume is from 0.0 (silent) to 1.0 (full volume)
    // Default is full volume.
    void SetMusicVolume(double volume);

    // Releases all sound assets, including music
    void ReleaseAllSounds();

    // This releases the specified sound, freeing memory. Remember that there may be multiple objects
    // using the same sound, and releasing it in one place affects all of them.
    // Is is usually best to leave sound alone and release them at the end of the game using
    // ReleaseAllSounds(), but a complex game with a lot of assets may need this.
    void ReleaseSound(SoundIndex soundToRelease);

    // Shuts down the Audio system. Called at the end of the program.
    // This calls ReleaseAllSounds()
    void Shutdown();

    // Singleton instance
    static HtAudio instance;
    
private:
    // Constructor. Private for singleton
    HtAudio();

    // Destructor. Private for singleton
    ~HtAudio();

    // Disable copy constructor for singleton
    HtAudio(const HtAudio& other) = delete;

    // Keeps a list of the SoundIndex for each filename, to avoid
    // loading more than once.
    std::map<std::string, SoundIndex> m_soundFileMap;

    // Maps the sound index to the SDL Chunk
    std::map<SoundIndex, Mix_Chunk*> m_soundMap;

    // The next index to use when loading a sound file
    SoundIndex m_nextSoundIndex;

    // Pointer to the music that is loaded
    Mix_Music* m_pMusic;

    // Music volume from 0 to MIX_MAX_VOLUME
    int m_musicVolume;
};

