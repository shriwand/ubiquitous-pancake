# ubiquitous-pancake
WhatsApp VoIP call recorder created for educational purposes.

# Setting up the emulator

Use android emulator because of availability & simplicity in terms of OS recovering.

## AVD creation

Use Android 14.0 ("UpsideDownCake") provided by Google.

[Image version](images/avd-image.png)

[AVD setup](images/avd-setup.png)

[AVD in Device Manager](images/avd-in-devicemanager.png)

## Rooting the emulator
Use [this project](https://gitlab.com/newbit/rootAVD) for getting root access for the emulator.
There is [an instruction](https://samsclass.info/128/proj/M142.htm) how to use root the emulator on different OSs using *rootAVD*.

Steps: 
1. Enable Developer Options & USB debugging.
2. Get *rootAVD*: `git clone https://gitlab.com/newbit/rootAVD && cd rootAVD`
3. Install *Magisk*: 
``sh
export PATH=$PATH:~/Android/Sdk/platform-tools/
./rootAVD.sh system-images/android-34/google_apis_playstore/x86_64/ramdisk.img
``
4. Cold Boot the emulator [](images/cold-boot.png).
5. Open & install *Magisk* [](images/open-magisk.png)
6. `adb shell su` and allow superuser request in the emulator. [](images/superuser-request.png)

## WhatsApp installation.
Install WhatsApp from Google Play.
