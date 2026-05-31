# SimpleMD — Feature Suggestions

This document proposes new functionalities that would enhance **SimpleMD** (`org.kde.simplemd`), a KDE/Qt6 markdown editor with live WebEngine preview, math and chemistry rendering (KaTeX + mhchem), Mermaid diagrams, citation helpers, AI-assisted editing, and PDF export.

These suggestions build on what the application already does well — rich preview rendering, scroll sync, insert menus for technical writing, and KDE-native theming — and aim to close gaps that users of modern markdown editors typically expect.

## How to read this document

Each suggestion includes:

- **What** — brief feature description
- **Why** — user value or problem solved
- **Complexity** — Low / Medium / High (estimated effort in this codebase)
- **Touches** — key files likely involved

Priorities:

- **P1** — high impact, natural next steps
- **P2** — valuable enhancements
- **P3** — longer-term or niche features

## P1 summary (high-impact quick wins)

| Feature | Section | Complexity |
|---------|---------|------------|
| Find and replace | Core editing | Medium |
| Markdown syntax highlighting | Core editing | Medium |
| Open file from command line | File handling | Low |
| Drag-and-drop files and images | File handling | Low |
| Autosave and crash recovery | File handling | Medium |
| Draggable split pane | Preview and layout | Low |
| Live word/character count | Preview and layout | Low |
| Streaming AI responses | AI-assisted editing | Medium |
| MIME type / desktop Actions | KDE integration | Low |
| Translation catalog | Quality and i18n | Medium |
| Automated tests | Quality and i18n | Medium |
| User-facing README and shortcut reference | Quality and i18n | Low |

---

## 1. Core editing experience

SimpleMD today uses a plain `TextArea` in `SplitEditor.qml` with undo/redo, cut/copy/paste, and find-next/previous via a dialog in `Main.qml`. There is no syntax highlighting or replace functionality.

### Find and replace — P1

**What:** Extend the existing Find dialog with Replace, Replace All, and optional regex mode.

**Why:** Find-without-replace is a common gap. Users editing long documents need bulk corrections (typo fixes, renaming terms, updating links) without manual repetition.

**Complexity:** Medium

**Touches:** `src/Main.qml`, `src/SplitEditor.qml`

### Markdown syntax highlighting — P1

**What:** Color-code headings, lists, code fences, links, emphasis, and math delimiters in the editor pane.

**Why:** Syntax highlighting is standard in markdown editors and significantly improves readability and error spotting, especially for technical documents with nested structures.

**Complexity:** Medium

**Touches:** `src/SplitEditor.qml`, possibly a new C++ `QSyntaxHighlighter` or QML-based highlighter component

### Line numbers and current-line highlight — P2

**What:** Display line numbers in a gutter beside the editor and subtly highlight the line containing the cursor.

**Why:** Long documents, log-style notes, and code-heavy markdown benefit from quick positional reference and visual anchoring.

**Complexity:** Medium

**Touches:** `src/SplitEditor.qml`, new gutter component

### Go to line — P2

**What:** A Ctrl+G dialog that jumps the cursor to a specified line number.

**Why:** Complements find for navigation in large files; pairs naturally with line numbers.

**Complexity:** Low

**Touches:** `src/Main.qml`, `src/SplitEditor.qml`

### Bracket and pair matching — P3

**What:** Highlight matching parentheses, brackets, and code-fence backticks as the cursor moves.

**Why:** Reduces errors when editing nested LaTeX, links, or multi-line code blocks.

**Complexity:** Medium

**Touches:** `src/SplitEditor.qml`

### Vim/emacs keybinding profiles — P3

**What:** Optional keybinding presets selectable in Preferences.

**Why:** Power users migrating from terminal editors expect modal or emacs-style bindings; making this optional avoids disrupting default Qt shortcuts.

**Complexity:** High

**Touches:** `src/Main.qml`, `src/SplitEditor.qml`, Settings block

### Soft wrap toggle and visible whitespace — P2

**What:** Preferences to toggle word wrap and show spaces/tabs/newlines.

**Why:** Common editor preferences; long lines and indentation-sensitive content (tables, nested lists) are easier to edit with visible whitespace.

**Complexity:** Low

**Touches:** `src/SplitEditor.qml`, `src/Main.qml` (Settings)

### Spell checking — P2

**What:** Inline spell checking using Sonnet or Hunspell via KDE Frameworks.

**Why:** Natural fit for a KDE text editor; prose-heavy documents benefit without leaving the app.

**Complexity:** Medium

**Touches:** `src/SplitEditor.qml`, `CMakeLists.txt` (KF6 Sonnet dependency)

