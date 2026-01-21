# Shell

## Summary

1. [Installation](#1-installation)
2. [Execution](#2-execution)
3. [Run the shell with Docker](#3-run-the-shell-with-docker)
4. [How to use](#4-how-to-use)
5. [Implemented features](#5-implemented-features)
6. [Custom commands](#6-custom-commands)
7. [TODO](#7-todo)

## 1. Installation

- Require `gcc`
- Require `libreadline-dev`(https://tiswww.case.edu/php/chet/readline/rltop.html)

    `sudo apt install libreadline-dev`

- Clone the repository

    `git clone https://gibson.telecomnancy.univ-lorraine.fr/projets/2526/Systeme/projet/Projet-systeme-groupe-30.git`

## 2. Execution

`make run`

## 3. Demo with Docker

- Require `docker`
- Pull the docker image
    `docker pull gibson-registry.telecomnancy.univ-lorraine.fr/projets/2526/systeme/projet/projet-systeme-groupe-30:latest`
- Clone the repository
    `git clone https://gibson.telecomnancy.univ-lorraine.fr/projets/2526/Systeme/projet/Projet-systeme-groupe-30.git`
- Access to the project folder
    `cd Projet-systeme-groupe-30/`
- Run the docker image
    `docker run -it -e USER=demo --mount type=bind,source="$(pwd)",target=/root gibson-registry.telecomnancy.univ-lorraine.fr/projets/2526/systeme/projet/projet-systeme-groupe-30:latest bash`
- Access to the repo
    `cd /root`
- And you can now use the shell :)

## 4. How to use

### Stop the shell

Enter `fin` or Ctrl+D

## 5. Implemented features

- Piping commands with `|`
- Redirections with `<`, `>` and `>>`
- Changing directory with `cd`
- Multi commands with `;`
- Chaining commands with conditions with `&&` and `||`
- Environnement variables ($HOME, ...)
- Shell infos (succes of last command, user name, path)
- Shell with command history and moving cursor
- Handle "", '', \  (no distinction between the two)
- Job control (bg, fg, &, stop processus with Ctrl+Z, jobs)

## 6. Custom commands

- `myps`
- `mypstree`
- `mynetstat`
- `myarp`
- `myenv -p <PID>`
- `myinfo`
- `mymaps -p <PID>`
- `mydump -p <PID> --start 0x... --end 0x... -o fichier`
- `mylof -p <PID>`

## 7. TODO
