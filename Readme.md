# Shell

A simple shell implemented in cpp.

I've only implemented a limited subset of functionality, atm basically just `cd`, pipes and file redirection.

Quit with `^C` or `^D` (I've not implemented passing SIGINT to child processes yet).


To compile
```bash
make compile
```

To run
```bash
build/shell
```
