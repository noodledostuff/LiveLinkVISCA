# Copyright (c) 2026 Timothy YU. All rights reserved.

import unittest
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
import visca_capture as capture


class ViscaCaptureTests(unittest.TestCase):
    def test_decode_zoom_variable(self):
        payload = bytes.fromhex("81 01 04 07 22 FF")
        decoded = capture.decode_visca_command(payload)
        self.assertTrue(decoded.valid)
        self.assertEqual(decoded.name, "Zoom Tele Variable")
        self.assertIn("Speed 2", decoded.details)

    def test_decode_pan_tilt_drive(self):
        payload = bytes.fromhex("81 01 06 01 18 18 02 01 FF")
        decoded = capture.decode_visca_command(payload)
        self.assertTrue(decoded.valid)
        self.assertEqual(decoded.name, "Pan/Tilt Drive")
        self.assertIn("Up Right", decoded.details)

    def test_parse_visca_ip_envelope(self):
        packet = bytes.fromhex("01 00 00 06 00 00 00 2A 81 01 04 07 00 FF")
        parsed = capture.parse_udp_datagram(packet)
        self.assertIsNotNone(parsed)
        self.assertTrue(parsed.wrapped)
        self.assertEqual(parsed.sequence, 42)
        self.assertEqual(parsed.payload, bytes.fromhex("81 01 04 07 00 FF"))

    def test_tcp_stream_parser_accepts_raw_and_wrapped_packets(self):
        raw = bytes.fromhex("81 01 04 07 22 FF")
        wrapped = bytes.fromhex("01 00 00 06 00 00 00 2A 81 01 04 07 00 FF")
        parser = capture.TcpStreamParser()
        packets = parser.feed(raw + wrapped)
        self.assertEqual(len(packets), 2)
        self.assertFalse(packets[0].wrapped)
        self.assertTrue(packets[1].wrapped)

    def test_reply_packets_wrap_when_input_was_wrapped(self):
        payload = bytes.fromhex("81 01 04 07 00 FF")
        replies = capture.make_reply_packets(payload, 42, True)
        self.assertEqual(len(replies), 2)
        self.assertEqual(replies[0][:8], bytes.fromhex("01 11 00 03 00 00 00 2A"))
        self.assertEqual(replies[0][8:], bytes.fromhex("90 41 FF"))


if __name__ == "__main__":
    unittest.main()
