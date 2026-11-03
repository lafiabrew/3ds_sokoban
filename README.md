# Sokoban 3DS (`3ds_sokoban`)

<p align="center">
  <img src="docs/icon.png" width="72" height="72" alt="3ds_sokoban icon" />
  &nbsp;&nbsp;&nbsp;
  <img src="docs/banner.png" width="256" height="128" alt="3ds_sokoban 3DS banner" />
</p>

<p align="center">
  <img src="docs/screenshot.png" width="360" alt="3ds_sokoban dual-screen 3DS preview" />
</p>

> **Warehouse Crate Puzzles (`CTR-P-SOKO`, UniqueId `0x0D3ED`)** — Part of the [**lafiabrew**](https://github.com/orgs/lafiabrew/repositories) Nintendo 3DS Homebrew Suite.

Classic warehouse box-pushing logic puzzle with 64-step undo stack, move/push counters, and multiple warehouses.

---

## Architecture & Data Flow

```mermaid
flowchart LR
    INPUT[D-Pad Move / Push] --> STEP[Wall & Crate Collision Check]
    STEP --> UNDO[64-Step Snapshot Undo Stack]
    STEP --> GOAL[Goal Tile Verification]
```

---

## Controls & Touch Ergonomics

| Button / Input | Action |
|---|---|
| `D-Pad` | Move worker and push crates onto green goal tiles |
| `B` | Undo last move/push (up to 64 steps) |
| `X` | Restart current warehouse |
| `L` / `R` | Switch warehouse level |

---

## Build, Verify & Install

```bash
make test
make 3ds && make verify-3ds
make qr
```
