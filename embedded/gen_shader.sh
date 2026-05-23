#!/usr/bin/env bash
set -euo pipefail

OUTPUT="shaders.hpp"
SPVDIR="${1:-.}"

{
    echo "#pragma once"
    echo "#include <array>"
    echo "#include <cstdint>"
    echo ""
    echo "#if defined (__cpp_pp_embed) && __cpp_pp_embed >= 202502L"
    echo ""
} > "$OUTPUT"

for spv in "$SPVDIR"/*.spv; do
    varname="g_$(basename "$spv" .spv | awk -F'.' '{
        result = $1
        for (i=2; i<=NF; i++) {
            word = $i
            result = result toupper(substr(word,1,1)) substr(word,2)
        }
        print result
    }')"
    echo "static constexpr uint8_t ${varname}[] = {" >> "$OUTPUT"
    echo "#embed \"$(basename "$spv")\"" >> "$OUTPUT"
    echo "};" >> "$OUTPUT"
    echo "" >> "$OUTPUT"
done

echo "#else" >> "$OUTPUT"
echo "" >> "$OUTPUT"

for spv in "$SPVDIR"/*.spv; do
    varname="g_$(basename "$spv" .spv | awk -F'.' '{
        result = $1
        for (i=2; i<=NF; i++) {
            word = $i
            result = result toupper(substr(word,1,1)) substr(word,2)
        }
        print result
    }')"
    echo "static constexpr uint8_t ${varname}[] = {" >> "$OUTPUT"
    xxd -i < "$spv" | fold -w 80 >> "$OUTPUT"
    echo "};" >> "$OUTPUT"
    echo "" >> "$OUTPUT"
done

echo "#endif" >> "$OUTPUT"

for spv in "$SPVDIR"/*.spv; do
    varname="g_$(basename "$spv" .spv | awk -F'.' '{
        result = $1
        for (i=2; i<=NF; i++) {
            word = $i
            result = result toupper(substr(word,1,1)) substr(word,2)
        }
        print result
    }')"
    echo "static constexpr uint32_t ${varname}Size{std::size(${varname})};" >> "$OUTPUT"
done

echo "" >> "$OUTPUT"
echo "Generated $OUTPUT"
