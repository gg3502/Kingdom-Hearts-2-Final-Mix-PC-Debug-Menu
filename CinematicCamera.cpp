#include "CinematicCamera.h"
#include "obj_camera.hpp"   // whatever defines obj_camera, Vector4
#include <algorithm>

// --------- helper functions to convert between obj_camera and our state ---------

static CinematicCameraState CaptureFromObjCamera(const obj_camera* cam)
{
    CinematicCameraState s{};
    s.eye = cam->m_eye;
    s.at = cam->m_at;
    s.yaw = cam->m_Yaw;
    s.pitch = cam->m_Pitch;
    s.fov = cam->m_fov;
    return s;
}

static void ApplyToObjCamera(const CinematicCameraState& s, obj_camera* cam)
{
    cam->m_eye = s.eye;
    cam->m_at = s.at;
    cam->m_eyeTarget = s.eye;
    cam->m_atTarget = s.at;
    cam->m_Yaw = s.yaw;
    cam->m_Pitch = s.pitch;
    cam->m_fov = s.fov;
}

// simple clamp
static float clampFloat(float x, float a, float b)
{
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

// --------- CinematicCamera implementation ---------

CinematicCamera::CinematicCamera()
    : m_objCamera(nullptr)
    , m_mode(Mode::Disabled)
    , m_playbackTime(0.0f)
    , m_loop(false)
{
}

void CinematicCamera::attachTarget(obj_camera* cam)
{
    m_objCamera = cam;
}

void CinematicCamera::setMode(Mode m)
{
    m_mode = m;
}

void CinematicCamera::removeLastKeyframe()
{
    if (!m_keyframes.empty())
        m_keyframes.pop_back();
}


CinematicCamera::Mode CinematicCamera::mode() const
{
    return m_mode;
}

bool CinematicCamera::isPlaying() const
{
    return m_mode == Mode::Playing;
}

bool CinematicCamera::isEditing() const
{
    return m_mode == Mode::Editing;
}

void CinematicCamera::addKeyframe(float timelineTime)
{
    if (!m_objCamera)
        return;

    // default spacing in seconds between auto-timed keyframes
    const float defaultStep = 48.0f;   // tweak: 3.0f, 5.0f, etc.

    if (!m_keyframes.empty())
    {
        float lastTime = m_keyframes.back().time;

        // If the user hasn't moved the timeline forward,
        // auto-place this keyframe further in the future.
        if (timelineTime <= lastTime)
        {
            timelineTime = lastTime + defaultStep;
        }
    }

    CinematicKeyframe kf;
    kf.time = timelineTime;
    kf.state = CaptureFromObjCamera(m_objCamera);
    m_keyframes.push_back(kf);

    std::sort(m_keyframes.begin(), m_keyframes.end(),
        [](const CinematicKeyframe& a, const CinematicKeyframe& b) {
            return a.time < b.time;
        });
}

void CinematicCamera::removeKeyframe(size_t index)
{
    if (index >= m_keyframes.size())
        return;

    m_keyframes.erase(m_keyframes.begin() + index);
}

void CinematicCamera::replaceKeyframes(const std::vector<CinematicKeyframe>& newKeys)
{
    m_keyframes = newKeys;
    m_playbackTime = 0.0f;
    m_mode = Mode::Editing;  // back to editing/idle
}



void CinematicCamera::clearKeyframes()
{
    m_keyframes.clear();
}

void CinematicCamera::applyKeyframeInstant(size_t index)
{
    if (!m_objCamera)
        return;

    if (index >= m_keyframes.size())
        return;

    // Apply stored camera state directly to the game's camera
    ApplyToObjCamera(m_keyframes[index].state, m_objCamera);

    // Ensure we're not "playing"
    m_mode = Mode::Editing;
}


void CinematicCamera::startPlayback(bool loop)
{
    if (m_keyframes.size() < 2 || !m_objCamera)
        return;

    m_playbackTime = m_keyframes.front().time;
    m_loop = loop;
    m_mode = Mode::Playing;
}


void CinematicCamera::stopPlayback()
{
    m_mode = Mode::Editing;
}

void CinematicCamera::update(float dt)
{
    if (!m_objCamera)
        return;

    if (m_mode == Mode::Playing)
    {
        float speed = m_playbackSpeed;
        if (speed < 0.0f) speed = 0.0f;
        if (speed > 10.0f) speed = 10.0f;   // clamped just in case

        updatePlayback(dt * speed);
    }
}



void CinematicCamera::updatePlayback(float dt)
{
    if (m_keyframes.empty())
        return;

    m_playbackTime += dt;

    const float startTime = m_keyframes.front().time;
    const float endTime = m_keyframes.back().time;
    const float duration = endTime - startTime;

    if (duration <= 0.0f)
    {
        ApplyToObjCamera(m_keyframes.front().state, m_objCamera);
        return;
    }

    if (m_loop)
    {
        while (m_playbackTime > endTime)
            m_playbackTime -= duration;
    }
    else
    {
        if (m_playbackTime >= endTime)
        {
            // Snap to the last keyframe
            ApplyToObjCamera(m_keyframes.back().state, m_objCamera);

            m_playbackTime = endTime;

            // Auto-stop playback when not looping
            m_mode = Mode::Editing;

            return;
        }
    }

    if (m_playbackTime <= startTime)
    {
        ApplyToObjCamera(m_keyframes.front().state, m_objCamera);
        return;
    }

    // find surrounding keyframes
    size_t i = 0;
    while (i + 1 < m_keyframes.size() &&
        m_keyframes[i + 1].time < m_playbackTime)
    {
        ++i;
    }

    const CinematicKeyframe& A = m_keyframes[i];
    const CinematicKeyframe& B = m_keyframes[i + 1];

    float segmentDuration = B.time - A.time;
    if (segmentDuration <= 0.0f)
    {
        ApplyToObjCamera(A.state, m_objCamera);
        return;
    }

    float t = (m_playbackTime - A.time) / segmentDuration;
    t = clampFloat(t, 0.0f, 1.0f);

    // linear interpolation (you can upgrade to easing later)
    CinematicCameraState result{};

    result.eye.x = A.state.eye.x + (B.state.eye.x - A.state.eye.x) * t;
    result.eye.y = A.state.eye.y + (B.state.eye.y - A.state.eye.y) * t;
    result.eye.z = A.state.eye.z + (B.state.eye.z - A.state.eye.z) * t;
    result.eye.w = 1.0f;

    result.at.x = A.state.at.x + (B.state.at.x - A.state.at.x) * t;
    result.at.y = A.state.at.y + (B.state.at.y - A.state.at.y) * t;
    result.at.z = A.state.at.z + (B.state.at.z - A.state.at.z) * t;
    result.at.w = 1.0f;

    result.yaw = A.state.yaw + (B.state.yaw - A.state.yaw) * t;
    result.pitch = A.state.pitch + (B.state.pitch - A.state.pitch) * t;
    result.fov = A.state.fov + (B.state.fov - A.state.fov) * t;

    ApplyToObjCamera(result, m_objCamera);
}