---

## 2. Document and file handling

File operations live in `DocumentController` with New/Open/Save/Save As, recent files (max 10), and unsaved-changes confirmation. `main.cpp` does not currently open files passed on the command line.

### Open file from command line — P1

**What:** Support `simplemd document.md` and `simplemd -- file.md` to open a file at launch.

**Why:** Standard desktop editor behavior; enables file-manager associations, shell scripts, and `xdg-open` workflows.

**Complexity:** Low

**Touches:** `src/main.cpp`, `src/documentcontroller.cpp`, `org.kde.simplemd.desktop`

### Drag-and-drop — P1

**What:** Drop `.md` files onto the window to open them; drop images into the editor to insert markdown image syntax at the cursor.

**Why:** No drag-and-drop handlers exist today; dropping files from Dolphin or a browser is a natural workflow.

**Complexity:** Low

**Touches:** `src/Main.qml`, `src/SplitEditor.qml`, `src/documentcontroller.cpp`

### Autosave and crash recovery — P1

**What:** Periodically save to a recovery file; offer to restore unsaved work after an unexpected exit.

**Why:** Only manual save exists today. Autosave protects against crashes and accidental closure without blocking intentional unsaved sessions.

**Complexity:** Medium

**Touches:** `src/documentcontroller.cpp`, `src/Main.qml`

### Multi-document tabs or split windows — P2

**What:** Open multiple documents in tabs within one window, or optionally in side-by-side split views.

**Why:** The single-document model limits users who reference notes, compare drafts, or edit related files simultaneously.

**Complexity:** High

**Touches:** `src/Main.qml`, `src/SplitEditor.qml`, `src/documentcontroller.cpp`

### File watcher — P2

**What:** Detect external changes to the open file and prompt to reload or keep local edits.

**Why:** Common when editing alongside git, build tools, or sync services that modify files externally.

**Complexity:** Medium

**Touches:** `src/documentcontroller.cpp`

### Session restore — P2

**What:** On launch, optionally reopen the files that were open in the previous session.

**Why:** Extends the existing recent-files list into a fuller workflow restoration, reducing friction for daily users.

**Complexity:** Medium

**Touches:** `src/documentcontroller.cpp`, `src/main.cpp`, `QSettings`

### Export to HTML — P2

**What:** Export the rendered preview as a standalone HTML file with embedded CSS and base64-encoded local images.

**Why:** The preview pipeline in `preview.html` already produces fully rendered output; exporting it enables sharing, hosting, and archival without PDF.

**Complexity:** Medium

**Touches:** `resources/preview/preview.html`, `src/previewhelper.cpp`, `src/Main.qml`

### Import from other formats — P3

**What:** Convert `.docx`, `.odt`, or HTML clipboard/content to markdown on import.

**Why:** Users receiving content from Word or Google Docs need a path into markdown; external tool integration (e.g. pandoc) keeps scope manageable.

**Complexity:** High

**Touches:** New import module, `src/documentcontroller.cpp`

### Git integration — P3

**What:** Optional diff view of unsaved changes, or a commit-message editing mode with diff preview.

**Why:** Niche but valuable for technical writers who use markdown for documentation in git repositories.

**Complexity:** High

**Touches:** New git helper, `src/Main.qml`

---

## 3. Preview and layout

The editor and preview share a fixed 50/50 horizontal split in `SplitEditor.qml`. Bidirectional scroll sync and active-block highlighting already work via WebChannel. Preview can be toggled with F9.

### Draggable split pane — P1

**What:** Allow users to drag the divider between editor and preview to adjust the split ratio.

**Why:** A fixed 50/50 ratio does not suit all workflows; some users want a wider preview for layout review, others a wider editor for drafting.

**Complexity:** Low

**Touches:** `src/SplitEditor.qml`

### Preview-only, editor-only, and side-by-side modes — P2

**What:** Layout modes including vertical stack (editor above preview) in addition to the current horizontal split and preview-hidden mode.

**Why:** Preview toggle exists; richer layout options accommodate different screen sizes and writing styles.

**Complexity:** Medium

**Touches:** `src/SplitEditor.qml`, Settings

### Click-to-sync cursor — P2

**What:** Clicking a block in the preview moves the editor cursor to the corresponding source location.

**Why:** Inverse of existing scroll sync; helps users jump from rendered output back to source when reviewing formatting.

**Complexity:** Medium

**Touches:** `resources/preview/preview.html`, `src/SplitEditor.qml`

### Scroll-locked typing — P2

**What:** While typing, keep the active preview block centered or pinned in view.

