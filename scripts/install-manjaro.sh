#!/usr/bin/env bash
# Build and install SimpleMD on Manjaro / Arch from a local checkout.
#
# Usage (from anywhere in the repo):
#   ./scripts/install-manjaro.sh
#
# Note: "pamac build" only works for packages published on AUR. For a local
# PKGBUILD, use this script or: cd packaging/arch && makepkg -si
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PKGDIR="${ROOT}/packaging/arch"

deps=(
  base-devel
  cmake
  extra-cmake-modules
  qt6-base
  qt6-declarative
  qt6-webengine
  qt6-tools
  kirigami
  ki18n
  kcoreaddons
  qqc2-desktop-style
  kiconthemes
  qtkeychain-qt6
)

echo "==> SimpleMD — Manjaro / Arch install"
echo "    Project: ${ROOT}"
echo "    PKGBUILD: ${PKGDIR}"

if [[ "${EUID}" -eq 0 ]]; then
  echo "Error: do not run this script as root (makepkg refuses root)." >&2
  exit 1
fi

if ! command -v pacman >/dev/null; then
  echo "Error: pacman not found (this script is for Arch / Manjaro)." >&2
  exit 1
fi

if ! command -v makepkg >/dev/null; then
  echo "Error: makepkg not found. Install base-devel:" >&2
  echo "  sudo pacman -S base-devel" >&2
  exit 1
fi

echo "==> Ensuring build dependencies…"
sudo pacman -S --needed --noconfirm "${deps[@]}"

cd "${PKGDIR}"

echo "==> Cleaning stale package build tree…"
rm -rf "${PKGDIR}/src" "${PKGDIR}/pkg"

echo "==> Building package with makepkg…"
makepkg -f --noconfirm --nocheck

# Remove older package files so pacman -U never sees duplicate targets.
mapfile -t old_pkgs < <(ls -1 simplemd-*.pkg.tar.zst 2>/dev/null | sort -V)
if ((${#old_pkgs[@]} > 1)); then
  for ((i = 0; i < ${#old_pkgs[@]} - 1; i++)); do
    rm -f "${old_pkgs[i]}"
  done
fi

mapfile -t built_pkgs < <(ls -1 simplemd-*.pkg.tar.zst 2>/dev/null | sort -V)
pkg="${built_pkgs[${#built_pkgs[@]} - 1]:-}"
if [[ -z "${pkg}" || ! -f "${pkg}" ]]; then
  echo "Error: package file not found after build." >&2
  exit 1
fi

echo "==> Installing ${pkg}…"
# Icons may exist from a prior cmake --install without pacman ownership.
sudo pacman -U --noconfirm \
  --overwrite 'usr/share/icons/hicolor/*/apps/io.github.gottstaff.SimpleMD.*' \
  "${pkg}"

echo
echo "Done. Launch with: simplemd"
echo "Or find “SimpleMD” in your application menu."
