# Changelog

## 1.0.1 - 2026-06-14

- Kept VISCA exposure compensation isolated from post-process ISO and shutter settings.
- Switched VISCA exposure application to histogram auto exposure with a wider exposure compensation range.
- Applied VISCA iris commands to Cine Camera aperture.
- Applied VISCA auto/manual focus mode to the Cine Camera and reduced manual focus drive speed.

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
