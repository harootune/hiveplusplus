# hiveplusplus
A Universal Hive Protocol (UHP) compliant Hive engine which implements move-ordering heuristics and supports custom piece configurations. 

Originally created as a final project for IS597DSO - Data Structures and Algorithms. However, support is continuing.

For more information on UHP and how to use this engine, see the [Universal Hive Protocol proposal](https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol) on the Mzinga wiki.

## Installation
Download the binary, or compile with `make build`, then run the `hpp` executable.

Remove the executable and object files with `make clean`, or just the .o files with `make clean_objects`.

## Testing 
As this current version was made on a short timescale for a school project, no formal testing harness is in place. A basic testing harness will be included in the next significant update.

## Piece Configurations - .pconfig Files
hiveplusplus supports custom piece configurations to encourage exploration of novel variants of hive. It does this via .pconfig files, a simple plaintext file format which describes a piece configuration.

A single line in a .pconfig file looks like this:

```
[int pieceCode]: [int numPieces]
```
Each pieceCode indicates a piece type, and numPieces indicates the number of pieces of that type available. Valid piece codes are:  
  
| UHP Piece Label | Code |
|-----------------|------|
|        wQ       |   0  |
|        wA       |   1  |
|        wB       |   2  |
|        wG       |   3  |
|        wS       |   4  |
|        bQ       |   5  |
|        bA       |   6  |
|        bB       |   7  |
|        bG       |   8  |
|        bS       |   9  |

The file format is currently somewhat fragile, and does not tolerate comments or blank lines. Additionally, valid piece configurations meet the following constraints:

1. There must be 1 and only 1 wQ, and 1 and only 1 bQ. .pconfig files that violate this constraint will be read, but will fail the validation step and log an `err` message.
2. No negative values. .pconfig files that violate this constraint will fail at the read step and log an `err` message.

## Using Piece Configurations

To run a game using a custom piece configuration, set the `CustomPath` option described later in this document to the path to the .pconfig file you would like to load. Then, start a `newgame` which specifies `Custom` in place of the usual `GameTypeString`. If the piece config is not valid or the file cannot be found, a corresponding `err` message will be logged.

## Extensions to UHP

To accomodate custom piece configuration functionality, this engine extends the UHP with an additional `OptionType` `string`, describing a string of characters read directly from input. The format of a `string`-typed option is:

```
OptionName;string;OptionValue;DefaultValue
```

Additionally, supplementary information produced by the Engine (e.g. best move at a certain search depth) is universally prepended with `note` to disambiguate output.

## Options
hiveplusplus currently exposes three options to the user.

| Option       | Typing               | Description                                                                         |
|--------------|----------------------|-------------------------------------------------------------------------------------|
| CustomPath   | string;\<value\>;      | The path to a .pconfig file to be loaded by a Custom game                           |
| MaxTableSize | int;\<value\>;1024;16; | The maximum size of the transposition table in MB                                   |
| DefaultDepth | int;\<value\>;4;1;     | The maximum depth for any `bestmove` search that does not specify a depth parameter |

## Known Issues

### Scoring Heuristic Quality
This engine was made with amateur insight into Hive at best, and so the scoring heuristic has significant room for improvement. Improving the scoring heuristic is an ongoing goal for continued support.

### Expansion Piece Support
The engine does not currently support Mosquito, Ladybug, or Pillbug pieces. However, it is well-set-up to do so, and an update supporting these pieces is prioritized.

### Code Architecture
Code organization could be improved to better comply with SOLID principles and general C++ fluency. This will also be improved over time.

## Reporting Bugs

Please report issues via the github issue tracker.

## Changelog

**v0.1.0**
- Initial release


