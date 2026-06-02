<p align="center">
  <img src="icons/sc-apps-io.github.gottstaff.SimpleMD.svg" alt="SimpleMD icon" width="128" height="128">
</p>

<h1 align="center">SimpleMD</h1>

<p align="center">A minimal KDE markdown editor with live preview.</p>

<p align="center">Work in progress. This is a personal tool designed for a low-distraction, neurodivergent workflow that encourages hyperfocus. <strong>SimpleMD is not designed with broad accessibility in mind.</strong></p>

## About

SimpleMD is spartan: a split editor, a live preview, and little else. No plugin system, no cloud sync, no bloated toolbar. Just markdown rendered well in a focused full-screen window and a minimal AI assistant because I'm fed up with writing formulae in LaTeX.

There are **no published packages** on Flathub or in distro repos yet. On Manjaro / Arch, use the install script below. On other Linux systems with Flatpak, see the Flatpak section.

![SimpleMD split editor with live preview](docs/screenshot.png)

## Features

- **Split-pane editing.** Markdown on the left, live preview on the right.
- **Single pane mode.** Markdown comfortably centered when expression matters.
- **Rich preview.** Math (KaTeX), chemistry (mhchem), Mermaid diagrams, …
- **Document outline.** Jump between headings in long documents.
- **Insert menus.** Snippets for common markdown, math, basic chemistry, citations, …
- **Images.** Insert or paste; on save, optionally copy into an `images/` folder beside the document.
- **PDF export.** Print or save from the preview with basic formatting options.
- **Light AI editing.** One optional command for quick rewrites (see below).
- **KDE-native.** Kirigami UI, desktop theming, full-screen focus mode.

## AI-assisted light editing

One dialog, direct apply. No chat panel, presets, streaming, or diff review.

![AI edit demo](docs/ai-demo.gif)

Any **OpenAI-compatible** `/chat/completions` endpoint works:

| Setup | API base URL | API key |
| --- | --- | --- |
| Cloud (OpenAI, DeepSeek, …) | Provider URL | Your API key |
| Local (Ollama, llama.cpp, LM Studio, …) | e.g. `http://127.0.0.1:11434/v1` | Whatever the server expects (often any placeholder) |

Configure **API base URL**, **Model**, and related options in **Settings → Preferences…** (AI section).

**Privacy:** each request sends your instruction plus either the current selection or ~1,200 characters of context around the cursor or the selected text to the configured server. **With an external provider, that content leaves your machine**.

**Security:** the API key is stored in Qt settings on disk **in plain text** (not encrypted yet).

AI is optional. Without it, the editor runs normally (offline aside from WebEngine’s usual needs).

## Install

### Manjaro / Arch

From the repository root:

```bash
./scripts/install-manjaro.sh
```

The script installs dependencies, builds a local package, and installs it with `pacman`. Do not run it as root. Launch with `simplemd` or from the application menu (Utility / Text Editor).

### Flatpak

Flatpak builds are supported; Flathub submission is not planned yet. The app is distributed from a self-hosted repository on GitHub Pages. KDE/Qt runtimes are pulled from Flathub automatically — that is a dependency, not an endorsement.

**Prerequisites**

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
```

**Add the SimpleMD repository and install**

```bash
flatpak remote-add --user --if-not-exists --no-gpg-verify simplemd \
  https://gottstaff.github.io/SimpleMD/simplemd.flatpakrepo

flatpak install --user simplemd io.github.gottstaff.SimpleMD
```

On first install, Flatpak also installs `org.kde.Platform//6.10` and `io.qt.qtwebengine.BaseApp//6.10` from Flathub.

**Run**

Launch **SimpleMD** from the application menu (Utility / Text Editor), or:

```bash
flatpak run io.github.gottstaff.SimpleMD
```

The menu entry is created automatically when the Flatpak is installed — no separate “add to menu” step.

**Update**

```bash
flatpak update --user io.github.gottstaff.SimpleMD
```

**Build from source (developers)**

```bash
./scripts/build-flatpak.sh --install
```

That builds the OSTree repo under `packaging/flatpak/repo/` and installs it for local testing. Building alone does **not** install the app — you must install from a remote (GitHub Pages or the local repo) before `flatpak run` works.

**Console noise when launching.** Messages such as `Session management error`, `Failed to connect to … system_bus_socket`, or `GBM is not supported … Fallback to Vulkan` are common for Qt WebEngine inside Flatpak and are usually harmless if the window and live preview work. The shortcut warnings come from KDE’s desktop style, not SimpleMD.

## Uninstall

### Manjaro / Arch

Remove SimpleMD and dependencies that no other installed package needs:

```bash
sudo pacman -Rs simplemd
```

Pacman keeps removed config files as `.pacsave` entries. Add `-n` (`-Rns`) if you also want those config files deleted.

To remove leftover orphan packages, list them first and confirm the output looks right:

```bash
pacman -Qtdq
sudo pacman -Rns $(pacman -Qtdq)
```

Skip the second command if the list includes packages you still want, or if `pacman -Qtdq` prints nothing.

### Flatpak

Remove the app:

```bash
flatpak uninstall --user io.github.gottstaff.SimpleMD
```

To remove **everything** SimpleMD-related (app, custom repository, unused runtimes, and saved settings):

```bash
flatpak uninstall --user io.github.gottstaff.SimpleMD
flatpak remote-delete --user simplemd
flatpak uninstall --user --unused
rm -rf ~/.var/app/io.github.gottstaff.SimpleMD
```

The last command deletes preferences, recent files, and cache stored inside the Flatpak sandbox. Your markdown documents are **not** removed (they live in your home directory, not in that folder).

Review `flatpak uninstall --user --unused` before confirming — it may remove KDE/Qt runtimes that other Flatpak apps still need.

## Third-party licenses

See `THIRD_PARTY_LICENSES.md` for bundled vendor assets and Mermaid-related third-party licensing details.

## License

SimpleMD is licensed under **GPL-3.0-or-later**. See `LICENSE`.
