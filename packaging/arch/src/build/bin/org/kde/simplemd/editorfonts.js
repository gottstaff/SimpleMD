.pragma library

function resolveFamily(preferred, themeFixedFamily) {
    if (preferred && preferred.length > 0 && preferred.toLowerCase() !== "monospace") {
        return preferred
    }
    if (themeFixedFamily && themeFixedFamily.length > 0) {
        return themeFixedFamily
    }
    return "DejaVu Sans Mono"
}

function canvasFontSpec(pixelSize, family, themeFixedFamily) {
    const safeFamily = resolveFamily(family, themeFixedFamily)
    const escaped = safeFamily.replace(/\\/g, "\\\\").replace(/"/g, "\\\"")
    return pixelSize + 'px "' + escaped + '"'
}
