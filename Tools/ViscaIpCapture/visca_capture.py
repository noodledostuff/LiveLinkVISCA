#!/usr/bin/env python3
# Copyright (c) 2026 Timothy YU. All rights reserved.

"""Standalone VISCA-over-IP command capture utility.

This app listens like a VISCA camera endpoint so a hardware/software control
panel can target this machine and the received command bytes can be inspected.
It uses only the Python standard library.
"""

from __future__ import annotations

import argparse
import csv
import json
import queue
import socket
import threading
import time
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path
from typing import Callable, Iterable, Optional

try:
    import tkinter as tk
    from tkinter import filedialog, messagebox
    from tkinter import ttk
except ImportError:  # pragma: no cover - headless Python builds
    tk = None
    ttk = None
    filedialog = None
    messagebox = None


DEFAULT_PORT = 52381
MAX_DATAGRAM_SIZE = 65507
MAX_VISCA_IP_PAYLOAD = 1024


def bytes_to_hex(data: bytes) -> str:
    return " ".join(f"{byte:02X}" for byte in data)


def read_nibbles(payload: bytes, start: int, count: int) -> Optional[int]:
    if start < 0 or count < 0 or start + count > len(payload):
        return None
    value = 0
    for byte in payload[start : start + count]:
        value = (value << 4) | (byte & 0x0F)
    return value


def format_nibbles(payload: bytes, start: int, count: int) -> str:
    value = read_nibbles(payload, start, count)
    if value is None:
        return "n/a"
    width = max(1, count)
    return f"0x{value:0{width}X}"


def on_off(byte: int) -> str:
    return {0x02: "On", 0x03: "Off"}.get(byte, f"0x{byte:02X}")


def press_release(byte: int) -> str:
    return {0x01: "Press", 0x00: "Release"}.get(byte, f"0x{byte:02X}")


def up_down(byte: int) -> str:
    return {0x02: "Up", 0x03: "Down"}.get(byte, f"0x{byte:02X}")


def zoom_direction(byte: int) -> tuple[str, str]:
    if byte == 0x00:
        return "Zoom Stop", "Stop"
    if byte == 0x02:
        return "Zoom Tele", "Standard tele"
    if byte == 0x03:
        return "Zoom Wide", "Standard wide"
    high = byte & 0xF0
    speed = byte & 0x0F
    if high == 0x20:
        return "Zoom Tele Variable", f"Speed {speed}"
    if high == 0x30:
        return "Zoom Wide Variable", f"Speed {speed}"
    return "Zoom", f"Parameter 0x{byte:02X}"


def focus_direction(byte: int) -> tuple[str, str]:
    if byte == 0x00:
        return "Focus Stop", "Stop"
    if byte == 0x02:
        return "Focus Far", "Standard far"
    if byte == 0x03:
        return "Focus Near", "Standard near"
    high = byte & 0xF0
    speed = byte & 0x0F
    if high == 0x20:
        return "Focus Far Variable", f"Speed {speed}"
    if high == 0x30:
        return "Focus Near Variable", f"Speed {speed}"
    return "Focus", f"Parameter 0x{byte:02X}"


def focus_mode(byte: int) -> str:
    return {0x02: "Auto", 0x03: "Manual", 0x10: "Toggle"}.get(byte, f"0x{byte:02X}")


def menu_mode(byte: int) -> str:
    return {0x02: "On", 0x03: "Off", 0x10: "Toggle"}.get(byte, f"0x{byte:02X}")


def preset_action(byte: int) -> str:
    return {0x00: "Reset", 0x01: "Set", 0x02: "Recall"}.get(byte, f"0x{byte:02X}")


def pan_tilt_direction(pan_byte: int, tilt_byte: int) -> str:
    pan = {0x01: "Left", 0x02: "Right", 0x03: "Stop"}.get(pan_byte, f"0x{pan_byte:02X}")
    tilt = {0x01: "Up", 0x02: "Down", 0x03: "Stop"}.get(tilt_byte, f"0x{tilt_byte:02X}")
    if pan == "Stop" and tilt == "Stop":
        return "Stop"
    if pan == "Stop":
        return tilt
    if tilt == "Stop":
        return pan
    return f"{tilt} {pan}"


def wb_mode(byte: int) -> str:
    return {
        0x00: "Auto",
        0x01: "Indoor",
        0x02: "Outdoor",
        0x03: "One Push",
        0x04: "ATW",
        0x05: "Memory A",
        0x06: "Memory B",
        0x0A: "Preset",
    }.get(byte, f"0x{byte:02X}")


def packet_type_name(packet_type: Optional[int]) -> str:
    if packet_type is None:
        return "Raw VISCA"
    return {
        0x00: "VISCA-over-IP Command",
        0x01: "VISCA-over-IP Command",
        0x10: "VISCA-over-IP Inquiry",
        0x11: "VISCA-over-IP Reply",
    }.get(packet_type, f"VISCA-over-IP 0x{packet_type:02X}")


@dataclass
class DecodedCommand:
    name: str
    details: str
    valid: bool = True
    is_inquiry: bool = False
    is_controller_command: bool = False


