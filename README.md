# MoiMoiOS

<h1 align="center">MoiMoiOS</h1>

<p align="center">
<pre>
 __  __       _ _       _   ___   ____  
|  \/  | ___ (_) |_ ___| | |__ \ / __ \ 
| |\/| |/ _ \| | __/ _ \ |    ) | |  | |
| |  | | (_) | | ||  __/ |   / /| |__| |
|_|  |_|\___/|_|\__\___|_|  |____\____/ 
</pre>
</p>

<p align="center">
<b>Modular Capability-Based Operating System</b><br>
<i>Built from Scratch.</i>
</
    p>

## What is MoiMoiOS?

**MoiMoiOS** is a modular, secure, lightweight operating system designed from the ground up.  
It centers around a **capability-based security model** where apps request privileges through the auditable **Chest** service.

Rather than giving blanket superuser access, MoiMoiOS empowers fine-grained, user-controlled privilege elevation.

---

## Features
- Lightweight custom microkernel
- Capability-based access control
- Secure Chest service for privileged actions
- Structured service traps instead of legacy syscalls
- Modular service-driven architecture
- Designed for both embedded and desktop environments

---

## Project Structure

kernel/ core/          --> Core kernel (init, trap dispatcher, scheduler) services/ chest/       --> Chest privilege service security/    --> Token and access control systems drivers/       --> Device and memory management

userspace/ chest_manager/ --> GUI/CLI for managing Chest access shell/         --> (planned) system shell

build/ Makefile       --> Kernel build instructions

docs/ design.md      --> OS design philosophy security.md    --> Chest security model

---

## Current Status
- [x] Kernel initialization
- [x] Trap system dispatcher
- [x] Chest service basic framework
- [ ] Memory manager (WIP)
- [ ] Process/task scheduler (planned enhancements)
- [ ] Shell and userspace apps
- [ ] Secure bootloader (future goal)

---

## License

MoiMoiOS is released under the **MIT License**.  
Feel free to fork, extend, and experiment — all contributions are welcome.

---

**Designed and Engineered with Precision**  
_The MoiMoiOS Team, 2025._

---
