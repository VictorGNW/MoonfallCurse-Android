#!/usr/bin/env python3
"""Stage and strip publisher-built Android libraries, without rebuilding or copying game data."""
from __future__ import annotations

import argparse
import hashlib
from io import BytesIO
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
from urllib.request import urlopen
from zipfile import ZipFile

# `python -I` (the release recipe) leaves the script's folder off sys.path.
sys.path.insert(0, str(Path(__file__).resolve().parent))
from android_elf_checks import BUNDLED_TURNIP_LIBRARY, check_stage  # noqa: E402


TURNIP_URL = ("https://github.com/nihui/mesa-turnip-android-driver/releases/download/26.3.0/"
              "mesa-turnip-android-26.3.0.zip")
TURNIP_ZIP_SHA256 = "6a9bef1c83aa6280e2389d33a992f2d80199d1d13be629c7ff12d5f9f17b5803"
TURNIP_ZIP_SIZE = 3519567


def bundled_turnip_bytes(archive: Path | None = None) -> bytes:
    """Verify the exact pinned Android build before extracting the only payload."""
    if archive is None:
        with urlopen(TURNIP_URL, timeout=45) as response:
            data = response.read(TURNIP_ZIP_SIZE + 1)
    else:
        with archive.open("rb") as source:
            data = source.read(TURNIP_ZIP_SIZE + 1)
    if len(data) != TURNIP_ZIP_SIZE or hashlib.sha256(data).hexdigest() != TURNIP_ZIP_SHA256:
        raise ValueError("Turnip 26.3.0 package does not match the pinned SHA-256")
    with ZipFile(BytesIO(data)) as package:
        return package.read("libvulkan_freedreno.so")


def host_llvm(ndk: Path) -> Path:
    """The NDK's prebuilt LLVM directory of the build host (Linux, Windows or macOS)."""
    prebuilt = ndk / "toolchains/llvm/prebuilt"
    hosts = sorted(path for path in prebuilt.iterdir() if path.is_dir()) if prebuilt.is_dir() else []
    if len(hosts) != 1:
        raise FileNotFoundError(f"Expected one NDK host toolchain under {prebuilt}, found {len(hosts)}")
    return hosts[0]


def stage(runtime: Path, title: Path, forge: Path, ndk: Path, output: Path) -> None:
    llvm = host_llvm(ndk)
    exe = ".exe" if (llvm / "bin/llvm-readelf.exe").is_file() else ""
    readelf, strip = llvm / f"bin/llvm-readelf{exe}", llvm / f"bin/llvm-strip{exe}"
    source = {
        "libSDL2.so": runtime / "_deps/sdl2-build/libSDL2.so",
        "libTriAevum.so": runtime / "product/libTriAevum.so",
        "libtriaevum_title_bootstrap.so": runtime / "product/libtriaevum_title_bootstrap.so",
        "libtriaevum_title_aot.so": title,
        "libtriaevum_forge.so": forge,
        "libc++_shared.so": llvm / "sysroot/usr/lib/aarch64-linux-android/libc++_shared.so",
    }
    def dependencies(path: Path) -> set[str]:
        dynamic = subprocess.check_output([readelf, "-d", path], text=True)
        return set(re.findall(r"\(NEEDED\).*\[(.*?)\]", dynamic))

    turnip = ("libtriaevum_vulkan.so", "libmain_hook.so", "libhook_impl.so")
    runtime_library = source["libTriAevum.so"]
    if runtime_library.is_file() and turnip[0] in dependencies(runtime_library):
        turnip_dir = runtime / "turnip-libs"
        source.update({name: turnip_dir / name for name in turnip})
    for name, path in source.items():
        if not path.is_file():
            raise FileNotFoundError(f"{name}: {path}")
        header = subprocess.check_output([readelf, "-h", path], text=True)
        if "AArch64" not in header:
            raise ValueError(f"Not an ARM64 library: {path}")
    output = output.resolve()
    if any(output == path.resolve().parent or path.resolve().is_relative_to(output) for path in source.values()):
        raise ValueError("Stage outside the input library directories")
    destination = output / "arm64-v8a"
    destination.mkdir(parents=True, exist_ok=True)
    for name in (*turnip, BUNDLED_TURNIP_LIBRARY):
        if name not in source:
            (destination / name).unlink(missing_ok=True)
    allowed = set(source) | {"libc.so", "libm.so", "libdl.so", "liblog.so", "libandroid.so",
                             "libEGL.so", "libGLESv1_CM.so", "libGLESv2.so", "libGLESv3.so", "libvulkan.so",
                             "libOpenSLES.so", "libaaudio.so", "libz.so"}
    for name, path in source.items():
        needed = dependencies(path)
        if needed - allowed:
            raise ValueError(f"Unpackaged dependency for {name}: {needed - allowed}")
        target = destination / name
        shutil.copy2(path, target)
        # --strip-all: .symtab is not needed at run time and the APK audit refuses it.
        subprocess.run([strip, "--strip-all", target], check=True)
        print(f"{name}: {target.stat().st_size} bytes; dependencies {sorted(needed)}")
    if turnip[0] in source:
        archive = os.environ.get("TRIAEVUM_ANDROID_TURNIP_ARCHIVE")
        target = destination / BUNDLED_TURNIP_LIBRARY
        target.write_bytes(bundled_turnip_bytes(Path(archive) if archive else None))
        needed = dependencies(target)
        if needed - {"libm.so", "libz.so", "libdl.so", "libc.so"}:
            raise ValueError(f"Unexpected dependencies for bundled Turnip: {needed}")
        subprocess.run([strip, "--strip-all", target], check=True)
        print(f"{BUNDLED_TURNIP_LIBRARY}: {target.stat().st_size} bytes; dependencies {sorted(needed)}")
    # 16 KiB LOAD alignment, one NDK for the project, and libc++ symbol closure.
    problems = check_stage(destination)
    if problems:
        raise ValueError("Staged libraries fail the release checks: " + "; ".join(problems))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runtime-build", type=Path, required=True)
    parser.add_argument("--title-module", type=Path, required=True)
    parser.add_argument("--forge-library", type=Path, required=True,
                        help="libtriaevum_forge.so from the Rust Forge (crates/forge-android)")
    parser.add_argument("--ndk", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    stage(args.runtime_build, args.title_module, args.forge_library, args.ndk, args.output)