def decode_reply(payload: bytes) -> DecodedCommand:
    if len(payload) < 2:
        return DecodedCommand("VISCA Reply", "Reply payload is too short", False)

    code = payload[1]
    socket_number = code & 0x0F
    if (code & 0xF0) == 0x40:
        return DecodedCommand("ACK", f"Socket {socket_number}")
    if (code & 0xF0) == 0x50:
        extra = payload[2:-1]
        detail = f"Socket {socket_number}"
        if extra:
            detail += f", data {bytes_to_hex(extra)}"
        return DecodedCommand("Completion", detail)
    if (code & 0xF0) == 0x60:
        error = payload[2] if len(payload) >= 4 else None
        error_name = {
            0x02: "Syntax error",
            0x03: "Command buffer full",
            0x04: "Command canceled",
            0x05: "No socket",
            0x41: "Command not executable",
        }.get(error, f"0x{error:02X}" if error is not None else "n/a")
        return DecodedCommand("Error", f"Socket {socket_number}, {error_name}", False)
    return DecodedCommand("VISCA Reply", f"Code 0x{code:02X}")


def decode_visca_command(payload: bytes) -> DecodedCommand:
    if not payload:
        return DecodedCommand("No VISCA payload", "Empty payload", False)
    if payload[-1] != 0xFF:
        return DecodedCommand("Malformed VISCA", "Missing 0xFF terminator", False)

    first_nibble = payload[0] & 0xF0
    if first_nibble == 0x90:
        return decode_reply(payload)
    if first_nibble != 0x80:
        return DecodedCommand("Unknown VISCA packet", f"Unexpected start byte 0x{payload[0]:02X}", False)

    if len(payload) < 4:
        return DecodedCommand("Malformed VISCA", "Controller packet is too short", False)

    op = payload[1]
    is_inquiry = op == 0x09
    is_command = op == 0x01
    prefix = "Inquiry" if is_inquiry else "Command"
    cat = payload[2]
    cmd = payload[3]
    p = payload

    def result(name: str, details: str = "") -> DecodedCommand:
        return DecodedCommand(
            name=f"{prefix}: {name}" if is_inquiry else name,
            details=details,
            is_inquiry=is_inquiry,
            is_controller_command=True,
        )

    if not (is_command or is_inquiry):
        return result("VISCA Operation", f"Operation 0x{op:02X}, category 0x{cat:02X}, command 0x{cmd:02X}")

    if cat == 0x00 and cmd == 0x01:
        return result("IF Clear")
    if cat == 0x00 and cmd == 0x02:
        return result("Camera Version")

    if cat == 0x04:
        if cmd == 0x00 and len(p) >= 6:
            return result("Power", on_off(p[4]))
        if cmd == 0x02 and len(p) >= 6:
            return result("Detail Level Step", up_down(p[4]))
        if cmd == 0x07 and len(p) >= 6:
            name, details = zoom_direction(p[4])
            return result(name, details)
        if cmd == 0x08 and len(p) >= 6:
            name, details = focus_direction(p[4])
            return result(name, details)
        if cmd == 0x0E and len(p) >= 6:
            return result("AE Level Step", up_down(p[4]))
        if cmd == 0x19:
            return result("Lens Initialize")
        if cmd == 0x33 and len(p) >= 6:
            return result("Backlight Compensation", on_off(p[4]))
        if cmd == 0x35 and len(p) >= 6:
            return result("White Balance Mode", wb_mode(p[4]))
        if cmd == 0x38 and len(p) >= 6:
            return result("Focus Mode", focus_mode(p[4]))
        if cmd == 0x3A and len(p) >= 6:
            return result("Spotlight Compensation", on_off(p[4]))
        if cmd == 0x3F and len(p) >= 7:
            slot = p[5] + 1
            return result("Preset", f"{preset_action(p[4])} slot {slot}")
        if cmd == 0x42 and len(p) >= 6:
            return result("Detail Level Direct", f"Level {p[4] & 0x0F}")
        if cmd == 0x47 and len(p) >= 9:
            return result("Zoom Direct", format_nibbles(p, 4, 4))
        if cmd == 0x48 and len(p) >= 9:
            return result("Focus Direct", format_nibbles(p, 4, 4))
        if cmd == 0x7D and len(p) >= 6:
            return result("Color Bar", on_off(p[4]))

    if cat == 0x05:
        if cmd == 0x03 and len(p) >= 9:
            direction = up_down(p[4])
            target = {0x01: "Preset white", 0x02: "Color temperature"}.get(p[5], f"Target 0x{p[5]:02X}")
            step = read_nibbles(p, 6, 2)
            return result(f"{target} Step", f"{direction}, step {step if step is not None else 'n/a'}")
        if cmd == 0x04 and len(p) >= 9:
            return result("Tint Step", f"{up_down(p[4])}, step {format_nibbles(p, 6, 2)}")
        if cmd == 0x10:
            return result("White Balance Set")
        if cmd == 0x34 and len(p) >= 6:
            return result("Auto Iris", on_off(p[4]))
        if cmd == 0x35 and len(p) >= 6:
            return result("Auto Shutter", on_off(p[4]))
        if cmd == 0x43 and len(p) >= 10:
            target = {0x01: "Preset white", 0x02: "Color temperature"}.get(p[4], f"Target 0x{p[4]:02X}")
            return result(f"{target} Direct", format_nibbles(p, 5, 4))
        if cmd == 0x44 and len(p) >= 10:
            return result("Tint Direct", format_nibbles(p, 5, 4))

    if cat == 0x06:
        if cmd == 0x01 and len(p) >= 9:
            direction = pan_tilt_direction(p[6], p[7])
            return result("Pan/Tilt Drive", f"{direction}, pan speed {p[4]}, tilt speed {p[5]}")
        if cmd == 0x02 and len(p) >= 17:
            pan = format_nibbles(p, 6, 5)
            tilt = format_nibbles(p, 11, 5)
            return result("Pan/Tilt Absolute", f"Pan {pan}, tilt {tilt}, speeds {p[4]}/{p[5]}")
        if cmd == 0x03 and len(p) >= 17:
            pan = format_nibbles(p, 6, 5)
            tilt = format_nibbles(p, 11, 5)
            return result("Pan/Tilt Relative", f"Pan {pan}, tilt {tilt}, speeds {p[4]}/{p[5]}")
        if cmd == 0x04:
            return result("Pan/Tilt Home")
        if cmd == 0x05:
            return result("Pan/Tilt Reset")
        if cmd == 0x06 and len(p) >= 6:
            return result("Menu", menu_mode(p[4]))
        if cmd == 0x07:
            return result("Pan/Tilt Limit", bytes_to_hex(p[4:-1]))
        if cmd == 0x12:
            return result("Pan Position")
        if cmd == 0x13:
            return result("Tilt Position")

    if cat == 0x7E and len(p) >= 6:
        group = p[3]
        ext = p[4]
        if group == 0x01:
            if ext == 0x0A:
                return result("Tally Red", on_off(p[5]))
            if ext == 0x53:
                return result("ND Preset", f"Preset {p[5]}")
            if ext == 0x54:
                mode = {0x00: "Auto", 0x04: "Manual"}.get(p[5], f"0x{p[5]:02X}")
                return result("Knee Mode", mode)
            if ext == 0x60:
                return result("Detail Setting", on_off(p[5]))
            if ext in (0x63, 0x64) and len(p) >= 9:
                channel = "R Gain" if ext == 0x63 else "B Gain"
                return result(f"{channel} Step", f"{up_down(p[5])}, memory {p[6]}, step {format_nibbles(p, 7, 2)}")
            if ext in (0x65, 0x66) and len(p) >= 9:
                channel = "R Black" if ext == 0x65 else "B Black"
                return result(f"{channel} Step", f"{up_down(p[5])}, step {format_nibbles(p, 7, 2)}")
            if ext == 0x6D:
                return result("Knee Setting", on_off(p[5]))
            if ext == 0x75:
                return result("AGC", on_off(p[5]))
        if group == 0x04:
            if ext == 0x12:
                return result("ND Variable Step", up_down(p[5]))
            if ext == 0x17 and len(p) >= 11:
                direction = {0x02: "Tele", 0x03: "Wide"}.get(p[5], f"0x{p[5]:02X}")
                return result("Zoom High Resolution", f"{direction}, speed {format_nibbles(p, 6, 4)}")
            if ext == 0x1A and len(p) >= 8:
                return result("Tally Green", on_off(p[7]))
            if ext == 0x1D:
                return result("Recording", press_release(p[5]))
            if ext == 0x20:
                return result("PTZ Trace", bytes_to_hex(p[5:-1]))
            if ext == 0x41 and len(p) >= 9:
                return result("Multi Function Dial", f"Target {p[5]}, step {format_nibbles(p, 7, 2)}")
            if ext == 0x42 and len(p) >= 9:
                return result("ND Variable Direct", format_nibbles(p, 7, 2))
            if ext in (0x43, 0x44) and len(p) >= 10:
                channel = "R Black" if ext == 0x43 else "B Black"
                return result(f"{channel} Direct", format_nibbles(p, 6, 4))
            if ext == 0x46 and len(p) >= 10:
                return result("R Gain Direct", format_nibbles(p, 6, 4))
            if ext == 0x4B and len(p) >= 9:
                return result("Iris Step", f"{up_down(p[5])}, step {format_nibbles(p, 6, 2)}")
            if ext == 0x52:
                mode = {0x00: "Preset", 0x01: "Variable"}.get(p[5], f"0x{p[5]:02X}")
                return result("ND Filter Mode", mode)
            if ext == 0x53:
                return result("Auto ND Filter", on_off(p[5]))
            if ext == 0x54:
                state = {0x02: "Filtered", 0x03: "Clear"}.get(p[5], f"0x{p[5]:02X}")
                return result("ND Clear", state)
            if ext == 0x56 and len(p) >= 10:
                return result("B Gain Direct", format_nibbles(p, 6, 4))
            if ext == 0x58:
                return result("Push AF/MF", press_release(p[5]))
            if ext == 0x60 and len(p) >= 7:
                mode = "Auto" if p[6] == 0x01 else "Manual"
                return result("Audio Level Control", f"Channel {p[5]}, {mode}")
            if ext == 0x61 and len(p) >= 9:
                return result("Audio Input Level Direct", format_nibbles(p, 6, 2))
            if ext == 0x62 and len(p) >= 9:
                return result("Audio Input Level Step", f"{up_down(p[5])}, step {format_nibbles(p, 7, 2)}")
            if ext in (0x72, 0x73, 0x74, 0x75):
                names = {
                    0x72: "Direct Menu",
                    0x73: "Assignable Button",
                    0x74: "Multi Function Dial Set",
                    0x75: "Display",
                }
                return result(names[ext], bytes_to_hex(p[5:-1]))

    return result("Unknown VISCA", f"Category 0x{cat:02X}, command 0x{cmd:02X}, data {bytes_to_hex(p[4:-1])}")


