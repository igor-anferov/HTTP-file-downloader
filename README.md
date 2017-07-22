# HTTP file downloader
HTTP file downloader is a tool that can download files specified in command line or in configuration file via HTTP protocol. The program supports:
- Downloading file in many threads simultaneously
- Resuming download after losing connection
- Handling redirection responses from server
- Downloading pages in chunked transfer encoding
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

You can get help by running program with option `-help`
### Program usage
```
HTTP_file_downloader [-help] [-i download_list] [-p part_count] [-o path] [URL]
```
#### Options description
|         Command        | Description |
| ---------------------- | --- |
| ` [-help] `            | Print help message. All other options will be ignored |
|                                     <br>` [-i download_list] `<br>                                     | Download multiple files. If you use this option, options `[URL]` and `[-o path]` will be ignored and program will download files specified in `download_list` file which must have next structure:<br>` URL1 [path1] `<br>` URL2 [path2] `<br>` ...          `<br>Where `URLs` are URL addresses of resources to download and `path's` are files specifying place where program will download corresponding resource. If there is no specified `path` for resource then it will be downloaded in working directory by name given to it on the server. |
| ` [-p part_count] `    | If server support partial downloading then file will be divided into `part_count` parts that will be downloaded simultaneously. |
| ` [-o path] `          | Same as `path's` in `download_list` |
| ` [URL] `              | Same as `URL's` in `download_list`  |