**Why:** Refinement of existing bidirectional sync; reduces disorientation when editing mid-document with a live preview.

**Complexity:** Medium

**Touches:** `src/SplitEditor.qml`, `resources/preview/preview.html`

### Custom preview CSS themes — P2

**What:** Selectable preview themes (academic, dark, print-like, minimal) beyond the current system-theme-derived styling.

**Why:** Theme colors are already passed to the preview via `PreviewHelper`; named themes would support different publishing contexts.

**Complexity:** Medium

**Touches:** `src/previewhelper.cpp`, `resources/preview/preview.html`, `src/Main.qml`

### Remote image loading toggle — P2

**What:** A preference to allow or block remote image URLs in the preview.

**Why:** `localContentCanAccessRemoteUrls: false` on the WebEngineView blocks remote assets today; a toggle lets users choose between security and convenience.

**Complexity:** Low

**Touches:** `src/SplitEditor.qml`, Settings

### Table of contents panel — P2

**What:** A sidebar panel listing document headings, clickable to jump to sections.

**Why:** Long documents with many headings benefit from navigation; headings are already parsed during preview rendering.

**Complexity:** Medium

**Touches:** `src/Main.qml`, `resources/preview/preview.html`

### Presentation mode — P3

**What:** Fullscreen preview mode that displays sections as slides, advancing with keyboard or click.

**Why:** Leverages the existing fullscreen default and rich preview rendering for talks or review sessions.

**Complexity:** Medium

**Touches:** `src/Main.qml`, `resources/preview/preview.html`

### Live word and character count — P1

**What:** A status bar showing word count, character count, and optionally reading time.

**Why:** Simple to add and frequently requested for essays, articles, and documents with length constraints. No status bar exists today.

**Complexity:** Low

**Touches:** `src/Main.qml`, `src/SplitEditor.qml`

---

## 4. Rich authoring tools

`Main.qml` provides extensive Insert menus for structure, lists, formatting, math, chemistry, Mermaid diagrams, and citations. These suggestions extend that authoring surface.

### User-defined snippets and templates — P2

**What:** Let users save and insert custom markdown snippets via a menu or abbreviation trigger.

**Why:** Insert menus are fixed; snippets personalize workflow for repeated structures (meeting notes, lab report sections, boilerplate front matter).

**Complexity:** Medium

**Touches:** `src/Main.qml`, new snippet storage (QSettings or JSON file)

### Bibliography file support — P2

**What:** Load BibTeX or CSL-JSON bibliography files and render `[@key]` citations and reference lists in the preview.

**Why:** Citation insert helpers exist; a full bibliography pipeline would complete the academic writing story.

**Complexity:** High

**Touches:** `resources/preview/preview.html`, new bibliography parser, `src/Main.qml`

### Footnote preview panel — P3

**What:** A panel listing footnotes with live preview text, linked to their references in the document.

**Why:** Footnote references are already styled in preview; a dedicated panel aids documents with many notes.

**Complexity:** Medium

**Touches:** `src/Main.qml`, `resources/preview/preview.html`

### Visual Mermaid diagram editor — P3

**What:** A simple GUI for building flowcharts and sequence diagrams, outputting Mermaid syntax into the document.

**Why:** High effort but complements existing Mermaid insert templates; lowers the barrier for users unfamiliar with diagram syntax.

**Complexity:** High

**Touches:** New QML dialog, `src/Main.qml`

### Paste-from-clipboard smart cleanup — P2

**What:** When pasting from Word, Google Docs, or rich HTML, convert to clean markdown.

**Why:** A common pain point; users frequently paste formatted content and must manually strip styling.

**Complexity:** Medium

**Touches:** `src/SplitEditor.qml`, new HTML-to-markdown converter

### Image resize and alignment helpers — P2

**What:** Dialogs or inline helpers for width, alignment, and caption syntax when inserting or editing images.

**Why:** Basic image insert exists; authoring aids for layout control are missing.

**Complexity:** Low

**Touches:** `src/Main.qml`, `src/SplitEditor.qml`

### Document outline sidebar with reorder — P3

**What:** A sidebar showing the heading hierarchy with drag-and-drop to reorder sections (updating source markdown).

**Why:** Requires structural editing beyond insert helpers; valuable for long documents but architecturally significant.

**Complexity:** High

**Touches:** `src/Main.qml`, `src/SplitEditor.qml`, markdown parser

---

## 5. AI-assisted editing

AI integration in `LlmClient` uses OpenAI-compatible `/chat/completions` endpoints. "Edit with AI" (Ctrl+Shift+I) sends selection or cursor context (±1200 characters) and replaces or inserts the model output.

