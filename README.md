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

1. Copy the `**LiveLinkVISCA`** folder into your project’s `**Plugins**` directory (or clone this repo into `Plugins/LiveLinkVISCA`).
2. Regenerate project files and **compile** the editor target.
3. Enable **Live Link: VISCA** in **Edit → Plugins → Virtual Production** (or search for “VISCA”).
4. Restart the editor if prompted.

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