# Third-party licenses

This repository bundles third-party assets used by the preview renderer under:

- `resources/preview/vendor/`

## Bundled dependencies

| Component | License | Notes |
| --- | --- | --- |
| [marked](https://github.com/markedjs/marked) | MIT | Bundled as `marked.min.js` |
| [KaTeX](https://katex.org/) | MIT | Bundled as `katex.min.js` and fonts/CSS assets |
| [mhchem for KaTeX](https://github.com/KaTeX/KaTeX/tree/main/contrib/mhchem) | Apache-2.0 | Bundled as `contrib/mhchem.min.js` |
| [Mermaid](https://mermaid.js.org/) | MIT | Bundled as `mermaid.min.js` |

## Mermaid bundled/transitive libraries

The bundled Mermaid build also includes code from additional libraries. License notices for these are embedded in `mermaid.min.js`.

| Library | License |
| --- | --- |
| [D3 and related D3 modules](https://github.com/d3) | ISC |
| [dagre-d3](https://github.com/dagrejs/dagre-d3) and [dagre](https://github.com/dagrejs/dagre) | MIT |
| [DOMPurify](https://github.com/cure53/DOMPurify) | Apache-2.0 OR MPL-2.0 |
| [js-yaml](https://github.com/nodeca/js-yaml) | MIT |
| [lodash](https://github.com/lodash/lodash) | MIT |
| [dayjs](https://github.com/iamkun/dayjs) | MIT |
| [cytoscape](https://github.com/cytoscape/cytoscape.js) | MIT |
| [uuid](https://github.com/uuidjs/uuid) | MIT |
| [jQuery snippet notices](https://github.com/jquery/jquery) | MIT |
| Embedded Bezier/Runge-Kutta helper notices in Mermaid bundle | MIT |

## Platform/build/runtime dependencies

SimpleMD also relies on system-provided dependencies at build/runtime (Qt, Qt WebEngine/Chromium, KDE Frameworks, CMake, ECM). Their licenses are provided by the corresponding upstream packages/distributions.
