# HTTP file downloader
Tool that can download files specified in command line or in configuration file via HTTP protocol. Program supported:
- Downloading file in many threads simultaneously
- Resuming download after losing connection
- Handling redirection responses from server
- Animated download status

## Installation

In the terminal window, change the working directory to the directory where the HTTP file downloader will be loaded, and run these commands:
```
git clone https://github.com/igor-anferov/HTTP-file-downloader.git
cd HTTP-file-downloader
mkdir build
cd build
cmake ..
make
```
After this working directory will be changed to HTTP-file-downloader/build and you'll be able to run HTTP file downloader by typing `./HTTP-file-downloader`

## Help

You can call help by typing 
`./HTTP-file-downloader -help`