@dataclass
class ParsedPacket:
    payload: bytes
    wire: bytes
    wrapped: bool
    sequence: Optional[int]
    packet_type: Optional[int]
    packet_type_label: str
    payload_offset: Optional[int] = None
    consumed_length: Optional[int] = None


def parse_visca_ip_envelope(data: bytes) -> Optional[ParsedPacket]:
    if len(data) < 8 or data[0] != 0x01:
        return None

    packet_type = data[1]
    length_be = (data[2] << 8) | data[3]
    length_le = data[2] | (data[3] << 8)
    length = length_be

    def fits(candidate: int) -> bool:
        return candidate <= MAX_VISCA_IP_PAYLOAD and len(data) >= 8 + candidate

    if not fits(length):
        if fits(length_le):
            length = length_le
        else:
            return None

    sequence = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]
    payload = data[8 : 8 + length]
    wire = data[: 8 + length]
    return ParsedPacket(
        payload=payload,
        wire=wire,
        wrapped=True,
        sequence=sequence,
        packet_type=packet_type,
        packet_type_label=packet_type_name(packet_type),
        payload_offset=8,
        consumed_length=8 + length,
    )


def extract_raw_visca_payload(data: bytes) -> Optional[ParsedPacket]:
    start = None
    end = None
    for index, byte in enumerate(data):
        high = byte & 0xF0
        if start is None and high in (0x80, 0x90):
            start = index
        if start is not None and byte == 0xFF:
            end = index
            break

    if start is None or end is None or end <= start:
        return None

    payload = data[start : end + 1]
    return ParsedPacket(
        payload=payload,
        wire=payload,
        wrapped=False,
        sequence=None,
        packet_type=None,
        packet_type_label=packet_type_name(None),
        payload_offset=start,
        consumed_length=end + 1,
    )


