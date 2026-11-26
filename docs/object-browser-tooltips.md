# Object browser tooltip coverage

The preprocessor and 2D/3D postprocessor trees now provide Japanese-translated operation help for standard display items, their result-selection children, and shared items such as background maps, measured values, axes, distance measures, titles, and time labels.

Solver-provided descriptions remain authoritative for grid attributes and boundary conditions. Scalar result displays can show output metadata alongside operation help. An isosurface retains its operation help when a result has no solver-provided description.

## Validation

- The implementation was built with Visual Studio 2022/v143 and Qt 5.14.2 in the existing 4.1.0.7161 test runtime (10 affected libraries).
- All 97 newly supplied Japanese translations were loaded from compiled QM files and compared with expected text. One isosurface translation already exists on this submission branch and was retained, leaving 96 newly added translations relative to the remote branch.
- Source inspection covered the standard tree-item constructors in the main preprocessor (17), post2D (33), and post3D (25), plus named leaf items and shared components. Internal controllers without a visible tree row are excluded.
- Native GUI checks confirmed Japanese popups for preprocessor grid-generation settings and node attributes, post3D velocity arrows, and post2D water depth. Post2D was checked both without output metadata and with a translated solver-specific description, including reopening the project. The user also confirmed the delivered test runtime.
- The changes were applied to remote commit `50ccfba670d277327794f381322c3a06bd65e589`, preserving upstream toolbar changes and the existing isosurface translation. The submission diff passes whitespace checks; its translation XML and compiled QM files were rechecked. The complete newer upstream base has not been rebuilt locally.

This does not claim exhaustive GUI coverage of every solver, external plugin, or auxiliary window. No solver binaries, simulation data, or unrelated restart features are included.
