# pi5-kms-video-viewer
A lightweight SDL2 middleware tool for the Raspberry Pi 5 (Bookworm/Trixie) that reads raw BGRA frames from FFmpeg via stdin and displays them fullscreen using the modern KMS/DRM stack, completely bypassing the deprecated fb0 system.

## 🚀 Why This Component Was Created
On the Raspberry Pi 5, the legacy frame buffer device (`/dev/fb0`) has been completely removed and deprecated. This breaks traditional methods of piping raw video from FFmpeg directly to a display screen. 

Because FFmpeg does not feature a built-in display output driver, this custom middleware bridges the gap. It allows you to pipe high-performance video streams directly into a hardware-accelerated SDL2 window without the heavy overhead of a massive media player like VLC or MPV.  

## ✨ Key Features
* **Zero Tearing:** Uses native hardware V-Sync through the KMS/DRM driver layers.
* **Ultra-Low CPU Usage:** Bypasses heavy display servers and legacy framebuffers.
* **Streamlined Pipeline:** Reads raw BGRA frames straight from `stdin` into memory.

## 🛠️ How to Compile It
First, make sure you have the SDL2 development libraries installed on your Raspberry Pi:
```bash
sudo apt update
sudo apt install libsdl2-dev
```
Then, compile the source file using `gcc`:
```bash
gcc -O3 kmsview.c -o kmsview -lSDL2
```

## 🎥 Example Usage with FFmpeg
You can feed raw BGRA video frames directly into the binary using a standard Linux pipe. Make sure to specify your target display resolution.

Under the old stack using fb0: as an output:
```bash
  ffmpeg -i your_video.mp4 -vf "scale=480:1280,fade=t=out:st=5.5" -pix_fmt bgra -f fbdev /dev/fb0
```

Using kmsview:
```bash
  ffmpeg -i your_video.mp4 -vf "scale=480:1280,fade=t=out:st=5.5" -pix_fmt bgra -f rawvideo pipe:1 | SDL_VIDEODRIVER=kmsdrm SDL_AUDIODRIVER=dummy ./kmsview --width 480 --height 1280
```

⚠️ Path Note: When running the command above, make sure the compiled `kmsview` binary is in the **same folder** as your video file. If it is located somewhere else, you must use the absolute path to the compiled kmsview file so FFmpeg can find it.

⚠️ Note: All standard FFmpeg video filters (-vf) remain fully functional when using kmsview! Because kmsview simply processes the raw video stream coming out of FFmpeg, you can continue to use complex filters like cropping, scaling, fading, color corrections, and speed adjustments (setpts) exactly as you did before.

