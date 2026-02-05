#include "HtAudio.h"
#include "ErrorLogger.h"

HtAudio HtAudio::instance;


HtAudio::HtAudio()
{
    m_nextSoundIndex = SoundIndex(0);
    m_pMusic = nullptr;
    m_musicVolume=SDL_MIX_MAXVOLUME;
}

HtAudio::~HtAudio()
{
    ReleaseAllSounds();
    Mix_Quit();
}

Result HtAudio::Initialise()
{
    // Start the audio system withh a playback sample frequency of 44100
    // Two channels (i.e. stereo)
    // Buffer size is 2048
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        ErrorLogger::Write("Could not initialise SDK_mixer.");
        ErrorLogger::Write(Mix_GetError());
        return Result::FAILURE;
    }
    else
    {
        return Result::SUCCESS;
    }
}

SoundIndex HtAudio::LoadSound(std::string filename)
{
    // See if file is already loaded
    auto it = m_soundFileMap.find(filename);
    if (it != m_soundFileMap.end())
    {
        // If it is, return the existing sound index
        return it->second;
    }

    // Pointer to the object containing the sound buffer
    Mix_Chunk* pChunk = nullptr;
    pChunk = Mix_LoadWAV(filename.c_str());
    if (pChunk == nullptr)
    {
        // Try again with "assets/" in front
        pChunk = Mix_LoadWAV((std::string("assets/") + filename).c_str());
    }
    if (pChunk == nullptr)
    {
        ErrorLogger::Write("Could not load wav: "+filename);
        ErrorLogger::Write(Mix_GetError());
        return NOSOUND;
    }

    // Insert the filename and buffer into the maps
    m_soundFileMap.insert(std::pair<std::string, SoundIndex>(filename, m_nextSoundIndex));
    m_soundMap.insert(std::pair<SoundIndex, Mix_Chunk*>(m_nextSoundIndex, pChunk));

    // Good grief. All this casting is a pain, but it allows SoundIndex to be type checked.
    // Effectively this is ++m_nextSoundIndex
    m_nextSoundIndex = SoundIndex(int(m_nextSoundIndex)+1);

    return SoundIndex(int(m_nextSoundIndex) - 1);
}

int HtAudio::Play(SoundIndex sound, bool playLooping)
{
// Find the sound
    auto it = m_soundMap.find(sound);
    if (it == m_soundMap.end())
    {
        ErrorLogger::Write("Play() Cannot find sound with index: "+std::to_string(int(sound)));
        return -1;
    }

    int loops = 0;
    if (playLooping) loops = -1;         // Will loop "infinitely"
    // Hmm. Can set loops to a set number. Is there a use for that?
    // Should I include it or will it confuse the students?
    int channel = Mix_PlayChannel(-1, it->second, loops);

    if (channel < 0)
    {
        ErrorLogger::Write("Could not play sound with index: "+std::to_string(int(sound)));
        ErrorLogger::Write(Mix_GetError());
    }

    return channel;
}

Result HtAudio::Stop(int channel)
{
    if (Mix_HaltChannel(channel) == 0)
    {
        return Result::SUCCESS;
    }
    else
    {
        return Result::FAILURE;
    }
}

void HtAudio::StopAllChannels()
{
    // Loop through all possible channels
    for (int i = 0; i < MIX_CHANNELS; ++i)
        Mix_HaltChannel(i);
}

Result HtAudio::SetChannelVolume(int channel, double volume)
{
    int vol = 0;
    if (volume < 0)
    {
        vol = 0;            // Zero volume
    }
    else if (volume < 1.0)
    {
        vol = int(volume * MIX_MAX_VOLUME);
    }
    else
    {
        vol = MIX_MAX_VOLUME;          // Max volume
    }

    // No useful error return from this function.
    // It actually returns the previous volume
    Mix_Volume(channel, vol);

    return Result::SUCCESS;
}

Result HtAudio::SetMasterVolume(double volume)
{
    int vol = 0;
    if (volume < 0)
    {
        vol = 0;            // Zero volume
    }
    else if (volume < 1.0)
    {
        vol = int(volume * MIX_MAX_VOLUME);
    }
    else
    {
        vol = MIX_MAX_VOLUME;          // Max volume
    }

    // No useful error return from this function.
    // It actually returns the previous volume
    Mix_MasterVolume(vol);

    return Result::SUCCESS;
}

