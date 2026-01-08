# KNOWN_GOOD_STATE.md
# ALSO SEE: CPUTEMP-Field-Notebook.txt

_Last verified working: Wed Jan 7, 2026_

This document captures the **exact system state and assumptions** under which
**Lab 3 (VS Code + Torizon Run/Debug)** works correctly.

If the project breaks in the future, use this file to quickly diagnose
what drifted.

---

## 1. Core Mental Model (Critical)

**This project uses a REMOTE Docker workflow.**

> VS Code (running in WSL) is a **Docker client only**.  
> The **only Docker daemon that matters is on the Toradex device**.

There is **no local Docker daemon or local registry involved** when things work.

VS Code (WSL:Torizon)
|
| docker CLI (DOCKER_HOST set)
v
Toradex device Docker daemon (tcp://192.168.8.173:2375)
|
├─ Local registry on device (:5002)
└─ Application containers

---

## 2. Device & Network (Known Good)

| Item | Value |
|----|----|
| Device IP | `192.168.8.173` |
| Host (PC) IP | `192.168.8.122` |
| SSH user | `torizon` |
| SSH port | `22` |
| Debug SSH port | `2230` |
| Architecture | `arm64` |
| Board | Verdin AM62 |
| GPU prefix | `-am62` |

---

## 3. Docker Configuration (This Is the Key)

### Docker Host
Docker **must** point to the device:

``
echo $DOCKER_HOST
# tcp://192.168.8.173:2375
Docker Info (from VS Code / WSL terminal)

docker info | grep -i "Operating System"
# Operating System: Torizon OS 7.4.0+build.28 (scarthgap)
If this says Docker Desktop or WSL, the lab will break.

Docker Context

docker context ls
Expected:

default * → tcp://192.168.8.173:2375

Warning about DOCKER_HOST overriding context is expected and correct

desktop-linux exists but must not be active

4. Registry (Runs on the Device)
The Docker registry is on the Toradex device, not on the PC.

Item	Value
Registry host	Toradex device
Registry port	5002
Registry image	registry:2
Access URL	http://localhost:5002/v2/

Because Docker is remote, localhost refers to the device.

Registry health check

curl http://localhost:5002/v2/
# {}
Port check

ss -ltn | grep 5002
# LISTEN 0.0.0.0:5002
5. Images (Expected State)
Images are stored on the device Docker daemon and may be tagged as:


localhost:5002/cputemp-debug:arm64
This means:

stored in the device registry

not running unless explicitly deployed

Example:


docker images | grep localhost:5002
6. Containers (When Debug Is Running)
When Run/Debug is active:


docker ps
Expected:

torizon-cputemp-debug-1   localhost:5002/cputemp-debug:arm64
This output must match both:

VS Code terminal

ssh torizon@192.168.8.173 docker ps

7. VS Code / Torizon Extension Expectations
VS Code shows: WSL:Torizon

Toradex extension version observed working: 3.2.6

Device may be set as Default (this is expected)

Extension will explicitly log:

Device 192.168.8.173 set as Docker Host
→ This is correct in the working model

8. “If It Breaks” – 60-Second Recovery Checklist
Run these in order from the VS Code WSL terminal:

1️⃣ Verify Docker is remote

docker info | grep "Operating System"
Must say Torizon OS.

2️⃣ Verify registry on device

curl http://localhost:5002/v2/
Must return {}.

3️⃣ Verify DOCKER_HOST

echo $DOCKER_HOST
Must be:

tcp://192.168.8.173:2375
4️⃣ If still broken
Reload VS Code window

Reboot Toradex device

Restart PC / WSL / Docker Desktop

Open project → Run/Debug again

A full reboot reliably restores the intended lab state.

9. Important Warnings / Gotchas
Do NOT assume local Docker is used for this lab — it is not.

Do NOT remove registry:2 while debugging.

Do NOT rely on Docker Desktop’s registry.

localhost:5002 always means the device, not the PC.

Aggressive docker prune -a can break extension assumptions.

Mixed Docker contexts cause misleading errors.

10. Summary (One Sentence)
If Docker commands from WSL show “Torizon OS” and curl localhost:5002/v2/ works, Run/Debug will work.

Anything else is drift.