.pragma library

function isImagePath(path) {
    const dot = path.lastIndexOf(".")
    if (dot < 0) {
        return false
    }
    const ext = path.substring(dot + 1).toLowerCase()
    return ext === "png" || ext === "jpg" || ext === "jpeg" || ext === "gif"
        || ext === "svg" || ext === "webp" || ext === "bmp"
}

function isDocumentPath(path) {
    const dot = path.lastIndexOf(".")
    if (dot < 0) {
        return true
    }
    const ext = path.substring(dot + 1).toLowerCase()
    return ext === "md" || ext === "markdown" || ext === "mdown" || ext === "txt"
}

function bracketPairFor(char) {
    switch (char) {
    case "(": return { open: "(", close: ")", forward: true }
    case ")": return { open: "(", close: ")", forward: false }
    case "[": return { open: "[", close: "]", forward: true }
    case "]": return { open: "[", close: "]", forward: false }
    case "{": return { open: "{", close: "}", forward: true }
    case "}": return { open: "{", close: "}", forward: false }
    default: return null
    }
}

function isBracketChar(ch) {
    return ch === "(" || ch === ")" || ch === "[" || ch === "]" || ch === "{" || ch === "}"
}

function findBracketMatch(text, cursorPos) {
    if (!text || text.length === 0) {
        return null
    }

    let pos = -1
    if (cursorPos < text.length && isBracketChar(text[cursorPos])) {
        pos = cursorPos
    } else if (cursorPos > 0 && isBracketChar(text[cursorPos - 1])) {
        pos = cursorPos - 1
    } else {
        return null
    }

    const ch = text[pos]

    const pair = bracketPairFor(ch)
    if (!pair) {
        return null
    }

    const openChar = pair.open
    const closeChar = pair.close
    const fromOpen = pair.forward ? ch === openChar : ch === closeChar

    if (fromOpen && ch === openChar) {
        let depth = 0
        for (let i = pos; i < text.length; ++i) {
            const c = text[i]
            if (c === openChar) {
                depth += 1
            } else if (c === closeChar) {
                depth -= 1
                if (depth === 0) {
                    return { start: pos, end: i }
                }
            }
        }
        return null
    }

    let depth = 0
    for (let i = pos; i >= 0; --i) {
        const c = text[i]
        if (c === closeChar) {
            depth += 1
        } else if (c === openChar) {
            depth -= 1
            if (depth === 0) {
                const start = i
                depth = 0
                for (let j = start; j < text.length; ++j) {
                    const c2 = text[j]
                    if (c2 === openChar) {
                        depth += 1
                    } else if (c2 === closeChar) {
                        depth -= 1
                        if (depth === 0) {
                            return { start: start, end: j }
                        }
                    }
                }
                return null
            }
        }
    }
    return null
}

function backtickRunLength(text, index) {
    let length = 0
    while (index + length < text.length && text[index + length] === "`") {
        length += 1
    }
    return length
}

function findBacktickMatch(text, cursorPos) {
    if (!text || text.length === 0) {
        return null
    }

    let pos = Math.max(0, Math.min(cursorPos, text.length - 1))
    if (text[pos] !== "`" && cursorPos > 0 && text[cursorPos - 1] === "`") {
        pos = cursorPos - 1
    } else if (text[pos] !== "`") {
        return null
    }

    const tickLen = backtickRunLength(text, pos)
    if (tickLen === 0) {
        return null
    }

    let searchFrom = pos + tickLen
    while (searchFrom < text.length) {
        const idx = text.indexOf("`".repeat(tickLen), searchFrom)
        if (idx < 0) {
            return null
        }

        const before = idx > 0 ? text[idx - 1] : ""
        const after = idx + tickLen < text.length ? text[idx + tickLen] : ""
        if (before === "\\") {
            searchFrom = idx + tickLen
            continue
        }
        if (after === "`") {
            searchFrom = idx + tickLen
            continue
        }

        return { start: pos, end: idx + tickLen - 1 }
    }

    return null
}

function findDelimiterMatch(text, cursorPos) {
    const bracket = findBracketMatch(text, cursorPos)
    if (bracket) {
        return bracket
    }
    return findBacktickMatch(text, cursorPos)
}
