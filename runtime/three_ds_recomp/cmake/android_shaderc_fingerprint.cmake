# shaderc_combined contains the compiler and its transitive static libraries.
file(SHA256 "${archive}" fingerprint)
set(header "#pragma once\n#define TRIAEVUM_ANDROID_SHADERC_FINGERPRINT \"${fingerprint}\"\n")
file(CONFIGURE OUTPUT "${output}" CONTENT "${header}" @ONLY)
