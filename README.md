# Description
A simple, practical solution for keeping local modifications of Steam Workshop items in games.

Have you ever needed to change something about a workshop addon without uploading your own version? Assuming you did, you probably felt the anger from Steam verifying your files after you've made the changes, essentially undoing them. 

swpatcher fixes this issue by storing your patched workshop addons in separate directories from your other addons, and it works like this: it copies your modded files over the original files, and then launches the game.

swpatcher is solely a CLI tool. It is designed to be called in the command line, or indirectly through a script in the Steam launch options of a title.

# Usage

```
swpatcher --appid=<id> [--executablePath=<path>] | [--absoluteWorkshopPath=<path> && --absoluteExecutablePath=<path>] ; [--suffix=<string>]
        appid                  (integer) = the appid of the steam game you're patching
        executablePath         (path)    = the relative executable path. use only when swpatcher is in the root folder of the game
        absoluteWorkshopPath   (path)    = the (non-relative) path to `steamapps/workshop/content/`
        absoluteExecutablePath (path)    = the (non-relative) path to the game executable
        suffix                 (string)   = name appended to the end of a directory to mark it as to be copied; default value is `_patched`
```

# Examples

## Command-line Interface

```
E:\SteamLibrary\steamapps\common\Perfect Heist 2> .\swpatcher --appid=1521580 --executablePath="PerfectHeist2.exe"
```

```
Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
d-----         5/16/2026   8:26 PM                Engine
d-----         5/16/2026   8:26 PM                PerfectHeist2
-a----         5/16/2026   8:24 PM            304 Manifest_DebugFiles_Win64.txt
-a----         5/16/2026   8:24 PM          11115 Manifest_NonUFSFiles_Win64.txt
-a----         5/16/2026   8:24 PM         268800 PerfectHeist2.exe
-a----          9/5/2026  12:00 PM              9 steam_appid.txt
-a----          9/5/2026  12:00 PM         316553 swpatcher.exe
```

## Steam Launch Options

```
.\patch_and_launch %command%
```

Calling the swpatcher with arguments in the Steam launch options doesn't seem to work, so your best bet is to wrap the call in a script.
```
.\swpatcher --appid=1521580 --executablePath="PerfectHeist2.exe"
```