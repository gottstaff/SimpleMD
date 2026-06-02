#!/usr/bin/env bash
# Build SimpleMD as a Flatpak and export a local OSTree repository.
#
# Usage (from anywhere in the repo):
#   ./scripts/build-flatpak.sh          # build only (updates packaging/flatpak/repo/)
#   ./scripts/build-flatpak.sh --install # build, then install into --user for local testing
#
# Requires: flatpak, flatpak-builder, and Flathub runtimes (installed below).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
FLATPAK_DIR="${ROOT}/packaging/flatpak"
MANIFEST="${FLATPAK_DIR}/io.github.gottstaff.SimpleMD.yml"
BUILDDIR="${FLATPAK_DIR}/builddir"
REPO="${FLATPAK_DIR}/repo"
RUNTIME_VERSION="6.10"
APP_ID="io.github.gottstaff.SimpleMD"
LOCAL_REMOTE="simplemd-local"
INSTALL_AFTER_BUILD=0

# GitHub Actions and some CI images disallow system-wide Flatpak; use --user there.
FPK=()
if [[ -n "${FLATPAK_USER:-}" ]]; then
  FPK=(--user)
fi

for arg in "$@"; do
  case "${arg}" in
    --install) INSTALL_AFTER_BUILD=1 ;;
    -h|--help)
      echo "Usage: $0 [--install]"
      echo "  --install  register local repo and install ${APP_ID} for --user"
      exit 0
      ;;
    *)
      echo "Unknown option: ${arg}" >&2
      exit 1
      ;;
  esac
done

echo "==> SimpleMD — Flatpak build"
echo "    Project: ${ROOT}"
echo "    Manifest: ${MANIFEST}"

if [[ "${EUID}" -eq 0 ]]; then
  echo "Error: do not run this script as root." >&2
  exit 1
fi

for cmd in flatpak flatpak-builder; do
  if ! command -v "${cmd}" >/dev/null; then
    echo "Error: ${cmd} not found. Install flatpak and flatpak-builder." >&2
    exit 1
  fi
done

if ! flatpak "${FPK[@]}" remote-list | grep -q '^flathub'; then
  echo "==> Adding Flathub remote…"
  flatpak "${FPK[@]}" remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
fi

echo "==> Ensuring Flatpak build runtimes (from Flathub)…"
flatpak "${FPK[@]}" install -y --noninteractive flathub \
  "org.kde.Platform//${RUNTIME_VERSION}" \
  "org.kde.Sdk//${RUNTIME_VERSION}" \
  "io.qt.qtwebengine.BaseApp//${RUNTIME_VERSION}"

mkdir -p "${REPO}"

echo "==> Building Flatpak…"
FB_ARGS=(--force-clean --install-deps-from=flathub --repo="${REPO}" "${BUILDDIR}" "${MANIFEST}")
if [[ ${#FPK[@]} -gt 0 ]]; then
  FB_ARGS=(--user "${FB_ARGS[@]}")
fi
flatpak-builder "${FB_ARGS[@]}"

echo "==> Updating repository metadata…"
flatpak build-update-repo "${REPO}"

echo
echo "Done. Local repo: ${REPO}"
echo
echo "Building does not install the app. To run locally, either:"
echo "  ./scripts/build-flatpak.sh --install"
echo "or:"
echo "  flatpak --user remote-add --if-not-exists --no-gpg-verify ${LOCAL_REMOTE} file://${REPO}"
echo "  flatpak install --user --noninteractive ${LOCAL_REMOTE} ${APP_ID}"
echo "  flatpak run ${APP_ID}"

if [[ "${INSTALL_AFTER_BUILD}" -eq 1 ]]; then
  echo
  echo "==> Installing ${APP_ID} from local repo…"
  flatpak --user remote-add --if-not-exists --no-gpg-verify "${LOCAL_REMOTE}" "file://${REPO}"
  flatpak install --user --noninteractive "${LOCAL_REMOTE}" "${APP_ID}"
  echo
  echo "Installed. Launch with: flatpak run ${APP_ID}"
fi
