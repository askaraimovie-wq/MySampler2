# My Sampler (VST3)

A simple sampler plugin: load a sound, play it on the keyboard, shape it with
Attack, Decay, Sustain, Release and Gain knobs. Middle C plays the sample at
its original pitch.

## Get the plugin without installing anything (works from an Android phone)

1. Create a free account at github.com and make a **new repository**.
2. Upload every file and folder from this project into it
   (keep the `.github/workflows/build.yml` path exactly).
3. Open the **Actions** tab. The "Build plugin" job starts by itself
   (or press **Run workflow**). It takes about 10 to 15 minutes.
4. When it finishes, open the run and download the **Artifacts**:
   - `MySampler-windows-latest` for Windows
   - `MySampler-macos-latest` for Mac
5. Unzip it on your laptop. You get `My Sampler.vst3`.

## Install on the laptop

- **Windows:** copy `My Sampler.vst3` to `C:\Program Files\Common Files\VST3`
- **Mac:** copy it to `/Library/Audio/Plug-Ins/VST3`

Then rescan plugins in your DAW (FL Studio, Ableton, Reaper, etc.).
There is also a Standalone app in the same download if you want to test
without a DAW.

## Using it

1. Click **Load Sample** and choose a WAV, AIFF, FLAC or OGG file
   (up to 30 seconds).
2. Play the on-screen keyboard or send MIDI from your DAW.
3. Shape the sound with the knobs. The sample path is saved with your project.

## Note for Mac

An unsigned plugin may be blocked the first time. If so, right-click the file,
choose Open, or allow it in System Settings, Privacy and Security.
