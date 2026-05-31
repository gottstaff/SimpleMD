.pragma library

function stripHeadingTitle(raw) {
    let title = raw.trim().replace(/\s+#+\s*$/, "")
    title = title.replace(/\[([^\]]+)\]\([^)]*\)/g, "$1")
    title = title.replace(/\*\*([^*]+)\*\*/g, "$1")
    title = title.replace(/\*([^*]+)\*/g, "$1")
    title = title.replace(/`([^`]+)`/g, "$1")
    return title.trim()
}

function parseOutline(markdown) {
    const items = []
    const lines = (markdown || "").split("\n")
    let inFence = false

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i]
        const trimmed = line.trim()
        if (/^(```|~~~)/.test(trimmed)) {
            inFence = !inFence
            continue
        }
        if (inFence) {
            continue
        }

        const match = line.match(/^(#{1,6})(?:\s+(.+))?\s*$/)
        if (!match) {
            continue
        }

        const level = match[1].length
        const title = stripHeadingTitle(match[2] || "")
        items.push({
            level: level,
            title: title.length > 0 ? title : "",
            line: i + 1
        })
    }

    return items
}

function activeIndexForLine(items, line) {
    if (!items || items.length === 0) {
        return -1
    }
    let active = -1
    for (let i = 0; i < items.length; i++) {
        if (items[i].line <= line) {
            active = i
        } else {
            break
        }
    }
    return active
}