### Streaming responses — P1

**What:** Display AI output token-by-token as it arrives instead of waiting for the full response.

**Why:** Current flow blocks until completion; streaming improves perceived responsiveness and lets users cancel early if output goes wrong.

**Complexity:** Medium

**Touches:** `src/llmclient.cpp`, `src/Main.qml`

### AI actions menu — P2

**What:** Preset actions such as Summarize, Expand, Fix grammar, Simplify, and Translate in addition to the generic edit prompt.

**Why:** Most users want task-specific AI operations rather than a single open-ended edit command.

**Complexity:** Low

**Touches:** `src/Main.qml`, `src/llmclient.cpp`

### Local model support — P2

**What:** Document and preset common local endpoints (Ollama, llama.cpp server) in Preferences.

**Why:** Privacy-conscious users and offline workflows need first-class local model support beyond manual URL entry.

**Complexity:** Low

**Touches:** `src/Main.qml` (Preferences), documentation

### Diff preview before applying AI output — P2

**What:** Show a side-by-side or inline diff of original text vs. proposed AI changes before the user accepts.

**Why:** Safer than direct replace; users can review and reject unwanted modifications.

**Complexity:** Medium

**Touches:** `src/Main.qml`, new diff dialog component

### AI chat sidebar — P3

**What:** A multi-turn chat panel aware of the current document, for questions and iterative edits.

**Why:** Larger UI and context-management scope; suits users who want conversational assistance rather than one-shot edits.

**Complexity:** High

**Touches:** `src/Main.qml`, `src/llmclient.cpp`

### Custom prompt library — P2

**What:** Save, name, and reuse custom prompts beyond the single system-prompt setting.

**Why:** Extends existing AI configuration; users with repeatable workflows (style guides, translation rules) benefit from a prompt library.

**Complexity:** Low

**Touches:** `src/Main.qml`, Settings

---

## 6. Export, print, and publishing

Printing uses `PrintHelper` to render the preview to a temporary PDF via WebEngine, then prints via `QPdfDocument`. PDF export (Ctrl+Shift+E) supports layout presets, page size, orientation, margins, and optional page numbers.

### Export presets — P2

**What:** Save and load named PDF export profiles (margins, page size, layout) in addition to remembering the last-used settings.

**Why:** Users with different document types (letters, manuscripts, slides handouts) switch between export configurations frequently.

**Complexity:** Low

**Touches:** `src/Main.qml`, Settings

### Header and footer templates — P2

**What:** Configurable headers and footers with placeholders for title, date, author, and "Page X of Y".

**Why:** Page numbers exist; a template system supports formal documents and institutional requirements.

**Complexity:** Medium

**Touches:** `resources/preview/preview.html`, `src/Main.qml`

### Print preview dialog — P2

**What:** A WYSIWYG preview of paginated output before sending to the printer.

**Why:** Current flow prints via a temp PDF without showing the user what will print; preview reduces wasted paper and surprises.

**Complexity:** Medium

**Touches:** `src/printhelper.cpp`, `src/Main.qml`

### Export to LaTeX or Typst — P3

**What:** Convert markdown (especially math and chemistry content) to LaTeX or Typst source.

**Why:** Academic users already write math-heavy content; export to print-oriented formats supports journal and thesis workflows.

**Complexity:** High

**Touches:** New export module, `resources/preview/preview.html`

### Publish to static site — P3

**What:** Export a folder structure suitable for Hugo, Jekyll, or similar static site generators.

**Why:** Batch HTML plus assets enables documentation and blog workflows from within the editor.

**Complexity:** High

**Touches:** New export module, `src/previewhelper.cpp`

### Watermark support — P3

**What:** Optional diagonal watermark text (e.g. "DRAFT") on PDF export.

**Why:** Niche publishing need for review copies and internal documents.

**Complexity:** Low

**Touches:** `resources/preview/preview.html`, PDF export dialog

---

## 7. KDE and desktop integration

SimpleMD is a KDE-native app using Kirigami, Breeze theming, and KF6 i18n markers. Packaging currently targets Arch Linux via PKGBUILD.

### MIME type and desktop Actions — P1

**What:** Register `text/markdown` MIME handling and add `.desktop` Actions such as "Edit with SimpleMD" for file managers.

**Why:** `org.kde.simplemd.desktop` exists but can be extended so Dolphin and other file managers offer SimpleMD as a default or context-menu option.

**Complexity:** Low

**Touches:** `org.kde.simplemd.desktop`, `CMakeLists.txt`

### KDE Purpose framework — P2

**What:** Integrate with Purpose (Share, OpenWith) for sending document excerpts or opening related files.

