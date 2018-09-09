# Key Recorder

Simple key recorder and replayer.

## Dependencies

Only works on X11-based systems.

Requires `libxtst-dev` package on Debian-based distros.

## Basic Usage

```bash
$ make`
$ ./keyrecorder out.csv
$ ./keyplayer out.csv
```

To combine multiple files use the utility python script:

```bash
python combine_csv.py out1.csv out2.csv out3.csv combined.csv --normalize --gap 3000
```

# Sample
The `sample` folder contains an example recording for the Flash Game 'Choppy Orc'.
Playing it back (`./keyreplayer sample/levels1-15.csv -d 1`) allows for a solid
Tool-Assisted-Speedrun (TAS) of the game.

## Credits

Builds off the work done by X11 maintainers, anko's xkbcat (https://github.com/anko/xkbcat)
which in turns builds off the work of Jon A. Maxwell.
