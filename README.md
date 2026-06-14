# LiveLinkVISCA

Unreal Engine plugin that adds a Live Link source for VISCA over IP. It receives Sony-style VISCA camera control commands and publishes camera state to Live Link so PTZ cameras, hardware controllers, and desktop VISCA tools can drive Unreal cameras.

The plugin is designed for virtual production, broadcast, previs, and test-control workflows where a VISCA sender should control a Cine Camera or expose camera state to Blueprint tools.

---

## Features

- Live Link source for VISCA over IP.
- Configurable IPv4 bind address and one receiver per Live Link subject.
- UDP listen mode for simple datagram senders.
- TCP session mode with VISCA-over-IP ACK/completion replies for controllers that wait for responses.
- Standalone VISCA-over-IP command capture utility for inspecting control panel traffic outside Unreal.
- Standard Live Link Camera subject for compatibility with existing Camera-role workflows.
- Additive custom VISCA Camera role subject named `<Subject>_VISCA` with extended VISCA state.
- Continuous pan, tilt, zoom, and focus drive until a stop/direct/home/reset command is received.
- Simulated state and inquiry replies for common VISCA commands.
- Live Link VISCA Camera component for applying standard and extended data to Camera/Cine Camera actors.

---

## Requirements

- Unreal Engine 5.x.
- Developed and tested against Unreal Engine 5.7.
- Required plugins:
  - Live Link
  - Live Link Camera

The included scripts look for UE 5.7 by default. Set `UE_ROOT` when using another installed engine path.

---

## Installation

1. Copy or clone `LiveLinkVISCA` into your project plugin folder:

```text
YourProject/Plugins/LiveLinkVISCA/
```

2. Build the plugin.
3. Enable `Live Link: VISCA` in `Edit > Plugins > Virtual Production`.
4. Restart the editor if prompted.

---

## Build Guide

The plugin is compiled with your project editor target. It is not a standalone executable.

### Prerequisites

- Windows.
- Visual Studio 2022 with the C++ toolchain required by your Unreal version.
- A full Unreal Engine 5.7 install.
- A C++ Unreal project that includes this plugin.

### BuildPlugin.bat

The included `BuildPlugin.bat` expects this layout:

```text
YourProject/
  YourProject.uproject
  Plugins/
    LiveLinkVISCA/
      BuildPlugin.bat
```

Run:

```bat
cd Plugins\LiveLinkVISCA
BuildPlugin.bat
```

Optional inputs:

| Input | Description |
| --- | --- |
| `BuildPlugin.bat Shipping` | Builds another configuration instead of the default `Development`. |
| `UE_ROOT` | Engine root folder. If unset, the script checks `E:\UE_5.7`, `D:\UE_5.7`, and `%ProgramFiles%\Epic Games\UE_5.7`. |
| `VISCA_UPROJECT` | Full path to the host `.uproject` when the plugin is not under `Project/Plugins/LiveLinkVISCA`. |

Example:

```bat
set UE_ROOT=C:\Program Files\Epic Games\UE_5.7
set VISCA_UPROJECT=D:\Work\MyGame\MyGame.uproject
Plugins\LiveLinkVISCA\BuildPlugin.bat
```

### PackagePlugin.bat

Use `PackagePlugin.bat` to create a release archive with Unreal AutomationTool:

```bat
cd Plugins\LiveLinkVISCA
PackagePlugin.bat Win64
```

The script runs `RunUAT BuildPlugin` and writes:

```text
Plugins/LiveLinkVISCA/Releases/LiveLinkVISCA.zip
```

The zip contains one top-level `LiveLinkVISCA` folder and is intended for GitHub release assets and Fab project file hosting.

---

## Quick Start

### Add The Source

1. Open `Window > Virtual Production > Live Link`.
2. Add a source and choose `VISCA over IP`.
3. In Source Settings:
   - Set `Bind Address` to `0.0.0.0` for all interfaces, or a specific local IPv4.
   - Add one or more receivers.
   - Set `Subject Name`, for example `Cam1`.
   - Set `Listen Port`, typically `52381`.
   - Choose `UDP (Listen)` or `TCP (Session + Replies)`.
4. Confirm the source status shows that it is listening.

### Drive A Cine Camera

1. Add a Cine Camera actor.
2. Add the `Live Link VISCA Camera` component to the same actor.
3. Set `Subject Name` to the standard subject, for example `Cam1`.
4. Enable `Evaluate Live Link`.
5. Enable `Update in Editor` if editor viewport updates are required.

