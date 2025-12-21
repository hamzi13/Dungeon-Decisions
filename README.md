# 🏰 Dungeon Decisions

![Language](https://img.shields.io/badge/Language-C++-blue.svg)
![Status](https://img.shields.io/badge/Status-Completed-success.svg)
![Course](https://img.shields.io/badge/Course-CS101-orange.svg)

> **"Pain is temporary, glory is forever!"**

**Dungeon Decisions** is a  text-based RPG developed in C++. It features a dynamic combat system, a high-stakes economy, and a brutal **Permadeath** mechanic that deletes your save file if you die.

---

## 🎮 Game Features

### 💀 Hardcore Permadeath
If your HP hits 0, the game is **over**. Your save file is permanently deleted from the system. No second chances.

### 🔐 Encrypted Save System (Anti-Cheat)
The game saves your progress (Gold, Stats, Level) to a local file.
* **Encryption:** Data is obfuscated using mathematical keys.
* **Checksum Validation:** If you try to edit the file to give yourself 9999 Gold, the game detects the tampering and deletes the corrupted save.

### ✨ Recursive "Ancient Magic"
As you win battles, your magic damage scales exponentially. This is calculated using a **Recursive Function** (`recursiveMagic`) rather than a simple loop.

### ⚔️ Dynamic Enemy Scaling
The game gets harder the longer you survive.
* **Early Game:** Slimes & Goblins.
* **Mid Game:** Bandits & Orcs.
* **End Game:** The Red Dragon (Boss).

### ⛺ Risk/Reward Mechanics
* **The Campfire:** You can heal, but resting advances time, making enemies stronger.
* **The Shop:** Decide between buying Potions (Survival) or upgrading your Sword (Power).

---

## 🛠️ Technical Concepts Used
This project was built to demonstrate core C++ programming concepts:
* **Pointers:** Used for direct memory manipulation in combat and training functions.
* **Recursion:** Used for damage calculation algorithms.
* **File Handling (`fstream`):** For reading/writing saves and high scores.
* **Cross-Platform Compatibility:** Uses Preprocessor Directives (`#ifdef`) to ensure the game runs smoothly on both Windows and Linux/macOS.
* **Input Validation:** Robust error handling to prevent crashes from invalid user input.

---
