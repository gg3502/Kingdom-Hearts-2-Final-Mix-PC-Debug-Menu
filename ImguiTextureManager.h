#pragma once
#include <string>
#include <cstdint>

struct ImguiTexView
{
    uint64_t gpuHandlePtr = 0;   // cast to ImTextureID in ImGui::Image/AddImage
    int width = 0;
    int height = 0;
    bool valid() const { return gpuHandlePtr != 0; }
};

namespace ImguiTextureManager
{
    // Call once after your DX12 globals exist (device/heap/queue) and ImGui_ImplDX12_Init() is done.
    // You can call it every frame; it will initialize only once.
    void Init();

    // Release all cached textures + internal upload objects.
    void Shutdown();

    // Release cached textures only (keeps upload objects alive).
    void ClearCache();

    // Load/cache a PNG as an ImGui texture (SRV in your shared ImGui heap).
    // Returns {gpuHandlePtr,width,height} or {} if not ready / failed.
    ImguiTexView GetOrLoadPNG(const std::wstring& path);
}
