/*#include "ImguiTextureManager.h"

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wincodec.h>
#include <wrl/client.h>

#include <unordered_map>
#include <mutex>
#include <vector>
#include <algorithm>
#include <helper.hpp>

using Microsoft::WRL::ComPtr;

// ===== EXTERN YOUR DX12 GLOBALS FROM YOUR DX12 HOOK CPP =====
// These must be the REAL game ones you capture (hkExecuteCommandLists for queue, swapchain device, etc.)
extern ID3D12Device* g_pd3dDevice;
extern ID3D12DescriptorHeap* g_pd3dSrvDescHeap;   // shader-visible CBV_SRV_UAV heap used for ImGui
extern ID3D12CommandQueue* g_pd3dCommandQueue;

// You must expose these too (or change to your own values):
extern UINT                       g_srvDescriptorSize;
extern UINT                       g_nextSrvIndex;      // 0 reserved for ImGui font SRV

// Optional but helpful if you have it
extern IDXGISwapChain3* g_pSwapChain;

// If you don't have a global swapchain pointer, the manager still works.
// It uses its own upload allocator/list and doesn't need your frame allocators.

// ============================================================

static inline UINT64 AlignUp(UINT64 v, UINT64 align) { return (v + (align - 1)) & ~(align - 1); }

struct Dx12ImguiTexture
{
    ComPtr<ID3D12Resource> texture;  // DEFAULT heap texture
    UINT                   srvIndex = 0;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
    int width = 0, height = 0;
};

static std::unordered_map<std::wstring, Dx12ImguiTexture> g_cache;
static std::mutex g_mtx;

// Internal upload objects (so we never stomp your ImGui command list)
static bool g_inited = false;
static ComPtr<ID3D12CommandAllocator>    g_uploadAlloc;
static ComPtr<ID3D12GraphicsCommandList> g_uploadList;
static ComPtr<ID3D12Fence>               g_fence;
static UINT64                            g_fenceValue = 0;
static HANDLE                            g_fenceEvent = nullptr;

// WIC factory (PNG decode)
static ComPtr<IWICImagingFactory> g_wicFactory;

static D3D12_CPU_DESCRIPTOR_HANDLE CpuSrvHandle(UINT idx)
{
    D3D12_CPU_DESCRIPTOR_HANDLE h = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
    h.ptr += (SIZE_T)idx * (SIZE_T)g_srvDescriptorSize;
    return h;
}

static D3D12_GPU_DESCRIPTOR_HANDLE GpuSrvHandle(UINT idx)
{
    D3D12_GPU_DESCRIPTOR_HANDLE h = g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();
    h.ptr += (UINT64)idx * (UINT64)g_srvDescriptorSize;
    return h;
}

static bool EnsureWIC()
{
    if (g_wicFactory)
        return true;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory, nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&g_wicFactory));

    return SUCCEEDED(hr) && g_wicFactory;
}

static bool LoadPNG_WIC_RGBA8(const std::wstring& path, std::vector<uint8_t>& outRGBA, int& outW, int& outH)
{
    outRGBA.clear();
    outW = outH = 0;

    if (!EnsureWIC())
        return false;

    ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = g_wicFactory->CreateDecoderFromFilename(
        path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

    if (FAILED(hr) || !decoder)
        return false;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr) || !frame)
        return false;

    UINT w = 0, h = 0;
    hr = frame->GetSize(&w, &h);
    if (FAILED(hr) || w == 0 || h == 0)
        return false;

    // Convert to 32bpp RGBA
    ComPtr<IWICFormatConverter> conv;
    hr = g_wicFactory->CreateFormatConverter(&conv);
    if (FAILED(hr) || !conv)
        return false;

    hr = conv->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom);

    if (FAILED(hr))
        return false;

    const UINT stride = w * 4;
    const UINT imageSize = stride * h;

    outRGBA.resize(imageSize);
    hr = conv->CopyPixels(nullptr, stride, imageSize, outRGBA.data());
    if (FAILED(hr))
    {
        outRGBA.clear();
        return false;
    }

    outW = (int)w;
    outH = (int)h;
    return true;
}

static bool WaitForFence()
{
    if (!g_fence || !g_pd3dCommandQueue)
        return false;

    const UINT64 waitVal = ++g_fenceValue;

    HRESULT hr = g_pd3dCommandQueue->Signal(g_fence.Get(), waitVal);
    if (FAILED(hr))
        return false;

    if (g_fence->GetCompletedValue() < waitVal)
    {
        hr = g_fence->SetEventOnCompletion(waitVal, g_fenceEvent);
        if (FAILED(hr))
            return false;

        WaitForSingleObject(g_fenceEvent, INFINITE);
    }
    return true;
}

static bool UploadRGBA_ToTextureSRV(const void* rgbaPixels, int width, int height, Dx12ImguiTexture& outTex)
{
    if (!g_pd3dDevice || !g_pd3dSrvDescHeap || !g_pd3dCommandQueue)
        return false;
    if (!g_uploadAlloc || !g_uploadList || !g_fence || !g_fenceEvent)
        return false;
    if (!rgbaPixels || width <= 0 || height <= 0)
        return false;

    // --- Create DEFAULT heap texture ---
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = (UINT)width;
    texDesc.Height = (UINT)height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_HEAP_PROPERTIES heapDefault = {};
    heapDefault.Type = D3D12_HEAP_TYPE_DEFAULT;

    ComPtr<ID3D12Resource> texture;
    HRESULT hr = g_pd3dDevice->CreateCommittedResource(
        &heapDefault, D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&texture));

    if (FAILED(hr) || !texture)
        return false;

    // --- Create UPLOAD heap buffer for pixels ---
    const UINT bytesPerPixel = 4;
    const UINT rowPitch = (UINT)width * bytesPerPixel;
    const UINT alignedRowPitch = (UINT)AlignUp(rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
    const UINT64 uploadSize = (UINT64)alignedRowPitch * (UINT64)height;

    D3D12_HEAP_PROPERTIES heapUpload = {};
    heapUpload.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC bufDesc = {};
    bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufDesc.Width = uploadSize;
    bufDesc.Height = 1;
    bufDesc.DepthOrArraySize = 1;
    bufDesc.MipLevels = 1;
    bufDesc.SampleDesc.Count = 1;
    bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    ComPtr<ID3D12Resource> upload;
    hr = g_pd3dDevice->CreateCommittedResource(
        &heapUpload, D3D12_HEAP_FLAG_NONE,
        &bufDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&upload));

    if (FAILED(hr) || !upload)
        return false;

    // Map + copy with row alignment
    uint8_t* mapped = nullptr;
    D3D12_RANGE noRead = { 0, 0 };
    hr = upload->Map(0, &noRead, (void**)&mapped);
    if (FAILED(hr) || !mapped)
        return false;

    const uint8_t* src = (const uint8_t*)rgbaPixels;
    for (int y = 0; y < height; ++y)
    {
        memcpy(mapped + (size_t)alignedRowPitch * y,
            src + (size_t)rowPitch * y,
            rowPitch);
    }
    upload->Unmap(0, nullptr);

    // Record upload commands on our private upload list
    g_uploadAlloc->Reset();
    g_uploadList->Reset(g_uploadAlloc.Get(), nullptr);

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
    footprint.Offset = 0;
    footprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    footprint.Footprint.Width = (UINT)width;
    footprint.Footprint.Height = (UINT)height;
    footprint.Footprint.Depth = 1;
    footprint.Footprint.RowPitch = alignedRowPitch;

    D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
    srcLoc.pResource = upload.Get();
    srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLoc.PlacedFootprint = footprint;

    D3D12_TEXTURE_COPY_LOCATION dstLoc = {};
    dstLoc.pResource = texture.Get();
    dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLoc.SubresourceIndex = 0;

    g_uploadList->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = texture.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    g_uploadList->ResourceBarrier(1, &barrier);

    g_uploadList->Close();

    ID3D12CommandList* lists[] = { g_uploadList.Get() };
    g_pd3dCommandQueue->ExecuteCommandLists(1, lists);

    // Wait so upload buffer can be freed safely and texture is ready immediately
    if (!WaitForFence())
        return false;

    // Allocate SRV slot from shared ImGui heap
    const UINT srvIndex = g_nextSrvIndex++;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu = CpuSrvHandle(srvIndex);
    D3D12_GPU_DESCRIPTOR_HANDLE gpu = GpuSrvHandle(srvIndex);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;

    g_pd3dDevice->CreateShaderResourceView(texture.Get(), &srvDesc, cpu);

    outTex.texture = texture;
    outTex.srvIndex = srvIndex;
    outTex.gpuHandle = gpu;
    outTex.width = width;
    outTex.height = height;

    return true;
}

namespace ImguiTextureManager
{
    void Init()
    {
        if (g_inited)
            return;

        // Must have real game objects
        if (!g_pd3dDevice || !g_pd3dSrvDescHeap || !g_pd3dCommandQueue || g_srvDescriptorSize == 0)
            return;

        // Create our private upload allocator/list + fence
        if (!g_uploadAlloc)
        {
            if (FAILED(g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_uploadAlloc))))
                return;
        }

        if (!g_uploadList)
        {
            if (FAILED(g_pd3dDevice->CreateCommandList(
                0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_uploadAlloc.Get(), nullptr, IID_PPV_ARGS(&g_uploadList))))
                return;
            g_uploadList->Close();
        }

        if (!g_fence)
        {
            if (FAILED(g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence))))
                return;
            g_fenceValue = 0;
        }

        if (!g_fenceEvent)
        {
            g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (!g_fenceEvent)
                return;
        }

        // WIC can be created lazily, but doing it here is fine too
        EnsureWIC();

        g_inited = true;
        LOG("[ImguiTextureManager] Init OK\n");
    }

    void Shutdown()
    {
        std::lock_guard<std::mutex> lock(g_mtx);

        g_cache.clear();

        g_wicFactory.Reset();

        g_uploadList.Reset();
        g_uploadAlloc.Reset();
        g_fence.Reset();

        if (g_fenceEvent)
        {
            CloseHandle(g_fenceEvent);
            g_fenceEvent = nullptr;
        }

        g_inited = false;
        LOG("[ImguiTextureManager] Shutdown\n");
    }

    void ClearCache()
    {
        std::lock_guard<std::mutex> lock(g_mtx);
        g_cache.clear();
        LOG("[ImguiTextureManager] Cache cleared\n");
    }

    ImguiTexView GetOrLoadPNG(const std::wstring& path)
    {
        Init(); // safe to call repeatedly

        // If not ready yet, return empty and let caller retry next frame
        if (!g_inited)
            return {};

        std::lock_guard<std::mutex> lock(g_mtx);

        auto it = g_cache.find(path);
        if (it != g_cache.end() && it->second.gpuHandle.ptr)
        {
            return { (uint64_t)it->second.gpuHandle.ptr, it->second.width, it->second.height };
        }

        // Decode PNG -> RGBA8
        std::vector<uint8_t> rgba;
        int w = 0, h = 0;
        if (!LoadPNG_WIC_RGBA8(path, rgba, w, h))
        {
            // Cache a "failed" entry? Up to you. For now just return empty.
            // LOG("[ImguiTextureManager] PNG decode failed: %ls\n", path.c_str());
            return {};
        }

        Dx12ImguiTexture tex;
        if (!UploadRGBA_ToTextureSRV(rgba.data(), w, h, tex))
        {
            // LOG("[ImguiTextureManager] Upload failed: %ls\n", path.c_str());
            return {};
        }

        // Insert into cache
        auto [insIt, ok] = g_cache.emplace(path, std::move(tex));
        if (!ok)
            return {};

        return { (uint64_t)insIt->second.gpuHandle.ptr, insIt->second.width, insIt->second.height };
    }
}*/
