Car-Pi
======

Download a copy of Arch Linux for Arm Processors to be put on an SD card

## Setup SD Card
fdisk /dev/sdX (where /dev/sdX is the SD card)
  - at fdisk type o to clear partitions
  - type p to list partitions (should be none)
  - type n, then p for primary, 1 for first, enter for default first sector, then +100M
  - type t, then c to set the first partition to type W95 FAT32
  - type n, p, 2, enter twice for default first and last
  - w to write and exit

Setup the SD card before loading onto the pi
  - mkfs.vfat /dev/sdX1
  - mkdir boot
  - mount /dev/sdX1 boot
  - mkfs.ext4 /dev/sdX2
  - mkdir root
  - mount /dev/sdX2 root
  - wget http://archlinuxarm.org/os/ArchLinuxARM-rpi-latest.tar.gz
  - tar xzf ArchLinuxARM-rpi-latest.tar.gz -C root
  - sync
  - mv root/boot/* boot
  - umount boot root

## Setup the pi
Login to the pi: use LAN and figure out it's ip, a good way is nmap -Sp 192.168.1.1/24
   - ssh root@alarmpi #password is root
```
  pacman -Syu
  passwd
  # Install any shells you want now (e.g. zsh)
  useradd -m -g <group> -s <shell> <username>
  usermod -a -G wheel <username>
  pacman -S sudo vim emacs git hostapd fakeroot binutils 
    patch make gcc mlocate wget mpd mpc ncmpcpp alsa-utils
  visudo add wheel
  # Add safe_mode_gpio=4 to /boot/config.txt
  # Edit /etc/hostname and /etc/hosts
```

## Setup wifi
12. wget and install https://aur.archlinux.org/packages/ud/udhcp/udhcp.tar.gz
12. cp /usr/share/doc/hostapd/hostapd.conf /etc/hostapd
12. edit /etc/hostapd/hostapd.conf # ssid, wpa_passphrase
```python
    ssid=HondaPi
    wpa_passphrase=RaspberryCivic
```
13. systemctl enable hostapd.service # Start it too
14. edit /etc/udhcpd.conf
```python
    start 192.168.7.2
    end 192.168.7.254
    interface wlan0
    remaining yes
    opt dns 8.8.8.8 4.2.2.2
    opt subnet 255.255.255.0
    opt router 192.168.7.1
    opt lease 864000
```
14. add file /etc/systemd/network/wlan0.network
```python
  [Match]
  Name=wlan0
  [Network]
  Address=192.168.7.1/24
```
14. systemctl enable systemd-networkd
14. systemctl start systemd-networkd
14. replace udhcpd.service with this custom hacky one
14. systemctl enable udhcpd
14. systemctl start udhcpd

## Install Hard drive
1. Get blk id with lsblk -f
2. create mount directory /media/usbdrive
3. Add to /etc/fstab:
   `UUID=<uuid> /media/usbdrive ext4 defaults 0 0`
4. set permissions on this directory chmod 770, chown mpd:audio -R
  

## Install mpd

## ALSA
usermod -aG audio <username>
set levels in alsamixer (should show name of usb dac)
** If the usb device isn't first, disable bcm2835
** modprobe -r snd_bcm2835 (with mpd stopped!) TODO:: write asound.conf
** locate and mv bcm2835 to a backup folder that isn't in modules
aplay /usr/share/sounds/alsa/Front_Right.wav # This should play sound
x. configure mpd
  usermod -aG audio mpd
  edit /etc/mpd.conf
  - music_directory "/var/lib/mpd/music"
  - volume_normalization "yes"
  touch /var/lib/mpd/mpd.db
  touch /var/lib/mpd/mpdstate
  ln -s /media/usbdrive/music /var/lib/mpd/music
  chown mpd:audio -R /var/lib/mpd
  chmod 770 -R /var/lib/mpd
  systemctl enable mpd.service
  systemctl start mpd
  mpc update
  # At this point check if mpc can play through the pi audio out

** Other bits
timedatectl set-timezone America/Los_Angeles
timedatectl set-time

Set up systemd files:
    cp carpi.service and fade_volume.service to /usr/lib/systemd/system/
    cp state-save and fade_volume to /usr/bin
    enable carpi and fade_volume in systemd