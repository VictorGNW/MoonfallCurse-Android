# MM3D-Android

MM3D-Android is an experimental native Android recompilation of **The Legend of Zelda: Majora's Mask 3D**. The port uses the [TriAevum runtime](https://github.com/coccofresco/TriAevum), with the translated Nintendo 3DS code compiled ahead of time for ARM64. On Android, SDL2 handles the window and input, and NRI runs the renderer on Vulkan.

## Current status

- **MM3D-Android** uses the Moonfall Curse build profile (`mc`, `org.triaevum.moonfallcurse`). It targets ARM64 phones running Android 10 (API 29) or later with a compatible Vulkan driver. The app contains the Forge installer and an editable touch overlay derived from Azahar.
- The latest public APK is [0.5.0-alpha.2](https://github.com/VictorGNW/MoonfallCurse-Android/releases/tag/v0.5.0-alpha.2). The [consolidated Android candidate](https://github.com/VictorGNW/MoonfallCurse-Android/pull/10) adds shader-cache and Vulkan recovery changes, the revised mobile graphics preset, SGSR Edge Direction, BCn-to-ASTC texture transcoding, and Turnip support for compatible Adreno devices. See the [changelog](CHANGELOG.md).
- The consolidated candidate passed the Android APK audit with the published signing certificate. On-device validation of that build is pending, including Forge installation, first frame, app resume, and the Turnip test variant.
- A brief gameplay test reached in-game play on a Motorola Edge 60 with a Mali-G615 MC2 using a decrypted USA v1.1 dump on the [earlier MM3D-Mobile prototype](https://github.com/VictorGNW/MM3D-Mobile/tree/46caf82a3014661b9407aa38aa4e57cb2bcda71e). USA v1.1 support for the current Forge package is tracked separately in [PR #2](https://github.com/VictorGNW/MoonfallCurse-Android/pull/2).

## Build and test

Follow the [MM3D Android build guide](ports/android/MM3D.md) for the Moonfall Curse inputs, build command and device checks. Forge's current MM3D recipe expects a matching decrypted **EUR** cartridge dump from your own copy. See the [supported revision catalog](titles/mm3d/forge/supported_revisions.json) and the [Android release guide](docs/TRIAEVUM_ANDROID_RELEASE.md) for package and signing details.

When reporting an issue, include the APK or commit version, phone model, Android version, GPU, reproduction steps and the relevant diagnostics. The build guide shows how to collect logs with ADB.

## Samsung Xclipse 540 installer test

Forge leaves **Prepare known shaders before playing** unchecked by default on new
installs. This optional step creates Vulkan pipelines in the installer process
and can increase memory pressure. Existing preferences are kept, so uncheck it
manually if you previously enabled it. Skipping this step can increase stutter
during the first play session.

This mitigation is awaiting device validation. On a Samsung SM-A566E with
Xclipse 540, install from your own decrypted dump with the option unchecked,
then launch the game and check whether it reaches the first rendered frame.
If installation or launch fails, reopen the app and export Diagnostics with the
Forge log. Include the APK version, whether shader preparation was enabled, and
the last observed Forge stage. If ADB is available, collect the native crash
message with `adb logcat -b crash -d`.

## Credits and licensing

Thanks to the [TriAevum](https://github.com/coccofresco/TriAevum) contributors, the [MM3D-PC](https://github.com/Volpe34/MM3D-PC) project that provided the basis for MM3D-Mobile, and the [Azahar](https://github.com/azahar-emu/azahar) contributors whose overlay is used by the Android app. See the [third-party notices](THIRD_PARTY_NOTICES.md) and [license scope](LICENSE_SCOPE.md) for component credits and terms.

Majora's Mask 3D and its game assets belong to their respective rights holders. This is an unofficial fan project. Use your own dump and keep ROM images, extracted game data, keys and locally generated title files private.

## Android documentation and development

See the [documentation index](docs/INDEX.md) for Android builds, Vulkan
compatibility, performance investigations and upstream tracking. Follow the
[development instructions](AGENTS.md) when working on this repository.

Consult the [device test status](ports/android/ANDROID_TEST_STATUS.md) for
feature validation and compatibility reports.
