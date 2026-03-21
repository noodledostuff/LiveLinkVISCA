**Complete VISCA Command List (VISCA over IP – Software Version 2.00)**

All commands below use the standard VISCA over IP format.  

- Replace `x` with `1` (fixed for VISCA over IP).  
- Parameters (`0p`, `0q`, `pp`, etc.) are explained in the **Comments** column.  
- Full packet example: `81 01 7E 04 52 02 FF` (for ND FILTER MODE = Preset).

### 1. ND FILTER

| Command                | Packet                          | Comments                                       |
| ---------------------- | ------------------------------- | ---------------------------------------------- |
| MODE (PRESET/VARIABLE) | `8x 01 7E 04 52 0p FF`          | p: 0=Preset, 1=Variable                        |
| ND VARIABLE UP         | `8x 01 7E 04 12 02 FF`          | —                                              |
| ND VARIABLE DOWN       | `8x 01 7E 04 12 03 FF`          | —                                              |
| ND VARIABLE            | `8x 01 7E 04 42 00 00 0p 0p FF` | pp: 00 (1/4) … 14 (1/128)                      |
| AUTO ND FILTER         | `8x 01 7E 04 53 0p FF`          | p: 2=On, 3=Off                                 |
| ND CLEAR               | `8x 01 7E 04 54 0p FF`          | p: 2=Filtered, 3=Clear                         |
| ND PRESET              | `8x 01 7E 01 53 0p FF`          | p: 0=Clear, 1=Preset 1, 2=Preset 2, 3=Preset 3 |

### 2. IRIS

| Command   | Packet                       | Comments                            |
| --------- | ---------------------------- | ----------------------------------- |
| IRIS UP   | `8x 01 7E 04 4B 02 0p 0p FF` | pp: Step 01–FF (≈1/256 EV brighter) |
| IRIS DOWN | `8x 01 7E 04 4B 03 0p 0p FF` | pp: Step 01–FF (≈1/256 EV darker)   |
| AUTO IRIS | `8x 01 05 34 0p FF`          | p: 2=On, 3=Off                      |

### 3. GAIN

| Command | Packet                 | Comments       |
| ------- | ---------------------- | -------------- |
| AGC     | `8x 01 7E 01 75 0p FF` | p: 2=On, 3=Off |

### 4. SHUTTER

| Command      | Packet              | Comments       |
| ------------ | ------------------- | -------------- |
| AUTO SHUTTER | `8x 01 05 35 0p FF` | p: 2=On, 3=Off |

### 5. AUTO EXPOSURE

| Command                | Packet              | Comments       |
| ---------------------- | ------------------- | -------------- |
| AE LEVEL UP            | `8x 01 04 0E 02 FF` | —              |
| AE LEVEL DOWN          | `8x 01 04 0E 03 FF` | —              |
| BACKLIGHT COMPENSATION | `8x 01 04 33 0p FF` | p: 2=On, 3=Off |
| SPOTLIGHT COMPENSATION | `8x 01 04 3A 0p FF` | p: 2=On, 3=Off |

### 6. WHITE BALANCE

| Command                       | Packet                                  | Comments                                       |
| ----------------------------- | --------------------------------------- | ---------------------------------------------- |
| MODE                          | `8x 01 04 35 0p FF`                     | p: 4=ATW, 5=Memory A, A=Preset                 |
| WB SET                        | `8x 01 04 10 05 FF`                     | —                                              |
| PRESET WHITE UP               | `8x 01 05 03 02 0p 0q 0q FF`            | p:1=Preset, qq: Step 01–FF                     |
| PRESET WHITE DOWN             | `8x 01 05 03 03 0p 0q 0q FF`            | p:1=Preset, qq: Step 01–FF                     |
| PRESET WHITE DIRECT           | `8x 01 05 43 0p 0r 0r 0r 0r FF`         | p:1=Preset, rrrr: 07D0 (2000K) … 3A98 (15000K) |
| R GAIN UP                     | `8x 01 7E 01 63 02 0p 0q 0q FF`         | p:2=Memory A, qq: Step 01–FF                   |
| R GAIN DOWN                   | `8x 01 7E 01 63 03 0p 0q 0q FF`         | p:2=Memory A, qq: Step 01–FF                   |
| R GAIN DIRECT                 | `8x 01 7E 04 46 0p 0r 0r 0r 0r FF`      | p:2=Memory A, rrrr: 0000(–99) … 07BC(+99)      |
| B GAIN UP / DOWN / DIRECT     | (same structure as R GAIN, use 64 / 56) | —                                              |
| COLOR TEMP UP / DOWN / DIRECT | (same as PRESET WHITE but p=2)          | —                                              |
| TINT UP / DOWN / DIRECT       | (use 05 04 / 05 44)                     | —                                              |
| OFFSET COLOR TEMP / TINT      | (use 05 05 / 05 45 and 05 06 / 05 46)   | p=0=ATW                                        |

### 7. BLACK

| Command                          | Packet                                        | Comments |
| -------------------------------- | --------------------------------------------- | -------- |
| MASTER BLACK UP/DOWN/DIRECT      | `8x 01 05 18 …` / `8x 01 05 48 …`             | —        |
| R BLACK / B BLACK UP/DOWN/DIRECT | `8x 01 7E 01 65/66 …` / `8x 01 7E 04 43/44 …` | —        |

### 8. DETAIL / KNEE