def parse_udp_datagram(data: bytes) -> Optional[ParsedPacket]:
    return parse_visca_ip_envelope(data) or extract_raw_visca_payload(data)


class TcpStreamParser:
    def __init__(self) -> None:
        self.buffer = bytearray()

    def feed(self, data: bytes) -> list[ParsedPacket]:
        self.buffer.extend(data)
        packets: list[ParsedPacket] = []

        while self.buffer:
            packet = parse_visca_ip_envelope(bytes(self.buffer))
            if packet is not None:
                del self.buffer[: packet.consumed_length or 0]
                packets.append(packet)
                continue

            if len(self.buffer) >= 8 and self.buffer[0] == 0x01:
                length_be = (self.buffer[2] << 8) | self.buffer[3]
                length_le = self.buffer[2] | (self.buffer[3] << 8)
                plausible = (
                    length_be <= MAX_VISCA_IP_PAYLOAD
                    or length_le <= MAX_VISCA_IP_PAYLOAD
                    or self.buffer[1] in (0x00, 0x01, 0x10, 0x11)
                )
                if plausible:
                    break

            packet = extract_raw_visca_payload(bytes(self.buffer))
            if packet is not None:
                del self.buffer[: packet.consumed_length or 0]
                packets.append(packet)
                continue

            next_candidates = [
                index
                for index, byte in enumerate(self.buffer[1:], start=1)
                if byte == 0x01 or (byte & 0xF0) in (0x80, 0x90)
            ]
            if next_candidates:
                del self.buffer[: next_candidates[0]]
            else:
                if len(self.buffer) > MAX_VISCA_IP_PAYLOAD:
                    self.buffer.clear()
                break

        return packets


def build_visca_ip_envelope(sequence: int, visca_payload: bytes, packet_type: int = 0x11) -> bytes:
    length = len(visca_payload)
    return bytes(
        [
            0x01,
            packet_type,
            (length >> 8) & 0xFF,
            length & 0xFF,
            (sequence >> 24) & 0xFF,
            (sequence >> 16) & 0xFF,
            (sequence >> 8) & 0xFF,
            sequence & 0xFF,
        ]
    ) + visca_payload


