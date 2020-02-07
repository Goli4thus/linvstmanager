#!/usr/bin/env python3

''' 
Purpose:
    Export all needed icon resolutions of application icon
    using inkscape cli-interface.
''' 

import subprocess


def main():
    resolutions = [14, 16, 22, 24, 32, 36, 48, 64, 72, 96, 128, 160, 192, 256, 384, 512, 1024]
    for res in resolutions:
        # example: inkscape linvstmanager.svg -D -w 72 -h 72 -e linvstmanager_72.png
        filename = "linvstmanager_" + str(res) + ".png"
        try:
            subprocess.run(["inkscape", "linvstmanager.svg", "-D", "-w", str(res), "-h", str(res), "-e", filename])
        except Exception as e:
            print('Failed at something %s. Reason: %s' % (str(res), e))



if __name__ == '__main__':
    main()
