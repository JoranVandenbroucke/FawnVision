#!/usr/bin/env bash
set -euo pipefail

OUTPUT="shaders.hpp"
SPVDIR="${1:-.}"
SPVDIR="$(cd "${SPVDIR}" && pwd)"
OUTPUT="${SPVDIR}/${OUTPUT}"

to_var_name() {
    local base
    base="$(basename "$1" .spv)"
    local stem="${base%%.*}"
    local suffix="${base#*.}"
    if [[ "${suffix}" == "${stem}" ]]; then
        printf 'g_%s' "${stem}"
        return
    fi
    local first rest
    first="$(printf '%s' "${suffix}" | cut -c1 | tr '[:lower:]' '[:upper:]')"
    rest="$(printf '%s' "${suffix}" | cut -c2-)"
    printf 'g_%s%s%s' "${stem}" "${first}" "${rest}"
}

format_byte_array() {
    local spv="$1"
    python3 - "$spv" <<'PY'
import pathlib
import sys

data = pathlib.Path(sys.argv[1]).read_bytes()
if not data:
    print("    };")
    raise SystemExit

width = 29
for index in range(0, len(data), width):
    chunk = data[index : index + width]
    line = ", ".join(f"0x{byte:02x}" for byte in chunk)
    print(f"    {line},")
print("};")
PY
}

mapfile -t SPV_FILES < <(find "${SPVDIR}" -maxdepth 1 -type f -name '*.spv' -printf '%f\n' | sort)

{
    echo "#pragma once"
    echo "import std;"
    echo ""
    echo "#if defined(__cpp_pp_embed) && __cpp_pp_embed >= 202502L"
    echo ""
} > "${OUTPUT}"

for spv in "${SPV_FILES[@]}"; do
    varname="$(to_var_name "${spv}")"
    {
        echo "inline constexpr std::uint8_t ${varname}[] = {"
        echo "#    embed \"${spv}\""
        echo "};"
        echo ""
    } >> "${OUTPUT}"
done

{
    echo "#else"
    echo ""
} >> "${OUTPUT}"

for spv in "${SPV_FILES[@]}"; do
    varname="$(to_var_name "${spv}")"
    {
        echo "inline constexpr std::uint8_t ${varname}[]{"
        format_byte_array "${SPVDIR}/${spv}"
        echo ""
    } >> "${OUTPUT}"
done

{
    echo "#endif"
    echo ""
} >> "${OUTPUT}"

for spv in "${SPV_FILES[@]}"; do
    varname="$(to_var_name "${spv}")"
    echo "inline constexpr std::uint32_t ${varname}Size{std::size(${varname})};" >> "${OUTPUT}"
done

echo "" >> "${OUTPUT}"
echo "Generated ${OUTPUT} from ${#SPV_FILES[@]} SPIR-V module(s)" >&2
