# Windows Keylogger (Educational Purpose Only)

This is a Windows-based keylogger developed in C++. It captures and logs keystrokes at the system level using Windows Hooks. The project demonstrates how low-level keyboard events can be monitored using the Windows API.

You can checkout https://medium.com/@atharvphadatare78/part-1-learn-building-a-windows-keylogger-with-c-7ad04c1da8c9 ,To learn how to build your own keylogger. And I have explained concepts releted to the Keylogger in detail there.  

> ⚠️ **DISCLAIMER:** This tool is built strictly for educational purposes. Unauthorized use of a keylogger is illegal and unethical. Use it only in lab environments or with explicit permission.

---

## 🔧 Features

- ✅ Captures every key press using a low-level keyboard hook
- ✅ Supports alphanumeric and special characters
- ✅ Detects shift and caps lock to log proper case
- ✅ Writes logs to a hidden file silently
- ✅ Automatically captures locale keyboard layout

---

## 🛠️ Technologies Used

- **Language:** C++
- **Platform:** Windows OS
- **API:** Windows API (Win32)
- **Tools:** MinGW / Visual Studio Code

---

## 📌 How It Works

### 1. **Windows Hook Setup**
The program installs a low-level keyboard hook using `SetWindowsHookEx()` with the `WH_KEYBOARD_LL` flag.

### 2. **Hook Callback Function**
Each keystroke triggers `HookCallback()`, which retrieves the key code using a `KBDLLHOOKSTRUCT` structure.

### 3. **Keystroke Interpretation**
It converts virtual key codes into readable characters, accounting for `SHIFT` and `CAPS LOCK` to log the correct case.

### 4. **Log Storage**
Captured keystrokes are stored in a log file, which can be hidden for stealth operation.

---

## 🚀 Getting Started

### ✅ Prerequisites
- Windows OS
- MinGW compiler or Visual Studio Build Tools

### 🔧 Compile and Run

Using **g++** via MinGW:

```bash
g++ keylogger.cpp -o keylogger.exe -luser32
./keylogger.exe
```
⚠️ Make sure to run the executable with administrator privileges to hook globally.
