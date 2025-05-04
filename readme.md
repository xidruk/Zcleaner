# Zcleaner

A lightweight, configurable directory cleaning utility for Linux systems

## About

Zcleaner is a command-line utility that helps you maintain a clean filesystem by automatically removing unnecessary files from specified directories. With built-in safeguards and detailed statistics, it's designed to be both powerful and safe for regular use.

## Features

-  Clean multiple directories in a single operation
-  Detailed cleaning statistics (before/after size, freed space)
-  Home directory usage tracking
-  Protection against critical system directories
-  Configuration management via CLI
-  Path expansion support (~ for home directory)

## Installation

### Prerequisites
- gcc compiler
- make (build system)

### Installation 
```
git clone [https://github.com/xidruk/Zcleaner.git](https://github.com/xidruk/zcleaner.git)
cd Zcleaner
make
```
## Usage

### Clean cache directories 
run :
```
./zclean
```
### Add new cache directories to the conf list 
run :
```
./zclean --add "path"
```
### Drop directory from the conf list 
run :
```
./zcleaner --drop "path"
```
