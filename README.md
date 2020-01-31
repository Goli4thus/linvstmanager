# LinVstManager
LinVstManager is a companion application that allows managing VSTs
in conjunction with various VST bridges created by osxmidi
(see https://github.com/osxmidi/LinVst).

## Features
- currently supported bridges:
    - https://github.com/osxmidi/LinVst
    - https://github.com/osxmidi/LinVst-X
    - https://github.com/osxmidi/LinVst3
    - https://github.com/osxmidi/LinVst3-X
- Batch adding of VSTs
- Scanning for VSTs
- GUI and shortcut driven user interaction
- No more manual config file editing (as was the case with https://github.com/Goli4thus/linvstmanage)
- Update/Create *.so files for all VSTs listed in config file
- Enable/Disable VSTs by adding/removing a softlink to the respective *.so file.
  All softlinks are mapped to a *link folder* which can be scanned by your preferred DAW.
- Detection and optional removal of orphaned _*.so-links/files_ in _link folder_
- and more...

## Dependencies
- TODO (obviously qt5; but anything on top apart from what is implied by LinVst?)

## Usage
### First start

### Manually adding VSTs


### Scanning for VSTs



Config file location: ~/.config/linvst/manager/linvstmanager_config.xml

If you'd prefer a different location you could always symlink your config file to one of these locations.
i.e. `ln -s ~/myconfigs/linvstConfig.ini ~/.config/linvst/manage/linvstmanage.ini`
    
    
## Further notes
### VST states

![Status view](https://github.com/Goli4thus/linvstmanage/blob/master/img/uiStatus.png)

TODO: update with current ones
| State       | Meaning                                                                    |
| --------    | -------------------------------------------------------------------------- |
| Enabled     | VST is enabled via active softlink                                         |
| Disabled    | VST is disabled due to missing softlink                                    |
| Mismatch    | Mismatch between linvst.so template and *.so file associated with VST file |
| No *.so     | VST-dll has no associated VST-so file                                      |
| Notfound    | VST-dll can't be found using the specified path                            |
| NoBridge,   | No suitable bridge has been enabled in preferences.                        |
| Orphan,     | The so-file seems orphaned as it doesn't refer to an existing VST-dll      |
| NA,         | Initial state a                                                            |
| Blacklisted | VST is blacklisted from being handled                                      |

Note: If a VST is 'Blacklisted', _enable_, _disable_, _update_ all will have no effect on
it until that VST is 'unblacklisted' once again.

    
### Usability features
- lots of tooltips
- shortcuts everywhere
- table sorting and filtering
    
![Help view](https://github.com/Goli4thus/linvstmanage/blob/master/img/uiHelp.png)
    
    
### Typical setup
TODO: Worth an update
Different "VST.dll"s are installed into different locations due to making use of different wine prefixes per plugin vendor (i.e. to allow for different dll overrides on a vendor basis).
This gives us a number of different folders containing one or more "VST.dll"s.

For each such folder we create a section within the config file _linvstmanage.ini_ (important: section names must be unique).
Each section is defined by it's path and one or more "VST.dll"s.

After starting *linvstmanage-cli* and assuming the config file is correctly setup, the status view will show an entry for each VST.dll.
Initially the status of each will be _No *.so_ (if this is a fresh setup without prior LinVst converts).

Performing an _update_ will create accompaning VST-so files for each VST.dll.
This results in the status of all VSTs going to _Disabled_.

After the VSTs can be _Enabled_, which creates symlinks within the _link folder_ referencing all previously created VST-so files.

This central _link folder_ can be scanned by whatever DAW you are using.


### Some usecases
#### Quick enable/disable of VSTs in the DAW
If one wanted to temporarily disable and later on enable a VST managed by LinVst it either meant:
    - deleting the so-files or symlinks manually or
    - having your own script that could do this or 
    - a combination of both

With this script it's centrally managed and _pretty_ fast to handle this usecase.

#### Updating for trying out different versions of a bridge
In case of updating or changing versions of LinVst (or another bridge), it can happend that one ends up with a version mismatch between the installed LinVst server executables and the VST-so files associated with "VST-dll"s.
In order to clean this mess up it's enough to run *LinVstManager* and perform and _update_ (assuming the mismatch was due to a managed VST).

### Scanning for VSTs

### Blacklisting VSTs
Why?
Let's say you've narrowed down to a subfolder within a wine prefix, but there still seem to be
many dll files that aren't VST files.
You could perform a scan *without* the 'verify' option. That would result in all non-VST-dll files
being detected as VST files.
After that you could add and blacklist those.
During subsequent scans these dll files would be ignored due to already being "tracked".
In addition they are _'blacklisted'_ and won't interfere during other operations (i.e. enable all,...).



