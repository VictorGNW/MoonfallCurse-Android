#include <adrenotools/driver.h>
#include <android/log.h>
#include <dlfcn.h>
#include <jni.h>

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_ANDROID_KHR
#include <vulkan/vulkan.h>

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace {
std::mutex loaderMutex;
void* loaderHandle = nullptr;
bool turnipSelected = false;

template <typename Function>
Function VulkanEntry(const char* name) {
    std::lock_guard lock(loaderMutex);
    if (!loaderHandle)
        loaderHandle = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    auto entry = loaderHandle ? reinterpret_cast<Function>(dlsym(loaderHandle, name)) : nullptr;
    if (!entry) {
        __android_log_print(ANDROID_LOG_FATAL, "TriAevum", "Missing Vulkan loader entry: %s", name);
        std::abort();
    }
    return entry;
}

constexpr uint32_t kQualcommVendorId = 0x5143;

int AdrenoFamily(const VkPhysicalDeviceProperties& gpu) {
    if (gpu.vendorID != kQualcommVendorId) return 0;
    std::string_view name(gpu.deviceName);
    if (name.substr(0, 6) != "Adreno") return 1;
    name.remove_prefix(6);
    while (!name.empty() && name.front() == ' ') name.remove_prefix(1);
    if (name.substr(0, 4) == "(TM)") name.remove_prefix(4);
    while (!name.empty() && name.front() == ' ') name.remove_prefix(1);
    const auto digit = [](char c) { return c >= '0' && c <= '9'; };
    if (name.size() < 3 || !digit(name[0]) || !digit(name[1]) || !digit(name[2]) ||
        (name.size() > 3 && digit(name[3]))) return 1;
    return name[0] - '0';
}

int ProbeSystemAdrenoFamily() {
    void* loader = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (loader == nullptr) return 0;
    const auto create = reinterpret_cast<PFN_vkCreateInstance>(dlsym(loader, "vkCreateInstance"));
    const auto destroy = reinterpret_cast<PFN_vkDestroyInstance>(dlsym(loader, "vkDestroyInstance"));
    const auto enumerate = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
        dlsym(loader, "vkEnumeratePhysicalDevices"));
    const auto properties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(
        dlsym(loader, "vkGetPhysicalDeviceProperties"));

    VkInstanceCreateInfo info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    VkInstance instance{};
    int family = 0;
    if (create && destroy && enumerate && properties &&
        create(&info, nullptr, &instance) == VK_SUCCESS) {
        VkPhysicalDevice devices[16]{};
        uint32_t count = 16;
        const VkResult result = enumerate(instance, &count, devices);
        if (result == VK_SUCCESS || result == VK_INCOMPLETE) {
            for (uint32_t i = 0; i < count && i < 16; ++i) {
                VkPhysicalDeviceProperties gpu{};
                properties(devices[i], &gpu);
                const int candidate = AdrenoFamily(gpu);
                if (candidate != 0) {
                    family = candidate;
                    break;
                }
            }
        }
        destroy(instance, nullptr);
    }
    dlclose(loader);
    return family;
}

int SystemAdrenoFamily() {
    static const int family = ProbeSystemAdrenoFamily();
    return family;
}

