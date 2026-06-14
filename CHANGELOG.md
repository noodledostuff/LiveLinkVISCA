# Changelog

## 1.0.0 - 2026-06-14

- Prepared the plugin for public GitHub and Fab release.
- Added release metadata, support/documentation URLs, license, changelog, and Fab checklist.
- Added `PackagePlugin.bat` for Unreal AutomationTool plugin packaging.
- Made `BuildPlugin.bat` derive the host editor target from the `.uproject` name.
- Hardened queued VISCA packet handling across source restarts and shutdowns.
- Removed generated Unreal build output from release tracking.

## 0.1.0

- Added the VISCA over IP Live Link source.
- Added UDP listen mode and TCP session mode with VISCA-over-IP replies.
- Added standard Live Link Camera-role output and a custom VISCA Camera role.
- Added `Live Link VISCA Camera` component for applying camera, lens, exposure, and extended VISCA state.