def make_completion_for_inquiry(inquiry: bytes) -> bytes:
    if len(inquiry) < 5:
        return bytes([0x90, 0x51, 0xFF])

    category = inquiry[2]
    command = inquiry[3]

    if category == 0x00 and command == 0x02:
        return bytes([0x90, 0x51, 0x00, 0x02, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF])
    if category in (0x04, 0x05, 0x06, 0x7E):
        return bytes([0x90, 0x51, 0x00, 0xFF])
    return bytes([0x90, 0x51, 0xFF])


def make_reply_packets(payload: bytes, sequence: Optional[int], wrapped: bool) -> list[bytes]:
    if not payload or (payload[0] & 0xF0) != 0x80:
        return []

    ack = bytes([0x90, 0x41, 0xFF])
    completion = make_completion_for_inquiry(payload) if len(payload) >= 2 and payload[1] == 0x09 else bytes([0x90, 0x51, 0xFF])
    replies = [ack, completion]

    if wrapped:
        seq = sequence or 0
        return [build_visca_ip_envelope(seq, reply) for reply in replies]
    return replies


@dataclass
class CaptureEvent:
    timestamp: str
    epoch: float
    transport: str
    source: str
    wrapped: bool
    sequence: str
    packet_type: str
    command: str
    details: str
    payload_hex: str
    raw_hex: str
    valid: bool

    @classmethod
    def from_packet(cls, transport: str, source: str, raw: bytes, parsed: Optional[ParsedPacket]) -> "CaptureEvent":
        now = time.time()
        timestamp = datetime.fromtimestamp(now).strftime("%H:%M:%S.%f")[:-3]

        if parsed is None:
            return cls(
                timestamp=timestamp,
                epoch=now,
                transport=transport,
                source=source,
                wrapped=False,
                sequence="",
                packet_type="Unrecognized",
                command="Unrecognized data",
                details="No VISCA payload or VISCA-over-IP envelope was found.",
                payload_hex="",
                raw_hex=bytes_to_hex(raw),
                valid=False,
            )

        decoded = decode_visca_command(parsed.payload)
        sequence = "" if parsed.sequence is None else str(parsed.sequence)
        return cls(
            timestamp=timestamp,
            epoch=now,
            transport=transport,
            source=source,
            wrapped=parsed.wrapped,
            sequence=sequence,
            packet_type=parsed.packet_type_label,
            command=decoded.name,
            details=decoded.details,
            payload_hex=bytes_to_hex(parsed.payload),
            raw_hex=bytes_to_hex(raw),
            valid=decoded.valid,
        )


QueueCallback = Callable[[tuple[str, object]], None]


class ViscaCaptureServer:
    def __init__(
        self,
        bind_address: str,
        port: int,
        transport: str,
        send_replies: bool,
        callback: QueueCallback,
    ) -> None:
        self.bind_address = bind_address
        self.port = port
        self.transport = transport
        self.send_replies = send_replies
        self.callback = callback
        self.stop_event = threading.Event()
        self.threads: list[threading.Thread] = []
        self.sockets: list[socket.socket] = []
        self.sockets_lock = threading.Lock()

    def start(self) -> None:
        self.stop_event.clear()
        if self.transport in ("UDP", "Both"):
            self._start_thread("UDP listener", self._run_udp)
        if self.transport in ("TCP", "Both"):
            self._start_thread("TCP listener", self._run_tcp)

    def stop(self) -> None:
        self.stop_event.set()
        with self.sockets_lock:
            sockets = list(self.sockets)
            self.sockets.clear()
        for sock in sockets:
            try:
                sock.close()
            except OSError:
                pass
        for thread in list(self.threads):
            if thread.is_alive():
                thread.join(timeout=0.7)
        self.threads.clear()

    def _start_thread(self, name: str, target: Callable[[], None]) -> None:
        thread = threading.Thread(target=target, name=f"VISCA {name}", daemon=True)
        self.threads.append(thread)
        thread.start()

    def _remember_socket(self, sock: socket.socket) -> None:
        with self.sockets_lock:
            self.sockets.append(sock)

    def _forget_socket(self, sock: socket.socket) -> None:
        with self.sockets_lock:
            if sock in self.sockets:
                self.sockets.remove(sock)

    def _send_status(self, message: str) -> None:
        self.callback(("status", message))

    def _send_error(self, message: str) -> None:
        self.callback(("error", message))

    def _send_event(self, event: CaptureEvent) -> None:
        self.callback(("event", event))

    def _run_udp(self) -> None:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._remember_socket(sock)
        try:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind((self.bind_address, self.port))
            sock.settimeout(0.2)
            self._send_status(f"UDP listening on {self.bind_address}:{self.port}")

            while not self.stop_event.is_set():
                try:
                    data, address = sock.recvfrom(MAX_DATAGRAM_SIZE)
                except socket.timeout:
                    continue
                except OSError:
                    break

                parsed = parse_udp_datagram(data)
                event = CaptureEvent.from_packet("UDP", f"{address[0]}:{address[1]}", data, parsed)
                self._send_event(event)

                if self.send_replies and parsed is not None:
                    for reply in make_reply_packets(parsed.payload, parsed.sequence, parsed.wrapped):
                        try:
                            sock.sendto(reply, address)
                        except OSError as exc:
                            self._send_error(f"UDP reply failed: {exc}")
                            break
        except OSError as exc:
            self._send_error(f"UDP bind/listen failed on {self.bind_address}:{self.port}: {exc}")
        finally:
            self._forget_socket(sock)
            try:
                sock.close()
            except OSError:
                pass

    def _run_tcp(self) -> None:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._remember_socket(sock)
        try:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind((self.bind_address, self.port))
            sock.listen(8)
            sock.settimeout(0.2)
            self._send_status(f"TCP listening on {self.bind_address}:{self.port}")

            while not self.stop_event.is_set():
                try:
                    client, address = sock.accept()
                except socket.timeout:
                    continue
                except OSError:
                    break
                self._remember_socket(client)
                thread = threading.Thread(
                    target=self._run_tcp_client,
                    args=(client, address),
                    name=f"VISCA TCP {address[0]}:{address[1]}",
                    daemon=True,
                )
                self.threads.append(thread)
                thread.start()
        except OSError as exc:
            self._send_error(f"TCP bind/listen failed on {self.bind_address}:{self.port}: {exc}")
        finally:
            self._forget_socket(sock)
            try:
                sock.close()
            except OSError:
                pass

    def _run_tcp_client(self, client: socket.socket, address: tuple[str, int]) -> None:
        source = f"{address[0]}:{address[1]}"
        parser = TcpStreamParser()
        self._send_status(f"TCP client connected: {source}")
        try:
            client.settimeout(0.2)
            while not self.stop_event.is_set():
                try:
                    data = client.recv(4096)
                except socket.timeout:
                    continue
                except OSError:
                    break
                if not data:
                    break

                for parsed in parser.feed(data):
                    event = CaptureEvent.from_packet("TCP", source, parsed.wire, parsed)
                    self._send_event(event)
                    if self.send_replies:
                        for reply in make_reply_packets(parsed.payload, parsed.sequence, parsed.wrapped):
                            try:
                                client.sendall(reply)
                            except OSError as exc:
                                self._send_error(f"TCP reply failed to {source}: {exc}")
                                return
        finally:
            self._forget_socket(client)
            try:
                client.close()
            except OSError:
                pass
            self._send_status(f"TCP client disconnected: {source}")