bool SupportsTurnip(PFN_vkGetInstanceProcAddr getProc) {
    if (!getProc)
        return false;
    const auto create = reinterpret_cast<PFN_vkCreateInstance>(getProc(nullptr, "vkCreateInstance"));
    if (!create)
        return false;

    const char* extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME};
    VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app.apiVersion = VK_API_VERSION_1_2;
    VkInstanceCreateInfo info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    info.pApplicationInfo = &app;
    info.enabledExtensionCount = 2;
    info.ppEnabledExtensionNames = extensions;
    VkInstance instance{};
    if (create(&info, nullptr, &instance) != VK_SUCCESS)
        return false;

    const auto enumerate = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
        getProc(instance, "vkEnumeratePhysicalDevices"));
    const auto properties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
        getProc(instance, "vkGetPhysicalDeviceProperties2"));
    const auto destroy = reinterpret_cast<PFN_vkDestroyInstance>(
        getProc(instance, "vkDestroyInstance"));
    const auto enumerateExtensions = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(
        getProc(instance, "vkEnumerateDeviceExtensionProperties"));
    const auto surface = getProc(instance, "vkCreateAndroidSurfaceKHR");
    bool supported = false;
    uint32_t count = 0;
    if (destroy && enumerate && properties && enumerateExtensions && surface &&
        enumerate(instance, &count, nullptr) == VK_SUCCESS && count > 0 && count <= 16) {
        std::vector<VkPhysicalDevice> devices(count);
        if (enumerate(instance, &count, devices.data()) == VK_SUCCESS) {
            for (uint32_t i = 0; i < count; ++i) {
                VkPhysicalDeviceDriverProperties driver{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES};
                VkPhysicalDeviceProperties2 device{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &driver};
                properties(devices[i], &device);
                if (driver.driverID != VK_DRIVER_ID_MESA_TURNIP ||
                    device.properties.apiVersion < VK_API_VERSION_1_2)
                    continue;

                uint32_t extensionCount = 0;
                if (enumerateExtensions(devices[i], nullptr, &extensionCount, nullptr) != VK_SUCCESS ||
                    extensionCount == 0 || extensionCount > 1024)
                    continue;
                std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
                if (enumerateExtensions(devices[i], nullptr, &extensionCount, deviceExtensions.data()) != VK_SUCCESS)
                    continue;
                for (uint32_t extension = 0; extension < extensionCount; ++extension) {
                    if (std::strcmp(deviceExtensions[extension].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
                        supported = true;
                        break;
                    }
                }
                if (supported) {
                    __android_log_print(ANDROID_LOG_INFO, "TriAevum", "Turnip gpu=%s driver=%s",
                        device.properties.deviceName, driver.driverName);
                    break;
                }
            }
        }
    }
    if (destroy)
        destroy(instance, nullptr);
    return supported;
}

std::string Directory(JNIEnv* env, jstring path) {
    const char* value = env->GetStringUTFChars(path, nullptr);
    if (!value)
        return {};
    std::string directory = std::string(value) + "/";
    env->ReleaseStringUTFChars(path, value);
    return directory;
}
}

#include "android_vulkan_dispatch.inc"

// SDL and NRI obtain this symbol through dlsym on the proxy library.
extern "C" VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(
    VkInstance instance, const char* name) {
    return __wrap_vkGetInstanceProcAddr(instance, name);
}

extern "C" JNIEXPORT jint JNICALL
Java_org_triaevum_android_TurnipDriver_systemAdrenoFamily(JNIEnv*, jclass) {
    return SystemAdrenoFamily();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_triaevum_android_TurnipDriver_selectNative(
    JNIEnv* env, jclass, jstring nativeDir, jstring driverDir, jboolean bundled) {
    const std::string hooks = Directory(env, nativeDir);
    if (hooks.empty())
        return JNI_FALSE;
    const std::string driver = Directory(env, driverDir);
    if (driver.empty())
        return JNI_FALSE;

    std::lock_guard lock(loaderMutex);
    if (loaderHandle)
        return turnipSelected ? JNI_TRUE : JNI_FALSE;
    const int family = SystemAdrenoFamily();
    if (family == 0 || (bundled && family != 6)) return JNI_FALSE;
    void* handle = adrenotools_open_libvulkan(RTLD_NOW | RTLD_LOCAL, ADRENOTOOLS_DRIVER_CUSTOM,
        nullptr, hooks.c_str(), driver.c_str(), bundled ? "libturnip_bundled.so" : "libturnip.so", nullptr, nullptr);
    if (!handle)
        return JNI_FALSE;
    auto getProc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(handle, "vkGetInstanceProcAddr"));
    if (!SupportsTurnip(getProc)) {
        dlclose(handle);
        return JNI_FALSE;
    }

    const std::string proxy = hooks + "libtriaevum_vulkan.so";
    if (setenv("TRIAEVUM_ANDROID_TURNIP_LOADER", proxy.c_str(), 1) != 0 ||
        setenv("SDL_VULKAN_LIBRARY", proxy.c_str(), 1) != 0) {
        unsetenv("TRIAEVUM_ANDROID_TURNIP_LOADER");
        dlclose(handle);
        return JNI_FALSE;
    }
    loaderHandle = handle;
    turnipSelected = true;
    return JNI_TRUE;
}
