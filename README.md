# Moonfall Curse Android

Moonfall Curse Android is an experimental native Android recompilation of **The Legend of Zelda: Majora’s Mask 3D**. It uses the [TriAevum runtime](https://github.com/coccofresco/TriAevum) and ahead-of-time compilation for ARM64. SDL2 handles the window and input, and NRI renders through Vulkan.

## Get the Android test build

[**Download the ARM64 APK**](https://github.com/VictorGNW/MoonfallCurse-Android/releases/download/v0.5.0-alpha.1-android-test/Moonfall-Curse-Android-arm64-local-test.apk) · [Pre-release notes and SHA-256](https://github.com/VictorGNW/MoonfallCurse-Android/releases/tag/v0.5.0-alpha.1-android-test)

- **Version:** 0.5.0-alpha.1 test build for ARM64 devices running Android 10 or later with a compatible Vulkan driver.
- **Installation:** The APK uses a test signature and a separate app ID (`org.triaevum.moonfallcurse.localtest`), so it can be installed alongside the normal build. Open **Moonfall Curse**, choose **Select ROM**, and let Forge prepare the game from your own compatible decrypted cartridge dump. The APK does not include the ROM.
- **Installer:** The centered **Language** control follows the device language by default and offers a manual override. Vulkan pipeline precompilation is optional.

## Validation status

On a Motorola Edge 60 (Mali-G615 MC2), the test build installed, opened the installer in Brazilian Portuguese automatically, imported **USA v1.1**, prepared **3,015 Vulkan pipelines**, and reached the **Ver. 1.1** title screen. Starting a playable save, save and reload behavior, and compatibility across devices still need validation. See the [device test report](https://github.com/VictorGNW/MoonfallCurse-Android/pull/2#issuecomment-7060557986).

The Android test package accepts the documented [EUR revision](titles/mm3d/forge/supported_revisions.json) and [USA v1.1](https://github.com/VictorGNW/MoonfallCurse-Android/blob/v0.5.0-alpha.1-android-test/titles/mm3d/forge/android_usa_v11.json). The on-device result above covers USA v1.1.

## Build and report issues

See the [Android build guide](ports/android/MM3D.md) for publisher build inputs and device diagnostics, and the [Android release guide](docs/TRIAEVUM_ANDROID_RELEASE.md) for signing and distribution. The build guides describe the base publisher workflow. The [source snapshot for the test APK](https://github.com/VictorGNW/MoonfallCurse-Android/tree/v0.5.0-alpha.1-android-test) includes the USA v1.1 support and installer changes.

When reporting an issue, include the APK or commit version, phone model, Android version, GPU, steps to reproduce, and relevant diagnostics.

## Credits and licensing

Thanks to the [TriAevum](https://github.com/coccofresco/TriAevum) contributors, the [MM3D-PC](https://github.com/Volpe34/MM3D-PC) project that provided the basis for the earlier MM3D-Mobile prototype, and the [Azahar](https://github.com/azahar-emu/azahar) contributors whose touch overlay is used by the Android app. See the [third-party notices](THIRD_PARTY_NOTICES.md) and [license scope](LICENSE_SCOPE.md) for component credits and terms.

Majora’s Mask 3D and its game assets belong to their respective rights holders. This is an unofficial fan project. Use your own dump and keep ROM images, extracted game data, keys, and locally generated title files private.

## Technical documentation

See [docs/INDEX.md](docs/INDEX.md) for Android/Vulkan architecture, GPU compatibility, SGSR EdgeDir planning, performance investigations, and upstream tracking. The [AGENTS.md](AGENTS.md) file contains development rules for AI agents.
