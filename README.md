# LiveLinkVISCA

**Unreal Engine plugin** that adds a **Live Link** source for **VISCA over IP** (Sony-style PTZ and camera control). Incoming VISCA commands drive **Live Link Camera** subjects so virtual cameras follow pan, tilt, zoom, and basic lens data from hardware or software controllers.

Designed for **virtual production**, **broadcast**, and **previz** workflows where PTZ cameras or control surfaces speak VISCA over a network.

---

## Features

- **Live Link source** — Listen on configurable **IPv4 bind address** and **ports**; one **subject** per receiver.
- **Transport** — **UDP** (listen-only) or **TCP** (session with VISCA-over-IP style **ACK/reply** handling for controllers that expect it).
- **Camera role** — Publishes **rotation** (pan/tilt), **focal length**, **aperture**, and **focus distance** where supported by the stream.
- **Live Link VISCA Camera** component — Applies a chosen subject to the owner actor’s **Camera** / **Cine Camera**, with the same **evaluation and editor ticking** options as Unreal’s **Live Link Controller** (spawnable / Sequencer behavior, update in editor, Blueprint preview).

---

## Requirements

- Unreal Engine **5.x** (developed and tested against **5.7**; other 5.x versions may work with a project rebuild).
- Plugins enabled in your project (dependencies are declared in `LiveLinkVISCA.uplugin`):
  - **Live Link**
  - **Live Link Camera**

---

## Installation