| Command            | Packet                            | Comments            |
| ------------------ | --------------------------------- | ------------------- |
| DETAIL SETTING     | `8x 01 7E 01 60 0p FF`            | p: 2=On, 3=Off      |
| DETAIL LEVEL       | `8x 01 04 02 …` / `8x 01 04 42 …` | pp: 00(–7) … 0E(+7) |
| KNEE SETTING       | `8x 01 7E 01 6D 0p FF`            | p: 2=On, 3=Off      |
| KNEE MODE          | `8x 01 7E 01 54 0p FF`            | p: 0=Auto, 4=Manual |
| KNEE SLOPE / POINT | `8x 01 7E 01 6F/6E …`             | —                   |

### 9. ZOOM

| Command                   | Packet                                | Comments                      |
| ------------------------- | ------------------------------------- | ----------------------------- |
| STANDARD TELE / WIDE      | `8x 01 04 07 02/03 FF`                | —                             |
| VARIABLE SPEED TELE/WIDE  | `8x 01 04 07 2p/3p FF`                | p: 0(Slow)–7(Fast)            |
| STOP                      | `8x 01 04 07 00 FF`                   | —                             |
| HIGH RESOLUTION TELE/WIDE | `8x 01 7E 04 17 02/03 0p 0p 0p 0p FF` | p: 0000(Slow)–7FFE(Fast)      |
| DIRECT                    | `8x 01 04 47 0z 0z 0z 0z FF`          | zzzz: 0000(Wide) … 6000(Tele) |

### 10. FOCUS

| Command           | Packet                       | Comments                          |
| ----------------- | ---------------------------- | --------------------------------- |
| MODE              | `8x 01 04 38 pp FF`          | pp: 02=Auto, 03=Manual, 10=Toggle |
| STANDARD FAR/NEAR | `8x 01 04 08 02/03 FF`       | —                                 |
| VARIABLE FAR/NEAR | `8x 01 04 08 2p/3p FF`       | p: 0–7                            |
| STOP              | `8x 01 04 08 00 FF`          | —                                 |
| DIRECT            | `8x 01 04 48 0p 0p 0p 0p FF` | pppp: 0000(Far) … FFFF(Near)      |
| PUSH AF/PUSH MF   | `8x 01 7E 04 58 0p FF`       | p: 0=Release, 1=Press             |

### 11. RECORDING & AUDIO

| Command                    | Packet                    | Comments                         |
| -------------------------- | ------------------------- | -------------------------------- |
| RECORDING                  | `8x 01 7E 04 1D 0p FF`    | p: 0=Release, 1=Press            |
| AUDIO LEVEL CONTROL        | `8x 01 7E 04 60 0p 0q FF` | p:1=CH1/2=CH2, q:0=Manual/1=Auto |
| INPUT LEVEL Up/Down/Direct | `8x 01 7E 04 62/61 …`     | —                                |

### 12. PAN/TILT (most important)

| Command                | Packet                                               | Comments                               |
| ---------------------- | ---------------------------------------------------- | -------------------------------------- |
| Drive (all directions) | `8x 01 06 01 vv ww 03/01/02/03 FF`                   | vv=Pan speed, ww=Tilt speed (01–18/32) |
| Stop                   | `8x 01 06 01 vv ww 03 03 FF`                         | —                                      |
| ABS (Absolute)         | `8x 01 06 02 vv ww 0p 0p 0p 0p 0p 0t 0t 0t 0t 0t FF` | ppppp=Pan, ttttt=Tilt                  |
| REL (Relative)         | `8x 01 06 03 vv ww …`                                | —                                      |
| HOME / RESET           | `8x 01 06 04/05 FF`                                  | —                                      |
| LIMIT SET / CLEAR      | `8x 01 06 07 …`                                      | —                                      |

(Full drive directions: Up=03 01, Down=03 02, Left=01 03, Right=02 03, diagonals, etc.)

### 13. PRESET & PTZ TRACE

| Command                   | Packet                       | Comments                       |
| ------------------------- | ---------------------------- | ------------------------------ |
| PRESET SET/RESET/RECALL   | `8x 01 04 3F 01/00/02 pp FF` | pp = Preset number – 1 (00–63) |
| PTZ TRACE REC/PLAY/DELETE | `8x 01 7E 04 20 …`           | —                              |

### 14. POWER / MENU / TALLY / MISC

| Command           | Packet                            | Comments                   |
| ----------------- | --------------------------------- | -------------------------- |
| POWER             | `8x 01 04 00 0p FF`               | p: 2=On, 3=Standby         |
| MENU ON/OFF       | `8x 01 06 06 pp FF`               | pp: 2=On, 3=Off, 10=Toggle |
| TALLY RED / GREEN | `8x 01 7E 01 0A / 04 1A 00 0p FF` | p: 2=On, 3=Off             |
| COLOR BAR         | `8x 01 04 7D 0p FF`               | p: 2=On, 3=Off             |
| LENS INIT         | `8x 01 04 19 01 FF`               | —                          |

### Special Commands (Press/Release, Dial, Direct Menu)

- **Push AF/MF, Recording, Display, Assignable Button, Multi Selector, Direct Menu**: use `8x 01 7E 04 58/1D/75/73/40/72 pp 0q FF` (q=0=Release, 1=Press)
- **Multi Function Dial SET**: `8x 01 7E 04 74 0p 0q 0q FF`
- **Multi Function Dial CW/CCW**: `8x 01 7E 04 41 0p 0q 0q FF`

### Standard VISCA Device Commands

| Command        | Packet           | Comments             |
| -------------- | ---------------- | -------------------- |
| IF_Clear       | `8x 01 00 01 FF` | Clears buffer        |
| CAM_VersionInq | `8x 09 00 02 FF` | Returns version info |

**Inquiry commands** (replace 01 with 09) are on pages 20–22 of the PDF — same structure, just ask for current value.