Result HtAudio::SetSoundVolume(SoundIndex sound, double volume)
{
    // Find the sound
    auto it = m_soundMap.find(sound);
    if (it == m_soundMap.end())
    {
        ErrorLogger::Write("SetSoundVolume() Cannot find sound with index: " + int(sound));
        return Result::FAILURE;
    }

    int vol = 0;
    if (volume < 0)
    {
        vol = 0;            // Zero volume
    }
    else if (volume < 1.0)
    {
        vol = int(volume * MIX_MAX_VOLUME);
    }
    else
    {
        vol = MIX_MAX_VOLUME;          // Max volume
    }
    
    // No useful error return from this function.
    // It actually returns the previos volume
    Mix_VolumeChunk(it->second, vol);

    return Result::SUCCESS;

}

Result HtAudio::SetPan(int channel, double pan)
{
    int leftPan = int(pan * 255);
    if (leftPan > 255) leftPan = 255;
    if (leftPan < 0) leftPan = 0;
    int err = Mix_SetPanning(channel, leftPan, 255 - leftPan);

    if (err == 0)
    {
        ErrorLogger::Write("Could not pan channel: " + channel);
        ErrorLogger::Write(Mix_GetError());
        return Result::FAILURE;
    }
    else
    {
        return Result::SUCCESS;
    }
}

Result HtAudio::LoadMusic(std::string filename)
{
    Mix_FreeMusic(m_pMusic);
    m_pMusic = nullptr;
    
    m_pMusic = Mix_LoadMUS(filename.c_str());
    if (m_pMusic == nullptr)
    {
        // Try again with "assets/" in front
        m_pMusic = Mix_LoadMUS((std::string("assets/") + filename).c_str());
    }
    if (m_pMusic == nullptr)
    {
        ErrorLogger::Write("Failed to load music file: "+filename);
        ErrorLogger::Write(Mix_GetError());
        return Result::FAILURE;
    }
    else
    {
        return Result::SUCCESS;
    }
}

Result HtAudio::PlayMusic(bool loop)
{
    if (IsMusicPlaying())
        StopMusic();
    if (m_pMusic)
    {
        int flag = 0;
        if (loop) flag = -1;
        int r = Mix_PlayMusic(m_pMusic, flag);
        Mix_VolumeMusic(int(MIX_MAX_VOLUME * m_musicVolume));
        Mix_VolumeMusic(m_musicVolume);
        if (r == 0)
        {
            return Result::SUCCESS;
        }
        else
        {
            ErrorLogger::Write("Failed to play music.");
            ErrorLogger::Write(Mix_GetError());
            return Result::FAILURE;
        }
    }
    else
    {
        ErrorLogger::Write("Cannot play music. Not loaded.");
        return Result::FAILURE;
    }
}

void HtAudio::StopMusic()
{
    Mix_HaltMusic();
}

bool HtAudio::IsMusicPlaying() const
{
    return Mix_PlayingMusic()!=0;
}

void HtAudio::PauseMusic()
{
    Mix_PauseMusic();
}

void HtAudio::ResumeMusic()
{
    Mix_ResumeMusic();
}

void HtAudio::SetMusicVolume(double volume)
{
    m_musicVolume = int(MIX_MAX_VOLUME * volume);
    if (m_musicVolume < 0) m_musicVolume = 0;
    if (m_musicVolume > MIX_MAX_VOLUME) m_musicVolume = MIX_MAX_VOLUME;

    Mix_VolumeMusic(m_musicVolume);
}

void HtAudio::ReleaseAllSounds()
{
    // Start at the beginning
    std::map<SoundIndex, Mix_Chunk*>::iterator sndit = m_soundMap.begin();

    // Loop through all textures
    for (; sndit != m_soundMap.end(); sndit++)
    {
        if (sndit->second)
        {
            // Release the texture
            Mix_FreeChunk(sndit->second);
        }
    }

    StopMusic();
    Mix_FreeMusic(m_pMusic);
    m_pMusic = nullptr;

    // Empty the two lists
    m_soundMap.clear();
    m_soundFileMap.clear();
    m_nextSoundIndex = SoundIndex(0);
}

void HtAudio::ReleaseSound(SoundIndex soundToRelease)
{
    std::map<SoundIndex, Mix_Chunk*>::iterator sndit = m_soundMap.find(soundToRelease);
    if (sndit != m_soundMap.end())
    {
        Mix_FreeChunk(sndit->second);
        m_soundMap.erase(sndit);
    }
}

void HtAudio::Shutdown()
{
    ReleaseAllSounds();
}
