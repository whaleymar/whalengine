#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <condition_variable>
#include <mutex>
#include <optional>

#include "Util/Expected.h"

namespace whal {

class Music {
public:
    Music() = default;
    ~Music();

    Music(const Music&) = delete;
    void operator=(const Music&) = delete;

    std::optional<Error> load(const char* path);
    bool isValid() const { return mData != nullptr; }
    Mix_Music* get() const { return mData; }

private:
    const char* mPath = nullptr;
    Mix_Music* mData = nullptr;
};

class AudioClip {
public:
    AudioClip() = default;
    ~AudioClip();

    AudioClip(const AudioClip&) = delete;
    void operator=(const AudioClip&) = delete;

    std::optional<Error> load(const char* path);
    bool isValid() const { return mData != nullptr; }
    Mix_Chunk* get() const { return mData; }

private:
    const char* mPath;
    Mix_Chunk* mData = nullptr;
};

class AudioPlayer {
public:
    static AudioPlayer& instance() {
        static AudioPlayer instance;
        return instance;
    }

    void start();
    void await();
    void end();

    void play(Music& music);
    void play(AudioClip& clip) const;
    bool isValid() const { return mIsValid; }

private:
    AudioPlayer();
    ~AudioPlayer();

    AudioPlayer(const AudioPlayer&) = delete;
    void operator=(const AudioPlayer&) = delete;

    void playerThread();

    std::mutex mMutex;
    std::thread mMusicThread;
    std::condition_variable mCondition;
    Music* mQueuedMusic = nullptr;
    bool mIsValid = false;  // doubles as termination condition
};

// TODO audio event manager

}  // namespace whal
