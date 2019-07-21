# Calendar App

## What is it?
A calendar application with an easy-to-use interface that can save its current contents in a configuration file and then read from it on the next startup.

## What's included?
* Source code (C and Header files)
* Executable application
* Example configuration file (config.cfg)

## How it works.
The calendar app uses a C/C++ based library called [libconfig](https://github.com/hyperrealm/libconfig) for the reading and writing of the local configuration file.

Although useful for getting started quickly and creating a simple application, libconfig also limits the amount of control a programmer has over their project.

## What comes next?
As a result of the limitations of libconfig, future version of this application will eventually abandon the library for a more simplistic and controllable solution.