**Why:** Better Dolphin and KDE application interoperability.

**Complexity:** Medium

**Touches:** New Purpose plugin or integration, `CMakeLists.txt`

### Global shortcuts and Plasma widget — P3

**What:** A global hotkey to open a quick-capture window, or a Plasma widget for scratch notes.

**Why:** Ecosystem play; positions SimpleMD as a quick-access note tool on the KDE desktop.

**Complexity:** High

**Touches:** Separate widget/quick-capture component, Plasma integration

### Window mode preferences — P2

**What:** Option to launch in a normal resizable window instead of fullscreen by default.

**Why:** The app defaults to `Window.FullScreen`, which is unusual for a text editor and may surprise new users.

**Complexity:** Low

**Touches:** `src/Main.qml`, Settings

### Secret Service for API key storage — P2

**What:** Store the AI API key in KWallet or Freedesktop Secret Service instead of plain `QSettings`.

**Why:** API keys in plain settings are a security concern; KDE apps typically use secure credential storage.

**Complexity:** Medium

**Touches:** `src/llmclient.cpp`, `src/Main.qml`, KWallet dependency

### Broader packaging and AppStream metadata — P2

**What:** Flatpak, Snap, and official KDE Gear release alignment with AppStream metadata for software centers.

**Why:** Only Arch PKGBUILD packaging exists today; broader distribution increases adoption on other distros.

**Complexity:** Medium

**Touches:** `packaging/`, new Flatpak manifest, AppStream XML

---

## 8. Accessibility, internationalization, and quality

The codebase uses `i18n` markers throughout QML and C++ but has no `po/` translation catalog. There are no automated tests or CI workflows.

### Translation catalog — P1

**What:** Add `po/` files and wire up KDE localization infrastructure so translators can contribute.

**Why:** i18n markers are present throughout the app; without `.po` files, the application cannot be translated despite being prepared for it.

**Complexity:** Medium

**Touches:** New `po/` directory, `CMakeLists.txt`, `src/main.cpp`

### High-contrast and reduced-motion modes — P2

**What:** Respect system high-contrast settings and offer a reduced-motion option for preview animations and Mermaid rendering.

**Why:** Preview content includes animations and WebEngine rendering that may be problematic for accessibility preferences.

**Complexity:** Medium

**Touches:** `resources/preview/preview.html`, `src/Main.qml`

### Screen reader improvements — P2

**What:** Improve accessibility metadata for the editor pane (role, labels, cursor position announcements).

**Why:** Plain `TextArea` may lack rich accessibility information needed by screen reader users.

**Complexity:** Medium

**Touches:** `src/SplitEditor.qml`

### Automated tests — P1

**What:** QTest unit tests for C++ helpers (`DocumentController`, `PreviewHelper`, `LlmClient`) and QML tests for preview sync behavior.

**Why:** No test targets exist in `CMakeLists.txt`; tests protect against regressions as features are added.

**Complexity:** Medium

**Touches:** `CMakeLists.txt`, new `tests/` directory

### CI pipeline — P2

**What:** GitHub Actions or GitLab CI to build on each push, run tests, and execute qmllint.

**Why:** No CI configuration is visible; automated builds catch breakage early across contributors and distros.

**Complexity:** Low

**Touches:** New `.github/workflows/` or `.gitlab-ci.yml`

### User-facing README and shortcut reference — P1

**What:** A README with build instructions, feature overview, and a keyboard shortcut table.

**Why:** The repository has no markdown documentation; new contributors and users lack a starting point.

**Complexity:** Low

**Touches:** New `README.md` at project root

### Wire or remove unused KaTeX auto-render asset — P3

**What:** Either integrate `resources/preview/vendor/contrib/auto-render.min.js` into the math pipeline or remove the unused file.

**Why:** The asset exists but is not included in `CMakeLists.txt` or `preview.html`; math uses a custom protect/inject approach instead. Cleaning this up reduces confusion.

**Complexity:** Low

**Touches:** `resources/preview/preview.html`, `src/CMakeLists.txt`

---

## Closing notes

This list is intentionally broad. Not every item needs to ship — priorities depend on target audience (students, academics, general note-takers) and maintenance capacity. A practical approach:

1. **Start with P1 items** that close obvious gaps (CLI open, find/replace, split pane, status bar, tests, README).
2. **Layer P2 enhancements** that deepen the editing and preview experience (syntax highlighting, autosave, AI streaming, preview themes).
3. **Evaluate P3 items** against user demand before committing to high-effort features (tabs, git integration, visual diagram editor).

Contributions and reordering of priorities are welcome as the project evolves.
