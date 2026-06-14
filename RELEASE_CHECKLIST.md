# Release Checklist

Use this checklist before publishing a GitHub release or submitting the plugin to
Fab.

## Versioning

- Update `LiveLinkVISCA.uplugin`:
  - `Version`
  - `VersionName`
  - `Description`
  - `DocsURL`
  - `SupportURL`
- Update `CHANGELOG.md`.
- Tag GitHub releases as `vX.Y.Z`.

## Local Validation

- Build the plugin from a host C++ Unreal project:

```bat
BuildPlugin.bat Development
```

- Package the plugin with Unreal AutomationTool:

```bat
PackagePlugin.bat Win64
```

- Test the packaged plugin in a clean Unreal project.
- In the Live Link window, add `VISCA over IP`, create a receiver, and verify:
  - The standard subject appears.
  - The `<Subject>_VISCA` subject appears.
  - UDP commands update pan, tilt, zoom, and focus.
  - TCP commands receive ACK/completion replies.
  - The `Live Link VISCA Camera` component drives a Cine Camera.

## GitHub Release

- Confirm generated folders are not tracked:
  - `Binaries/`
  - `Intermediate/`
  - `Saved/`
  - `.vs/`
- Push to `main`. The `Release` GitHub Actions workflow reads
  `VersionName` from `LiveLinkVISCA.uplugin`, creates tag `vX.Y.Z`, builds
  `LiveLinkVISCA-X.Y.Z.zip`, and publishes it as a GitHub Release asset.
- If the release tag already exists, bump `VersionName` before pushing another
  release.
- The workflow zip is a clean source plugin package. Use `PackagePlugin.bat`
  locally or on a self-hosted Unreal runner when you need a UAT-built package
  with compiled platform binaries.
- Include version notes in `CHANGELOG.md`; the workflow uses the matching
  `## X.Y.Z` section as release notes.

## Fab Submission

- Upload a zip that contains exactly one top-level plugin folder.
- Confirm the plugin folder contains:
  - `LiveLinkVISCA.uplugin`
  - `Source/`
  - `Config/`
  - `Content/`
  - `Resources/`
- Keep generated development folders out of the archive unless Unreal
  AutomationTool intentionally generated platform binaries for the package.
- Provide the public download link in the Project File Link field.
- Make the download link accessible without account permissions. If the archive
  is password protected, put the password in Fab version notes.
- Add written documentation by linking the GitHub README or attaching equivalent
  setup notes.
- Prepare gallery images as PNG or JPEG, at least 1920x1080, under Fab's file
  size limits.

## Suggested Fab Listing Copy

Short description:

> Live Link source and camera component for receiving VISCA over IP PTZ, lens,
> exposure, tally, and extended camera state in Unreal Engine.

Technical details:

- Unreal Engine 5.x code plugin.
- Developed and tested with Unreal Engine 5.7.
- Requires the built-in Live Link and Live Link Camera plugins.
- Network modes: UDP listen and TCP session with ACK/completion replies.
- Primary classes:
  - `UViscaLiveLinkSourceFactory`
  - `ULiveLinkVISCACameraRole`
  - `UViscaCameraComponent`
