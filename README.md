# Jacknoll-IP3-Mthrux
# MTHRUX - Linux User Session Cleaner

```
███╗░░░███╗████████╗██╗░░██╗██████╗░██╗░░░██╗██╗░░██╗
████╗░████║╚══██╔══╝██║░░██║██╔══██╗██║░░░██║╚██╗██╔╝
██╔████╔██║░░░██║░░░███████║██████╔╝██║░░░██║░╚███╔╝░
██║╚██╔╝██║░░░██║░░░██╔══██║██╔══██╗██║░░░██║░██╔██╗░
██║░╚═╝░██║░░░██║░░░██║░░██║██║░░██║╚██████╔╝██╔╝╚██╗
╚═╝░░░░░╚═╝░░░╚═╝░░░╚═╝░░╚═╝╚═╝░░╚═╝░╚═════╝░╚═╝░░╚═╝
```

## Overview
MTHRUX is a powerful Linux tool designed to clear user session temporary files from the `/home/username` directory. Over time, these temporary files can accumulate, making the system slower and consuming extra disk space. MTHRUX efficiently cleans these files based on configuration settings.

## Features
- Developed in **C** and **Shell Scripting**.
- Reads configuration from `mthrux.mtconf` to determine which directories to scan and clean.
- Provides multiple commands to manage and automate the cleaning process.
- Improves system performance by freeing up unnecessary space.

## Installation
```bash
git clone https://github.com/yourusername/mthrux.git
cd mthrux
make install  # If there's a Makefile
```

## Usage
### Basic Commands
- **Run MTHRUX manually:**
  ```bash
  mthrux --clean
  ```
- **Check configured directories:**
  ```bash
  mthrux --list
  ```
- **Add a new directory to monitor:**
  ```bash
  mthrux --add /path/to/directory
  ```
- **Remove a directory from monitoring:**
  ```bash
  mthrux --remove /path/to/directory
  ```
- **View current configuration:**
  ```bash
  cat ~/.config/mthrux.mtconf
  ```

## Configuration File: `mthrux.mtconf`
The file `mthrux.mtconf` contains a list of directories where MTHRUX should search for and delete temporary files. You can manually edit this file or use MTHRUX commands to modify it.

Example:
```
/home/user/.cache
/home/user/.local/share/Trash
/home/user/tmp
```

## Contributing
If you'd like to contribute to MTHRUX, feel free to fork the repository and submit a pull request!