The component automatically looks for the matching custom VISCA subject named `Cam1_VISCA` and applies supported extended camera settings when that subject is available.

---

## VISCA Command Capture Utility

The plugin includes a standalone command capture utility at:

```text
Tools/ViscaIpCapture/
```

Use it when validating a control panel, checking whether commands are reaching the machine, or identifying the exact VISCA bytes a panel sends before configuring the Live Link source.

Run on Windows:

```bat
Tools\ViscaIpCapture\Run_VISCA_Capture.bat
```

The utility listens on `52381` by default, supports `UDP`, `TCP`, and `Both`, decodes common VISCA commands, displays raw packet hex, and exports captures to CSV or JSONL. Enable `Send ACK/completion replies` when testing TCP panels or any panel that waits for camera responses.

Only one process can bind the same protocol and port. Stop the Unreal Live Link VISCA source or choose another port while the capture utility is running.

---

## Live Link Subjects

For each receiver subject, the source publishes two subjects:

| Subject | Role | Purpose |
| --- | --- | --- |
| `<Subject>` | Camera Role | Standard Unreal camera data: rotation, focal length, aperture, focus distance. |
| `<Subject>_VISCA` | VISCA Camera Role | Extended VISCA camera state in a custom Live Link role. |

Example:

| Receiver Subject Name | Published Subjects |
| --- | --- |
| `Cam1` | `Cam1`, `Cam1_VISCA` |

Use `Cam1` for maximum compatibility with existing Live Link Camera tooling. Use `Cam1_VISCA` when Blueprint or custom systems need direct access to VISCA-specific fields.

---

## Supported Control Areas

The plugin currently tracks and/or applies these command areas:

| Area | Behavior |
| --- | --- |
| Pan/Tilt | Continuous drive, stop, absolute, relative, home, reset. |
| Zoom | Continuous standard/variable/high-resolution drive, stop, direct position. |
| Focus | Continuous standard/variable drive, stop, direct position, auto/manual/toggle. |
| Iris | Step up/down, auto iris state. |
| Presets | Set, reset, recall. Stored in plugin runtime state. |
| Power/Menu/Color Bar | State tracking and inquiry replies. |
| ND Filter | Mode, preset, variable value, auto ND, clear/filter state. |
| Exposure | AE level, backlight compensation, spotlight compensation. |
| Shutter/Gain | Auto shutter and AGC state. |
| White Balance | Mode, preset/color temperature, R/B gain approximation. |
| Detail/Knee | Enable states, level, knee mode. |
| Tally/Recording | Red/green tally and recording press/release state. |
| Audio | Audio level mode, channel, and normalized input level. |

Commands that do not map cleanly to Unreal Camera/Cine Camera properties are still tracked in the custom VISCA role and Live Link metadata.

---

## Custom VISCA Camera Role

The custom role is `ULiveLinkVISCACameraRole`.

It uses:

- `FLiveLinkVISCACameraStaticData`
- `FLiveLinkVISCACameraFrameData`
- `FLiveLinkVISCACameraBlueprintData`

The custom frame data extends standard camera frame data and adds VISCA fields such as:

- `bPowerOn`
- `bMenuOpen`
- `bColorBar`
- `bAutoFocus`
- `bAutoIris`
- `bAgc`
- `bAutoShutter`
- `bBacklightCompensation`
- `bSpotlightCompensation`
- `bDetailEnabled`
- `bKneeEnabled`
- `bVariableNdMode`
- `bAutoNd`
- `bNdFiltered`
- `bTallyRed`
- `bTallyGreen`
- `bRecording`
- `bAudioLevelAuto`
- `NdPreset`
- `WhiteBalanceMode`
- `KneeMode`
- `AudioLevelChannel`
- `NormalizedNd`
- `NormalizedAeLevel`
- `NormalizedDetailLevel`
- `NormalizedWhiteTemperature`
- `NormalizedRGain`
- `NormalizedBGain`
- `NormalizedAudioInputLevel`

Blueprints and custom tooling can evaluate the `<Subject>_VISCA` subject with the VISCA Camera role to read these fields directly.

---

## Cine Camera Application

The `Live Link VISCA Camera` component applies:

- Actor rotation from pan/tilt.
- Focal length from zoom.
- Aperture from iris.
- Manual focus distance from focus.
- Exposure mode and exposure bias from VISCA AE/shutter state.
- White temperature and tint approximation from VISCA WB/R/B gain state.
- Physical exposure toggle, shutter speed, and ISO approximation from VISCA shutter/gain state.

Important limitation: Unreal does not expose one-to-one Cine Camera properties for every physical VISCA command. ND, tally, recording, many menu controls, and some detailed color controls remain available as custom role data and metadata rather than direct Cine Camera properties.

---

## Live Link Metadata

The standard Camera-role subject also includes extended VISCA values in `FrameData.MetaData.StringMetaData`. This keeps extended state accessible to tools that read metadata but do not evaluate the custom role.

Metadata keys include:

- `ViscaSubject`
- `ViscaPowerOn`
- `ViscaMenuOpen`
- `ViscaColorBar`
- `ViscaAutoFocus`
- `ViscaAutoIris`
- `ViscaAgc`
- `ViscaAutoShutter`
- `ViscaBacklightCompensation`
- `ViscaSpotlightCompensation`
- `ViscaDetailEnabled`
- `ViscaKneeEnabled`
- `ViscaVariableNdMode`
- `ViscaAutoNd`
- `ViscaNdFiltered`
- `ViscaTallyRed`
- `ViscaTallyGreen`
- `ViscaRecording`
- `ViscaAudioLevelAuto`
- `ViscaNdPreset`
- `ViscaWhiteBalanceMode`
- `ViscaKneeMode`
- `ViscaAudioLevelChannel`
- `ViscaNormalizedNd`
- `ViscaNormalizedAeLevel`
- `ViscaNormalizedDetailLevel`
- `ViscaNormalizedWhiteTemperature`
- `ViscaNormalizedRGain`
- `ViscaNormalizedBGain`
- `ViscaNormalizedAudioInputLevel`

Boolean metadata values are stored as `1` or `0`.

---

## Transport Notes

### UDP

UDP mode is stateless and suitable when Unreal only needs to receive commands.

### TCP

TCP mode accepts a VISCA session and sends ACK/completion replies. Use this for controllers that block until they receive a response.

Each receiver must bind a unique port on the selected bind address.

---

## Component Options

| Option | Purpose |
| --- | --- |
| `Evaluate Live Link` | Enables or disables applying Live Link data. |
| `Disable Evaluate Live Link when Spawnable` | Skips evaluation when the actor is a Sequencer spawnable tagged `SequencerActor`. |
| `Update in Editor` | Ticks in the editor viewport. |
| `Update in Preview Editor` | Allows ticking in Blueprint preview worlds when editor ticking is enabled. |
| `Map Lens Values from 0-1` | Treats incoming focal length, aperture, and focus as normalized values and maps them to Cine Camera lens limits. |

---

## Troubleshooting

| Issue | Checks |
| --- | --- |
| No subject appears | Confirm source status, bind address, firewall, and listen port. |
| No movement | Confirm the VISCA sender is targeting this machine and port. Confirm `Evaluate Live Link` is enabled. |
| No editor viewport update | Enable `Update in Editor` on the component. |
| TCP controller does not connect | Confirm only one client is using the receiver port and the port is not blocked. |
| Need to verify panel output | Run `Tools\ViscaIpCapture\Run_VISCA_Capture.bat` and point the panel at this machine and port. |
| Existing Camera-role tooling ignores extended state | Use the `<Subject>_VISCA` custom role subject or read metadata keys from the standard subject. |
| Extended settings do not exactly match hardware | Some VISCA fields are approximated because Unreal Cine Camera/PostProcess has different controls than physical cameras. |

---

## Known Limitations

- Runtime preset memory is not persisted across editor sessions.
- VISCA command coverage is broad but not exhaustive.
- Physical ND, tally, recording, and menu state do not have direct Cine Camera equivalents.
- Exposure, shutter, ISO, and white balance application is an Unreal post-process approximation.
- The source simulates camera state from incoming commands; it does not query a physical camera directly unless the controller sends inquiries to this plugin.

---

## Author

Timothy YU - [noodledostuff.com](https://www.noodledostuff.com)

---

## License

Copyright (c) 2026 Timothy YU. All rights reserved. See [LICENSE.md](LICENSE.md).

Fab purchases and downloads are also governed by the applicable Fab license and marketplace terms.
