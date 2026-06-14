# VISCA-over-IP Command Capture Utility

Standalone Windows-friendly utility for capturing VISCA-over-IP commands sent by a control panel. It does not depend on Unreal Engine and uses only Python's standard library.

## Run

From the `LiveLinkVISCA` plugin root, double-click:

```bat
Tools\ViscaIpCapture\Run_VISCA_Capture.bat
```

Or run from a terminal:

```bat
cd Tools\ViscaIpCapture
python visca_capture.py
```

The default listen port is `52381`, matching the LiveLink VISCA plugin defaults.

## Typical Setup

1. Set the control panel camera destination IP to this PC's IP address.
2. Set the panel destination port to `52381`, or the port shown in the app.
3. Choose `UDP`, `TCP`, or `Both` to match the panel.
4. Click `Start`.
5. Press buttons or move controls on the panel.
6. Use `Save CSV` or `Save JSONL` to export the capture.

If the panel uses TCP or waits for camera feedback, enable `Send ACK/completion replies` before starting capture.

## Notes

- Only one app can bind the same protocol and port at the same time. Stop Unreal's LiveLink VISCA receiver or use a different port while this capture tool is running.
- The utility is capture-only. It does not forward commands to Unreal or to a physical camera.
- If Windows Firewall prompts for Python network access, allow access on the network used by the control panel.
- The app captures both raw VISCA payloads such as `81 01 04 07 00 FF` and 8-byte VISCA-over-IP envelopes.

## Console Mode

For headless logging:

```bat
python visca_capture.py --console --transport Both --port 52381 --send-replies
```

Run decoder/parser checks:

```bat
python visca_capture.py --self-test
```
