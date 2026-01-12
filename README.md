# Shell

## Summary

1. [Installation](#1-installation)
2. [Execution](#2-execution)
3. [How to use](#3-how-to-use)
4. [Implemented features](#4-implemented-features)
5. [Custom commands](#5-custom-commands)
6. [TODO](#6-todo)

## 1. Installation

- Require `gcc`
- Require `libreadline-dev`(https://tiswww.case.edu/php/chet/readline/rltop.html)

    `sudo apt install libreadline-dev`

- Clone the repository

    `git clone https://gibson.telecomnancy.univ-lorraine.fr/projets/2526/Systeme/projet/Projet-systeme-groupe-30.git`

## 2. Execution

`make run`

## 3. How to use

### Stop the shell

Enter `fin` or Ctrl+D

## 4. Implemented features

- Piping commands with `|`
- Redirections with `<`, `>` and `>>`
- Changing directory with `cd`
- Multi commands with `;`
- Chaining commands with conditions with `&&` and `||`
- Environnement variables ($HOME, ...)
- Shell infos (succes of last command, user name, path)
- Shell with command history and moving cursor
- Handle "", '', \  (no distinction between the two)

## 5. Custom commands

- `myps`
- `mypstree`
- `mynetstat`
- `myarp`
- `myenv -p <PID>`
- `myinfo`
- `mymaps -p <PID>`
- `mydump -p <PID> --start 0x... --end 0x... -o fichier`
- `mylof -p <PID>`

## 6. TODO

- Background processus with `&`
- Stop the processus with Ctrl+Z, Ctrl+C