<p align="center">
  <img src="icons/sc-apps-org.kde.simplemd.svg" alt="SimpleMD icon" width="128" height="128">
</p>

<h1 align="center">SimpleMD</h1>

<p align="center">A minimal KDE markdown editor with live preview.</p>

<p align="center"><strong>Work in progress.</strong> A personal project built for the author’s own writing workflow.</p>

## About

SimpleMD is intentionally spartan: a split editor, a live preview, and little else. No plugin system, no cloud sync, no bloated toolbar. Just markdown, rendered well, in a focused full-screen window. It is shaped by one person’s needs rather than a product roadmap.

There are **no published packages** yet (no AUR listing). On Manjaro / Arch, use the install script below.

![SimpleMD split editor with live preview](docs/screenshot.png)

## Features

- **Split-pane editing.** Markdown on the left, live preview on the right.
- **Rich preview.** Math (KaTeX), chemistry (mhchem), and Mermaid diagrams.
- **Document outline.** Jump between headings in long documents.
- **Insert menus.** Snippets for common markdown, math, and citations.
- **Images.** Insert or paste; on save, optionally copy into an `images/` folder beside the document.
- **PDF export.** Print or save from the preview.
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

**Privacy:** each request sends your instruction plus either the current selection or ~1,200 characters of context around the cursor to the configured server. With an external provider, that content leaves your machine.

**Security:** the API key is stored in Qt settings on disk **in plain text** (not encrypted).

AI is optional. Without it, the editor runs normally (offline aside from WebEngine’s usual needs).

## Install

On Manjaro / Arch, from the repository root:

```bash
./scripts/install-manjaro.sh
```

The script installs dependencies, builds a local package, and installs it with `pacman`. Do not run it as root. Launch with `simplemd` or from the application menu.

## Uninstall

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

## Third-party licenses

| Component | License |
| --- | --- |
| [Qt 6](https://www.qt.io/) | LGPL-3.0-only |
| [Qt WebEngine](https://doc.qt.io/qt-6/qtwebengine-index.html) | LGPL-3.0-only; includes [Chromium](https://www.chromium.org/) (BSD-3-Clause and others) |
| [KDE Frameworks 6](https://api.kde.org/) (Kirigami, KI18n, KCoreAddons, …) | LGPL-2.0-or-later / LGPL-2.1-or-later |
| [CMake](https://cmake.org/) | BSD-3-Clause |
| [Extra CMake Modules](https://api.kde.org/ecm/) | BSD-2-Clause |
| [marked](https://github.com/markedjs/marked) | MIT |
| [KaTeX](https://katex.org/) | MIT |
| [mhchem](https://github.com/KaTeX/KaTeX/tree/main/contrib/mhchem) | Apache-2.0 |
| [Mermaid](https://mermaid.js.org/) | MIT |

Bundled under `resources/preview/vendor/`. Mermaid includes additional libraries (Lodash, DOMPurify, js-yaml, …); see license comments in `mermaid.min.js`.

## License

SimpleMD is licensed under **GPL-3.0-or-later**.