class ViscaCaptureApp:
    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root.title("VISCA-over-IP Command Capture")
        self.root.geometry("1180x720")
        self.events: list[CaptureEvent] = []
        self.queue: queue.Queue[tuple[str, object]] = queue.Queue()
        self.server: Optional[ViscaCaptureServer] = None

        self.bind_var = tk.StringVar(value="0.0.0.0")
        self.port_var = tk.StringVar(value=str(DEFAULT_PORT))
        self.transport_var = tk.StringVar(value="UDP")
        self.replies_var = tk.BooleanVar(value=False)
        self.status_var = tk.StringVar(value="Idle")
        self.selected_details_var = tk.StringVar(value="")

        self._build_ui()
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)
        self.root.after(100, self._poll_queue)

    def _build_ui(self) -> None:
        root = self.root
        root.columnconfigure(0, weight=1)
        root.rowconfigure(1, weight=1)

        toolbar = ttk.Frame(root, padding=(10, 8))
        toolbar.grid(row=0, column=0, sticky="ew")
        toolbar.columnconfigure(12, weight=1)

        ttk.Label(toolbar, text="Bind").grid(row=0, column=0, padx=(0, 4))
        ttk.Entry(toolbar, textvariable=self.bind_var, width=16).grid(row=0, column=1, padx=(0, 10))

        ttk.Label(toolbar, text="Port").grid(row=0, column=2, padx=(0, 4))
        ttk.Entry(toolbar, textvariable=self.port_var, width=8).grid(row=0, column=3, padx=(0, 10))

        ttk.Label(toolbar, text="Transport").grid(row=0, column=4, padx=(0, 4))
        transport = ttk.Combobox(toolbar, textvariable=self.transport_var, values=("UDP", "TCP", "Both"), width=8, state="readonly")
        transport.grid(row=0, column=5, padx=(0, 10))

        ttk.Checkbutton(toolbar, text="Send ACK/completion replies", variable=self.replies_var).grid(row=0, column=6, padx=(0, 12))

        self.start_button = ttk.Button(toolbar, text="Start", command=self.start_capture)
        self.start_button.grid(row=0, column=7, padx=(0, 6))
        self.stop_button = ttk.Button(toolbar, text="Stop", command=self.stop_capture, state="disabled")
        self.stop_button.grid(row=0, column=8, padx=(0, 6))
        ttk.Button(toolbar, text="Clear", command=self.clear_events).grid(row=0, column=9, padx=(0, 6))
        ttk.Button(toolbar, text="Save CSV", command=self.save_csv).grid(row=0, column=10, padx=(0, 6))
        ttk.Button(toolbar, text="Save JSONL", command=self.save_jsonl).grid(row=0, column=11, padx=(0, 6))

        ttk.Label(toolbar, textvariable=self.status_var, anchor="e").grid(row=0, column=12, sticky="ew")

        paned = ttk.PanedWindow(root, orient="vertical")
        paned.grid(row=1, column=0, sticky="nsew", padx=10, pady=(0, 10))

        table_frame = ttk.Frame(paned)
        table_frame.columnconfigure(0, weight=1)
        table_frame.rowconfigure(0, weight=1)
        paned.add(table_frame, weight=4)

        columns = ("time", "transport", "source", "seq", "type", "command", "details", "payload")
        self.tree = ttk.Treeview(table_frame, columns=columns, show="headings", selectmode="browse")
        self.tree.heading("time", text="Time")
        self.tree.heading("transport", text="Transport")
        self.tree.heading("source", text="Source")
        self.tree.heading("seq", text="Seq")
        self.tree.heading("type", text="Type")
        self.tree.heading("command", text="Command")
        self.tree.heading("details", text="Details")
        self.tree.heading("payload", text="Payload")

        self.tree.column("time", width=105, stretch=False)
        self.tree.column("transport", width=78, stretch=False)
        self.tree.column("source", width=145, stretch=False)
        self.tree.column("seq", width=70, stretch=False)
        self.tree.column("type", width=160, stretch=False)
        self.tree.column("command", width=220, stretch=True)
        self.tree.column("details", width=260, stretch=True)
        self.tree.column("payload", width=320, stretch=True)
        self.tree.grid(row=0, column=0, sticky="nsew")
        self.tree.bind("<<TreeviewSelect>>", self._on_select)

        scrollbar_y = ttk.Scrollbar(table_frame, orient="vertical", command=self.tree.yview)
        scrollbar_y.grid(row=0, column=1, sticky="ns")
        self.tree.configure(yscrollcommand=scrollbar_y.set)

        detail_frame = ttk.Frame(paned, padding=(0, 8, 0, 0))
        detail_frame.columnconfigure(0, weight=1)
        detail_frame.rowconfigure(1, weight=1)
        paned.add(detail_frame, weight=1)

        ttk.Label(detail_frame, text="Selected Packet").grid(row=0, column=0, sticky="w")
        self.detail_text = tk.Text(detail_frame, height=8, wrap="word", font=("Consolas", 10))
        self.detail_text.grid(row=1, column=0, sticky="nsew")
        self.detail_text.configure(state="disabled")

    def start_capture(self) -> None:
        if self.server is not None:
            return

        bind_address = self.bind_var.get().strip() or "0.0.0.0"
        try:
            port = int(self.port_var.get().strip())
            if not 1 <= port <= 65535:
                raise ValueError
        except ValueError:
            messagebox.showerror("Invalid Port", "Port must be an integer from 1 to 65535.")
            return

        transport = self.transport_var.get()
        self.server = ViscaCaptureServer(
            bind_address=bind_address,
            port=port,
            transport=transport,
            send_replies=self.replies_var.get(),
            callback=self.queue.put,
        )
        self.server.start()
        self.status_var.set(f"Starting {transport} capture on {bind_address}:{port}")
        self.start_button.configure(state="disabled")
        self.stop_button.configure(state="normal")

    def stop_capture(self) -> None:
        if self.server is None:
            return
        server = self.server
        self.server = None
        server.stop()
        self.status_var.set("Stopped")
        self.start_button.configure(state="normal")
        self.stop_button.configure(state="disabled")

    def clear_events(self) -> None:
        self.events.clear()
        for item in self.tree.get_children():
            self.tree.delete(item)
        self._set_detail_text("")

    def save_csv(self) -> None:
        if not self.events:
            messagebox.showinfo("No Packets", "No captured packets to save.")
            return
        path = filedialog.asksaveasfilename(
            title="Save VISCA Capture CSV",
            defaultextension=".csv",
            filetypes=(("CSV files", "*.csv"), ("All files", "*.*")),
            initialfile=f"visca_capture_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv",
        )
        if not path:
            return
        with open(path, "w", newline="", encoding="utf-8") as handle:
            writer = csv.DictWriter(handle, fieldnames=list(asdict(self.events[0]).keys()))
            writer.writeheader()
            for event in self.events:
                writer.writerow(asdict(event))
        self.status_var.set(f"Saved CSV: {path}")

    def save_jsonl(self) -> None:
        if not self.events:
            messagebox.showinfo("No Packets", "No captured packets to save.")
            return
        path = filedialog.asksaveasfilename(
            title="Save VISCA Capture JSONL",
            defaultextension=".jsonl",
            filetypes=(("JSON Lines files", "*.jsonl"), ("All files", "*.*")),
            initialfile=f"visca_capture_{datetime.now().strftime('%Y%m%d_%H%M%S')}.jsonl",
        )
        if not path:
            return
        with open(path, "w", encoding="utf-8") as handle:
            for event in self.events:
                handle.write(json.dumps(asdict(event), separators=(",", ":")) + "\n")
        self.status_var.set(f"Saved JSONL: {path}")

    def _poll_queue(self) -> None:
        while True:
            try:
                kind, payload = self.queue.get_nowait()
            except queue.Empty:
                break
            if kind == "event":
                self._add_event(payload)  # type: ignore[arg-type]
            elif kind == "status":
                self.status_var.set(str(payload))
            elif kind == "error":
                self.status_var.set(str(payload))
                self.start_button.configure(state="normal")
                self.stop_button.configure(state="normal" if self.server is not None else "disabled")
        self.root.after(100, self._poll_queue)

    def _add_event(self, event: CaptureEvent) -> None:
        self.events.append(event)
        item = self.tree.insert(
            "",
            "end",
            values=(
                event.timestamp,
                event.transport,
                event.source,
                event.sequence,
                event.packet_type,
                event.command,
                event.details,
                event.payload_hex,
            ),
        )
        self.tree.selection_set(item)
        self.tree.see(item)

    def _on_select(self, _event: object) -> None:
        selection = self.tree.selection()
        if not selection:
            self._set_detail_text("")
            return
        index = self.tree.index(selection[0])
        if not 0 <= index < len(self.events):
            return
        event = self.events[index]
        lines = [
            f"Time: {event.timestamp}",
            f"Transport: {event.transport}",
            f"Source: {event.source}",
            f"Wrapped: {event.wrapped}",
            f"Sequence: {event.sequence or 'n/a'}",
            f"Packet type: {event.packet_type}",
            f"Command: {event.command}",
            f"Details: {event.details}",
            f"Valid: {event.valid}",
            "",
            f"VISCA payload: {event.payload_hex or 'n/a'}",
            f"Raw packet: {event.raw_hex or 'n/a'}",
        ]
        self._set_detail_text("\n".join(lines))

    def _set_detail_text(self, text: str) -> None:
        self.detail_text.configure(state="normal")
        self.detail_text.delete("1.0", "end")
        self.detail_text.insert("1.0", text)
        self.detail_text.configure(state="disabled")

    def on_close(self) -> None:
        if self.server is not None:
            self.stop_capture()
        self.root.destroy()


