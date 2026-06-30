#requires -Version 5.1
param(
    [string]$SpvDir = "."
)

$ErrorActionPreference = "Stop"

$SpvDir = (Resolve-Path -LiteralPath $SpvDir).Path
$OutputPath = Join-Path $SpvDir "shaders.hpp"

function ConvertTo-VarName {
    param([string]$FileName)

    # strip ".spv"
    $base = [System.IO.Path]::GetFileNameWithoutExtension($FileName)

    $dotIndex = $base.IndexOf('.')
    if ($dotIndex -lt 0) {
        return "g_$base"
    }

    $stem   = $base.Substring(0, $dotIndex)
    $suffix = $base.Substring($dotIndex + 1)

    $first = $suffix.Substring(0, 1).ToUpperInvariant()
    $rest  = $suffix.Substring(1)

    return "g_$stem$first$rest"
}

function Format-ByteArray {
    param([string]$SpvPath)

    $bytes = [System.IO.File]::ReadAllBytes($SpvPath)
    $width = 29

    if ($bytes.Length -eq 0) {
        return @("    };")
    }

    $lines = New-Object System.Collections.Generic.List[string]
    for ($i = 0; $i -lt $bytes.Length; $i += $width) {
        $end   = [Math]::Min($i + $width, $bytes.Length) - 1
        $chunk = $bytes[$i..$end]
        $hex   = ($chunk | ForEach-Object { "0x{0:x2}" -f $_ }) -join ", "
        $lines.Add("    $hex,")
    }
    $lines.Add("};")
    return $lines
}

$spvFiles = Get-ChildItem -LiteralPath $SpvDir -Filter "*.spv" -File |
    Sort-Object Name |
    Select-Object -ExpandProperty Name

$content = New-Object System.Collections.Generic.List[string]
$content.Add("#pragma once")
$content.Add("import std;")
$content.Add("")
$content.Add("#if defined(__cpp_pp_embed) && __cpp_pp_embed >= 202502L")
$content.Add("")

foreach ($spv in $spvFiles) {
    $varname = ConvertTo-VarName $spv
    $content.Add("inline constexpr std::uint8_t $varname[] = {")
    $content.Add("#    embed `"$spv`"")
    $content.Add("};")
    $content.Add("")
}

$content.Add("#else")
$content.Add("")

foreach ($spv in $spvFiles) {
    $varname = ConvertTo-VarName $spv
    $content.Add("inline constexpr std::uint8_t $varname[]{")
    foreach ($line in (Format-ByteArray (Join-Path $SpvDir $spv))) {
        $content.Add($line)
    }
    $content.Add("")
}

$content.Add("#endif")
$content.Add("")

foreach ($spv in $spvFiles) {
    $varname = ConvertTo-VarName $spv
    $content.Add("inline constexpr std::uint32_t ${varname}Size{static_cast<std::uint32_t>(std::size($varname))};")
}
$content.Add("")

Set-Content -LiteralPath $OutputPath -Value $content -Encoding utf8

Write-Host "Generated $OutputPath from $($spvFiles.Count) SPIR-V module(s)"