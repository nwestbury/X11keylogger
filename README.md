# Key Recorder

Simple key recorder and replayer.

## Dependencies

Only works on X11-based systems.

Requires `libxtst-dev` package on Debian-based distros.

## Basic Usage

`$ make`
`$ ./keyrecorder out.csv`
`$ ./keyplayer out.csv`

## Credits

Builds off the work done by anko's xkbcat (https://github.com/anko/xkbcat)
which in turns builds off the work of Jon A. Maxwell.