# Moonfall Curse Android changelog

## Unreleased

Changes since [0.5.0-alpha.2](https://github.com/VictorGNW/MoonfallCurse-Android/releases/tag/v0.5.0-alpha.2), consolidated in [PR #10](https://github.com/VictorGNW/MoonfallCurse-Android/pull/10).

### Rendering and graphics

- Added SGSR Edge Direction and updated the Enhanced for Mobile preset.
- Simplified the Android graphics settings in the in-game menu.
- Added optional BCn-to-ASTC transcoding for compatible Vulkan devices and exposed the setting in Forge.
- Added imported Turnip support for compatible Adreno devices and limited automatic selection of bundled Turnip to Adreno 600-series devices.
- Improved Vulkan surface recovery, shader diagnostics, and management of large pipeline-cache snapshots.

### Forge and controls

- Retained the adaptive touch layout, installer improvements, and opt-in shader prewarming from the 0.5.0-alpha.2 baseline.
- Added a build check that pins the Android signing certificate to the certificate used by the public APK.

### Validation

- Built the ARM64 candidate from commit [`7cf2bd24`](https://github.com/VictorGNW/MoonfallCurse-Android/commit/7cf2bd244dd221dbbfcb1a2aa95bfe8c87969941). The local variant used Mesa Turnip 26.3.0 R5. The Forge package was signed, and the APK passed the Android release audit with the published certificate.
- On-device validation of this candidate and its bundled driver is pending. USA v1.1 support is tracked in [PR #2](https://github.com/VictorGNW/MoonfallCurse-Android/pull/2).

## 0.5.0-alpha.2

See the [published release notes](https://github.com/VictorGNW/MoonfallCurse-Android/releases/tag/v0.5.0-alpha.2) for changes and downloads.
