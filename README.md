# Introduction

Cello - A tool to measure video latency in a SDI workflow.

Imagine a workflow, a black box. It has a SDI input and a SDI output.
For example, a video encoder -> video decoder workflow.

The tool used the blackmagic decklink DUO2 four port SDI card to create
and monitor video entering and leaving the blackbox.

Specifically, the physical ports on the rear of the DUO2 card, left to right are:
REF A B C D

Cello receives normal HD video (720p or 1080i) signal on input A. This can be anything
you like, a product feed, a debug feed, colorbars, whatever.

Cello burns in video metadata into the input signal, and outputs this on port C.

Feed port C to the input of the blackbox workflow.

The blackbox workflow eventually outputs SDI video, connect this to port D.

Cello analyzes the video differences between port C and D, extracts metadata and calculates
the video latency between these two points.

Cello outputs the statistics to the console.

Cello outputs a copy of the port D signal on Port B, and burns into the signal additional
information, including the measured latency.

## License

	LGPL-V2.1
	See the included lgpl-2.1.txt for the complete license agreement.

## Dependencies
* ffmpeg
* readline

## Compilation
    ./autogen.sh --build
    ./configure
    make

## Features
* Any reasonable framerate (29.97, 30, 59.94, 60) is supported and the blackbox may adjust the framerate.
* Any HD video format is supported (720p 1080i/p), the blackbox may adjust the format.
* Once running, if cello detects 2 seconds of video loss, the process self terminates.
* The output on port B allows production staff to use the product with no access to Linux consoles, as a pure video appliance.

## Known Limitations
* Only video is processed, no audio is pushed or received from the blackbox workflow.
* No support for SD formats.
* No support for VANC processing.
* Tested and developed on Linux.

## Todo
* Audio support.
* Support for NDI workflows.
* Support for hybrid SDI/NDI workflows.
* Minimal web-server, presenting thumbnails and latency calculations for remote use.