1. Copy the **LiveLinkVISCA** folder into your project’s **Plugins** directory (for example `YourProject/Plugins/LiveLinkVISCA`), or clone this repository to that path.
2. Build the plugin (see [Build guide](#build-guide) below), then enable it in the editor.
3. Enable **Live Link: VISCA** in **Edit → Plugins → Virtual Production** (or search for “VISCA”).
4. Restart the editor if prompted.

---

## Build guide

The plugin module **LiveLinkVISCA** is compiled with your **game/editor target** (it is not a standalone executable). You need a C++ Unreal project that lists this plugin and the engine versions must match your `.uproject` (for example **EngineAssociation** `5.7`).

### Prerequisites

- **Windows:** Visual Studio 2022 with the workloads Epic documents for your engine version (typically **Game development with C++** and the MSVC toolset UE expects).
- **.NET:** Supplied with the engine; Unreal Build Tool uses it automatically.
- **Engine:** A full Unreal Editor install (Launcher or source) for the version your project uses.

### Project layout (for the included batch script)

`BuildPlugin.bat` assumes the plugin lives at:

`YourProject/Plugins/LiveLinkVISCA/`

and that **one** `.uproject` exists in `YourProject/` (two directories above the script). If your layout differs, set **`VISCA_UPROJECT`** to the full path of your `.uproject` before running the script (see below).

### Option A — Visual Studio / Rider (recommended for daily work)

1. Right-click your **`*.uproject`** → **Generate Visual Studio project files** (if you have not already).
2. Open the solution and build the **Editor** target for your game (for example **Development Editor** for `YourGameEditor`).
3. The **LiveLinkVISCA** module builds as a dependency of that target.

### Option B — `BuildPlugin.bat` (command line, Windows)

The repository ships **`BuildPlugin.bat`** next to `LiveLinkVISCA.uplugin`. It invokes the engine’s **`Build.bat`** for the reference project **`VISCALivelinkDevEditor`** (Win64). If you use another project name, open **`BuildPlugin.bat`** and change the target on the `call "%BUILD_BAT%" ...` line to **`YourGameEditor`** (your editor target name from `*.Target.cs`).

| Input | Description |
|--------|----------------|
| **Optional argument** | Build configuration: `Development` (default) or `Shipping`, etc. Example: `BuildPlugin.bat Shipping`. |
| **`UE_ROOT`** | If unset, the script tries `E:\UE_5.7` then `%ProgramFiles%\Epic Games\UE_5.7`. Override to your engine root (the folder that **contains** `Engine`). |
| **`VISCA_UPROJECT`** | Full path to your `.uproject` when the plugin is **not** under `Project/Plugins/LiveLinkVISCA`. |

**Examples**

```bat
cd Plugins\LiveLinkVISCA
BuildPlugin.bat
```

```bat
set UE_ROOT=C:\Program Files\Epic Games\UE_5.7
set VISCA_UPROJECT=D:\Work\MyGame\MyGame.uproject
Plugins\LiveLinkVISCA\BuildPlugin.bat
```

### Option C — Engine `Build.bat` directly

From a **Developer Command Prompt for VS** or any shell where `UE_ROOT` points at your install:

```bat
"%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" YourGameEditor Win64 Development -Project="D:\Work\YourGame\YourGame.uproject" -WaitMutex
```

Replace **`YourGameEditor`** and the **`-Project=`** path with your project.

### Packaging the plugin for distribution (optional)

To produce a **standalone packaged plugin** (for example for Fab), use **RunUAT** `BuildPlugin` with your engine’s **RunUAT.bat**, pointing **`-Plugin=`** at `LiveLinkVISCA.uplugin` and **`-Package=`** at an output folder. That path validates the plugin against a clean engine tree; your Fab pipeline may already wrap this step.

### Troubleshooting builds

| Issue | What to try |
|--------|----------------|
| **Engine not found** | Set **`UE_ROOT`** explicitly to the folder that contains **`Engine`**, not to `Engine\Binaries\...` alone. |
| **No .uproject found** | Use **`VISCA_UPROJECT`**, or move/sync the plugin under **`ProjectName/Plugins/LiveLinkVISCA`**. |
| **Wrong target / link errors** | Regenerate project files; ensure **Live Link** and **Live Link Camera** are enabled in the host `.uproject`; match engine version to **EngineAssociation**. |

---

## Quick start

### 1. Add the Live Link source

1. Open **Window → Virtual Production → Live Link**.
2. **Add** a source and choose **VISCA over IP**.
3. In **Source Settings**:
  - Set **Bind Address** (use `0.0.0.0` for all interfaces, or a specific local IPv4).
  - Under **Receivers**, add a row:
    - **Subject Name** — Name that appears as a Live Link subject (e.g. `Cam1`).
    - **Listen Port** — Typically **52381** (common VISCA over IP default) unless your device uses another port.
    - **Transport** — **UDP (Listen)** or **TCP (Session + Replies)** depending on your controller.
4. Confirm the source status shows **Listening** and that your subject appears in the Live Link list.

### 2. Drive a Cine Camera

1. Add a **Cine Camera** actor (or Camera + Cine Camera).
2. Add the **Live Link VISCA Camera** component to the same actor.
3. Set **Subject Name** to match the receiver’s subject.
4. Enable **Evaluate Live Link** and use **Update in Editor** if you want the viewport to update while editing.

---

## Live Link component options

These align with **Live Link Controller** behavior:


| Option                                        | Purpose                                                                                                                                     |
| --------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------- |
| **Evaluate Live Link**                        | Master on/off for applying frames to the camera.                                                                                            |
| **Disable Evaluate Live Link when Spawnable** | Skips evaluation when the owner is a **Level Sequence spawnable** (tagged `SequencerActor`), avoiding double-driving during take recording. |
| **Update in Editor**                          | Ticks in the **editor** level viewport (not only PIE).                                                                                      |
| **Update in Preview Editor**                  | Allows ticking in **Blueprint class** preview worlds; only relevant when **Update in Editor** is on.                                        |


**Camera Mapping → Map Lens Values from 0–1** — When enabled, focal length, f-stop, and focus distance from Live Link are treated as **normalized 0–1** and mapped to your Cine Camera’s lens limits.

---

## Transport: UDP vs TCP

- **UDP (Listen)** — Simple fire-and-forget; suitable when you only need Unreal to **receive** VISCA and do not need to **reply** on the same session.
- **TCP (Session + Replies)** — Use when the **controller expects ACK/completion** (VISCA over IP style). Required for many **desktop VISCA** apps and **hardware** that wait for replies.

Each **receiver** must use a **unique port** on the bind address.

---

## Troubleshooting


| Issue                            | Things to check                                                                                            |
| -------------------------------- | ---------------------------------------------------------------------------------------------------------- |
| Subject **invalid** or no frames | Source status text; firewall; correct **port** and **bind address**; device sending to this machine’s IP.  |
| **TCP** won’t connect            | Port not blocked; only one typical client per receiver; try **UDP** if the sender only supports datagrams. |
| Sequencer / spawnable conflicts  | Enable **Disable Evaluate Live Link when Spawnable** on the VISCA camera component.                        |
| No motion in editor              | Enable **Update in Editor** on the component.                                                              |


---

## Fab / marketplace

When publishing on **Fab**, set **Docs URL** and **Support URL** in your product page to match your support channels. This README is suitable to ship in the plugin root for buyers.

---

## Author

**Timothy YU** — [noodledostuff.com](https://www.noodledostuff.com)

---

## License

Include your license file in the package (e.g. `LICENSE`) and describe terms on Fab. If no license is bundled, all rights are reserved by the author unless stated otherwise.