# VSH

## Requirements
The shell uses C library functions like `sigdescr_np()` and `sigabbrev_np()` 
which are only available in `glibc` version 2.32 and above, can be found in 
Ubuntu 22.04 and recent versions of Arch

## Usage
- make
- ./vsh

## Assumptions
- Maximum length of a command can be `MAXLINE = 1000`
- Maximum number of arguments can be `MAXARGS = 1000`
- jobs are not ordered in alphabetical order
