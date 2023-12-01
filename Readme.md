# Shell

A simple shell implemented in cpp.

I've only implemented a limited subset of functionality:
- `cd` to change directory
- `|` to pipe output of one command to another
- `>` to redirect output of one command to a file
- `<` to redirect input of one command to a file
- `>>` to append output of one command to a file
- `&` to run a command in the background. (`fg` to bring it back to the foreground not yet implmented)
- `~` expands to $HOME
- `ctrl-c` sends SIGINT to the child processes

Notable omissions:
- `;` to run multiple commands in sequence
- `&&` and `||` to run commands conditionally
- globbing
- `*` and `?` expansion
- quotes and escaping (everything is assumed to be whitespace separated)


Quit with `^D`.


To compile
```bash
make compile
```

To run
```bash
build/shell
```
