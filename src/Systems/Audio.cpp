#include "Audio.h"

#include <SDL_mixer.h>
#include <thread>

#include "Util/Print.h"

namespace whal {

Music::~Music() {
    if (mData) {
        Mix_FreeMusic(mData);
    }
}

std::optional<Error> Music::load(const char* path) {
    mPath = path;
    mData = Mix_LoadMUS(path);
    if (mData == nullptr) {
        return Error(Mix_GetError());
    }
    return std::nullopt;
}

AudioClip::~AudioClip() {
    if (mData) {
        Mix_FreeChunk(mData);
    }
}

std::optional<Error> AudioClip::load(const char* path) {
    mPath = path;
    // can be mp3 or wav (or others?)
    mData = Mix_LoadWAV(path);
    if (mData == nullptr) {
        return Error(Mix_GetError());
    }
    return std::nullopt;
}

AudioPlayer::AudioPlayer() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 8, 2048) < 0) {
        return;
    }
    mIsValid = true;
}

AudioPlayer::~AudioPlayer() {
    if (mIsValid) {
        Mix_CloseAudio();
    }
}

void AudioPlayer::start() {
    mMusicThread = std::thread(&AudioPlayer::playerThread, this);
}

void AudioPlayer::await() {
    mMusicThread.join();
}

void AudioPlayer::end() {
    mIsValid = false;
}

void AudioPlayer::play(Music& music) {
    if (!music.isValid()) {
        return;
    }

    mQueuedMusic = &music;
    mCondition.notify_one();
}

void AudioPlayer::play(AudioClip& clip) const {
    if (!clip.isValid()) {
        return;
    }

    // TODO should probably track which channels are being used + set some aside for ECS?
    // arg 1: -1 to choose any channel
    // arg 3:
    // 0 == play once and stop
    // -1 == play forever?
    auto channel = Mix_PlayChannel(-1, clip.get(), 0);
    if (channel == -1) {
        print("couldn't play audio clip");
    }
}

void AudioPlayer::playerThread() {
    while (mIsValid) {
        std::unique_lock<std::mutex> lock(mMutex);
        mCondition.wait(lock, [this] { return mQueuedMusic != nullptr || !mIsValid; });
        if (mQueuedMusic == nullptr || !mQueuedMusic->isValid()) {
            mQueuedMusic = nullptr;
            continue;
        }

        // 0 == play once and stop
        // -1 == play forever?
        Mix_PlayMusic(mQueuedMusic->get(), 0);
        mQueuedMusic = nullptr;
        while (Mix_PlayingMusic()) {
            if (mQueuedMusic || !mIsValid) {
                break;
            }
            SDL_Delay(100);
        }
    }
}

}  // namespace whal
