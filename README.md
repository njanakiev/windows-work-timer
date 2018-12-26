# WindowsWorkTimer

This is a simple command-line interface work-timer written for Windows that tracks your working duration and reminds you after a while by turning the screen black. 

# Usage

To start the program, run:

```bash
worktimer.exe [DURATION] ([TAG]) 
```

This would run the work time for the given duration (in seconds) and a given tag if specified. After running this command the console disappears in the background. In order to see it again, press `ALT + SHIFT + F9`. In order to stop the timer, press `ALT + SHIFT + F10`. The resulting start time, duration and tag are then stored in the log file, which is stored in the `FILE_PATH_LOG` folder and with the `FILE_NAME_LOG` name specified in the code.

# License 
This project is licensed under the MIT license. See the [LICENSE](LICENSE) for details.
