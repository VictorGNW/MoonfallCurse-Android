# MM3D-Android

MM3D-Android is an experimental native Android recompilation of **The Legend of Zelda: Majora's Mask 3D**. The port uses the [TriAevum runtime](https://github.com/coccofresco/TriAevum), with the translated Nintendo 3DS code compiled ahead of time for ARM64. On Android, SDL2 handles the window and input, and NRI runs the renderer on Vulkan.

## Current status

- **MM3D-Android** uses the Moonfall Curse build profile (`mc`, `org.triaevum.moonfallcurse`). It targets ARM64 phones running Android 10 (API 29) or later with a compatible Vulkan driver. The app contains the Forge installer and an editable touch overlay derived from Azahar.
- **Hardware validation is pending for this build.** ROM import through Forge, gameplay, saving, touch and controller input, and app resume still need end-to-end validation for Moonfall Curse on Android.
- **Previous MM3D-Mobile prototype.** A brief gameplay test reached in-game play on a Motorola Edge 60 with a Mali-G615 MC2 using a decrypted USA v1.1 dump. The prototype also compiled the title module in Termux from C++ generated on a PC. Those results apply to the [MM3D-Mobile prototype](https://github.com/VictorGNW/MM3D-Mobile/tree/46caf82a3014661b9407aa38aa4e57cb2bcda71e).

## Build and test

Follow the [MM3D Android build guide](ports/android/MM3D.md) for the Moonfall Curse inputs, build command and device checks. Forge's current MM3D recipe expects a matching decrypted **EUR** cartridge dump from your own copy. See the [supported revision catalog](titles/mm3d/forge/supported_revisions.json) and the [Android release guide](docs/TRIAEVUM_ANDROID_RELEASE.md) for package and signing details.

When reporting an issue, include the APK or commit version, phone model, Android version, GPU, reproduction steps and the relevant diagnostics. The build guide shows how to collect logs with ADB.

## Credits and licensing

Thanks to the [TriAevum](https://github.com/coccofresco/TriAevum) contributors, the [MM3D-PC](https://github.com/Volpe34/MM3D-PC) project that provided the basis for MM3D-Mobile, and the [Azahar](https://github.com/azahar-emu/azahar) contributors whose overlay is used by the Android app. See the [third-party notices](THIRD_PARTY_NOTICES.md) and [license scope](LICENSE_SCOPE.md) for component credits and terms.

Majora's Mask 3D and its game assets belong to their respective rights holders. This is an unofficial fan project. Use your own dump and keep ROM images, extracted game data, keys and locally generated title files private.
