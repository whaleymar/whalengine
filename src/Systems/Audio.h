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

// TODO should be able to set volume
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
    void stopMusic();
    void stopAll();
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
    bool mIsValid = false;
    bool mIsMusicStopSignal = false;
    bool mIsTerminated = false;
};

// TODO audio event manager

/*
* API Design:

Define event classes like this

    public class ThrowEvent : UnityEvent<Vector3> { }
    public class SlideEvent : UnityEvent<Vector3> { }
    public class ItemPickupEvent : UnityEvent<Vector3> { }

Declare event listeners like this:

    private UnityAction<Vector3> throwEventListener;
    private UnityAction<Vector3> slideEventListener;
    private UnityAction<Vector3> pickupEventListener;

And define them on init:

    throwEventListener = new UnityAction<Vector3>(throwEventHandler);
    slideEventListener = new UnityAction<Vector3>(slideEventHandler);
    pickupEventListener = new UnityAction<Vector3>(pickupEventHandler);

There's some master Event Listener which listens for these events, and the Audio Event Manager sets itself as an observer on init:

    EventManager.StartListening<ThrowEvent, Vector3>(throwEventListener);
    EventManager.StartListening<SlideEvent, Vector3>(slideEventListener);
    EventManager.StartListening<ItemPickupEvent, Vector3>(pickupEventListener);

For completeness, in destructor:

    EventManager.StopListening<ThrowEvent, Vector3>(throwEventListener);
    EventManager.StopListening<SlideEvent, Vector3>(slideEventListener);
    EventManager.StopListening<ItemPickupEvent, Vector3>(pickupEventListener);

Then each event has a handler which plays audio with AudioPlayer and may do additional logic

    void slideEventHandler(Vector3 worldPos)
    {
        if (eventSound3DPrefab)
        {
            EventSound3D snd = Instantiate(eventSound3DPrefab, worldPos, Quaternion.identity, null);
            snd.audioSrc.clip = this.slideAudio;
            snd.audioSrc.volume = 0.25f;

            snd.audioSrc.Play();
        }
    }

Notably, sound effects like this are loaded into memory as a class variable on startup(this.slideAudio). Music can probably be managed with a
unique_ptr that is loaded/unloaded at will

*/

}  // namespace whal