def run_gui() -> int:
    if tk is None or ttk is None:
        raise RuntimeError("Tkinter is not available in this Python installation.")
    root = tk.Tk()
    ViscaCaptureApp(root)
    root.mainloop()
    return 0


def run_console(bind: str, port: int, transport: str, send_replies: bool) -> int:
    event_queue: queue.Queue[tuple[str, object]] = queue.Queue()
    server = ViscaCaptureServer(bind, port, transport, send_replies, event_queue.put)
    server.start()
    print(f"Capturing {transport} VISCA on {bind}:{port}. Press Ctrl+C to stop.")
    try:
        while True:
            kind, payload = event_queue.get()
            if kind == "event":
                event = payload
                assert isinstance(event, CaptureEvent)
                print(
                    f"{event.timestamp} {event.transport} {event.source} "
                    f"seq={event.sequence or '-'} {event.command} | {event.details} | {event.payload_hex}"
                )
            else:
                print(f"{kind.upper()}: {payload}")
    except KeyboardInterrupt:
        print("\nStopping capture.")
    finally:
        server.stop()
    return 0


def run_self_test() -> int:
    samples = [
        bytes.fromhex("81 01 04 07 22 FF"),
        bytes.fromhex("81 01 06 01 18 18 02 01 FF"),
        bytes.fromhex("01 00 00 06 00 00 00 2A 81 01 04 07 00 FF"),
        bytes.fromhex("81 09 00 02 FF"),
    ]
    for sample in samples:
        parsed = parse_udp_datagram(sample)
        if parsed is None:
            raise AssertionError(f"Failed to parse sample {bytes_to_hex(sample)}")
        decoded = decode_visca_command(parsed.payload)
        if not decoded.valid:
            raise AssertionError(f"Failed to decode sample {bytes_to_hex(sample)}")
    parser = TcpStreamParser()
    combined = samples[0] + samples[2]
    parsed_packets = parser.feed(combined)
    if len(parsed_packets) != 2:
        raise AssertionError(f"TCP stream parser returned {len(parsed_packets)} packets, expected 2")
    print("Self-test passed.")
    return 0


def parse_args(argv: Optional[Iterable[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Capture VISCA-over-IP commands from a control panel.")
    parser.add_argument("--console", action="store_true", help="Run without the GUI and print captured packets.")
    parser.add_argument("--bind", default="0.0.0.0", help="Local IPv4 address to bind. Default: 0.0.0.0")
    parser.add_argument("--port", type=int, default=DEFAULT_PORT, help=f"Listen port. Default: {DEFAULT_PORT}")
    parser.add_argument("--transport", choices=("UDP", "TCP", "Both"), default="UDP", help="Transport to listen on.")
    parser.add_argument("--send-replies", action="store_true", help="Send VISCA ACK/completion replies to the controller.")
    parser.add_argument("--self-test", action="store_true", help="Run decoder/parser self-tests and exit.")
    return parser.parse_args(argv)


def main(argv: Optional[Iterable[str]] = None) -> int:
    args = parse_args(argv)
    if args.self_test:
        return run_self_test()
    if args.console:
        return run_console(args.bind, args.port, args.transport, args.send_replies)
    return run_gui()


if __name__ == "__main__":
    raise SystemExit(main())
