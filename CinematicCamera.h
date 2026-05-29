#pragma once

#include <vector>
#include "common_types.hpp"

// forward declare your game types
struct obj_camera;
struct Vector4;


// --- State that the cinematic system controls ---
struct CinematicCameraState
{
    Vector4 eye;
    Vector4 at;
    float   yaw;
    float   pitch;
    float   fov;
};

// --- One keyframe on the cinematic timeline ---
struct CinematicKeyframe
{
    float time;                 // seconds on timeline
    CinematicCameraState state; // camera state at that time
};

class CinematicCamera
{
public:
    enum class Mode { Disabled, Editing, Playing };

    CinematicCamera();

    void attachTarget(obj_camera* cam);

    const std::vector<CinematicKeyframe>& getKeyframes() const { return m_keyframes; }

    float currentPlaybackTime() const { return m_playbackTime; }

    void setMode(Mode m);
    Mode mode() const;

    float getPlaybackSpeed() const { return m_playbackSpeed; }
    void  setPlaybackSpeed(float s) { m_playbackSpeed = s; }

    bool isPlaying() const;
    bool isEditing() const;

    void addKeyframe(float timelineTime);
    void clearKeyframes();

    void startPlayback(bool loop = false);
    void stopPlayback();

    void removeLastKeyframe();
    void applyKeyframeInstant(size_t index);

    void removeKeyframe(size_t index);
    void replaceKeyframes(const std::vector<CinematicKeyframe>& newKeys);

    bool isLooping() const { return m_loop; }


    // call once per frame
    void update(float dt);

private:
    obj_camera* m_objCamera;
    std::vector<CinematicKeyframe> m_keyframes;

    Mode  m_mode;
    float m_playbackTime;
    bool  m_loop;

    float m_playbackSpeed = 1.0f;

    void updatePlayback(float dt);
};
