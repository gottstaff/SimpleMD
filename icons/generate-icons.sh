#!/usr/bin/env bash
# Regenerate app icons from the source SVG.
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
src="${root}/kde-minimal-markdown-editor-icon-v2.svg"
out="${root}/icons"
processed="${out}/sc-apps-io.github.gottstaff.SimpleMD.svg"

if [[ ! -f "$src" ]]; then
    echo "Missing source icon: $src" >&2
    exit 1
fi

mkdir -p "$out"

# Single palette for predictable launcher / PNG rendering (light scheme).
cat > "$processed" <<'EOF'
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 256 256" role="img" aria-label="SimpleMD">
  <style>
    .back { fill: #24c7e5; }
    .tile { fill: #12324a; }
    .edge { stroke: #0d273a; }
    .mark { stroke: #eef6fb; }
    .accent { stroke: #35d3f2; }
    .muted { stroke: #7ea3b8; }
    .round { stroke-linecap: round; stroke-linejoin: round; }
  </style>

  <g transform="translate(128 128) scale(1.263) translate(-126 -128)">
  <rect class="back" x="36" y="33" width="180" height="190" rx="34"/>
  <rect class="tile" x="44" y="33" width="176" height="190" rx="31"/>
  <path class="edge" d="M74 223h115c17.1 0 31-13.9 31-31V65" fill="none" stroke-width="2" opacity=".35"/>

  <g class="round mark" fill="none" stroke-width="12">
    <path d="M98 80 L84 148"/>
    <path d="M132 80 L118 148"/>
    <path d="M76 106 H144"/>
    <path d="M70 132 H138"/>
  </g>

  <path class="round accent" d="M168 80 V148" fill="none" stroke-width="6"/>
  <path class="round accent" d="M80 171 L88 179 L80 187" fill="none" stroke-width="6"/>
  <path class="round accent" d="M101 179 H181" fill="none" stroke-width="6"/>
  <path class="round muted" d="M101 193 H149" fill="none" stroke-width="6" opacity=".9"/>
  </g>
</svg>
EOF

for size in 16 22 24 32 48 64 128 256; do
    rsvg-convert -w "$size" -h "$size" "$processed" \
        -o "${out}/${size}-apps-io.github.gottstaff.SimpleMD.png"
done

echo "Icons written to ${out}/"
