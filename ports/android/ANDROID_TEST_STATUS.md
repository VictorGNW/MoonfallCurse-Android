# Moonfall Curse Android test status

Updated November 21, 2026. This record covers the installer, Android renderer,
touch controls, and experimental Adreno compatibility path.

## Forge installation and Xclipse 540

The Samsung SM-A566E report ended in `SIGABRT` on `forge-install` while the
Samsung Vulkan driver was allocating memory. It also recorded exits under
memory pressure. The Forge can create Vulkan pipelines in the installer process
before the game starts, making optional prewarming a plausible contributor.
The report alone does not establish the cause of the mapping failure.

Prewarming now defaults to off and preserves saved choices. Diagnostics record
whether it was requested, the last observed Forge stage, and a native heap
sample. The public alpha was built and audited, including verification of both
the Android signature and the Forge package signature. A clean install and
first frame on Xclipse 540 still need device validation. The mitigation applies
to every GPU.

## Menus, presentation and touch

- Android uses the graphics presentation queue by default, recognizes SDL2
  when selecting it, avoids redundant swapchain rebuilds, and preserves copy
  surfaces and the original renderer error during cleanup. Optional VRS
  defaults to off. Frame pacing at 30 Hz recovers after a missed deadline.
- The native pause pages and host in-game menu fill spare borders with a
  lightly blurred, dimmed, olive-gold copy of the game image. The center keeps
  its presentation bounds. The shader was validated for both Vulkan scanout
  paths. The test APK was installed on the Edge 60 and the installer opened.
  Menu appearance and gameplay still need a device check.
- The Edge 60 log recorded 14 writes of a roughly 190–205 MB Vulkan pipeline
  cache while pipelines were being created in gameplay. Android now waits at
  least 60 seconds between snapshots instead of 20, and logs snapshot and
  write times. This reduces cache activity. The cause of individual first-use
  stalls remains open.
- Touch changes include the revised default layout, automatic Zora art,
  customizable shortcuts, inactivity and controller hiding, and menu-only
  controls while the host menu is open. The viewport-relative layout uses the
  short edge for sizes and remaps saved positions when the view resizes. The
  Android and controls unit suites passed, including a resize regression test.
  That last layout change has yet to be installed on the Edge 60.

## Next priority: first-use stutter and short freezes

Resume this work in a later session. Compare the first encounter with a scene,
effect, or inventory action against repeated attempts in the same session and
after restarting. Use the timings to distinguish shader compilation, Vulkan
pipeline creation, cache persistence, and other frame work before changing
the preparation policy. The goal is to reduce stalls during play without
bringing back the installer's high-memory prewarming by default.

The SPIR-V cache now serializes concurrent cold requests for the same shader
within a cache instance. Once the first request publishes the entry, the next
request can reuse it. Android also lowers the priority of the background
pipeline-cache writer. These changes avoid duplicate work and reduce
contention, and their effect on gameplay still needs a device measurement.

For a focused comparison on a developer build, use the existing
`TriAevum.android.host.json` developer environment to set
`TRIAEVUM_SLOW_STEP_MS=12` and `TRIAEVUM_NRI_PIPELINE_TRACE=1`. Repeat the same
menu action or route on first use, immediately afterward, and after a normal
restart with the cache intact. Compare `TRIAEVUM_SLOW_STEP scope=pica_preparation`
(`shader_program` versus `device_pipeline`), NRI pipeline creation times, and
the cache snapshot and write timings in `native-stderr.log`. The Edge 60 had
no Moonfall Curse installation when this pass began, so there is no fresh
on-device comparison yet.

## Installation and loading follow-up

Forge uses the portable shader pack digest it already verified instead of
reading the entire pack again. On signed APK updates it can reuse the previous
private copy of an unchanged portable corpus. The new signed inventory still
verifies the staged files. An unsupported link operation or a failed integrity
check falls back to copying the corpus from the APK.

Android startup logs now time pipeline-cache reads and identify individual
startup pipeline parts that fail or take a second or more. Use these with
`first_frame_presented` to compare cold and warm starts. Measure whether
reading the driver cache or compiling startup parts accounts for the delay
before changing the number of workers or the startup shader inventory.

The GPU-specific loader and cache namespace experiments remain in their
original branch. This branch uses the system Vulkan path and the existing
driver-identity checks for saved pipelines.
