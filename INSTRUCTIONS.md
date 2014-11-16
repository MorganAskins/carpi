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
  usermod -a -G wheel audio <username>
  pacman -S sudo vim emacs git hostapd fakeroot binutils 
    patch make gcc mlocate wget mpd mpc ncmpcpp alsa-utils python
  visudo add wheel
  # Add safe_mode_gpio=4 to /boot/config.txt
  # Edit /etc/hostname and /etc/hosts
```

## Setup wifi
```
wget and install https://aur.archlinux.org/packages/ud/udhcp/udhcp.tar.gz
cp /usr/share/doc/hostapd/hostapd.conf /etc/hostapd
```
Edit /etc/hostapd/hostapd.conf # ssid, wpa_passphrase
```
    ssid=HondaPi
    wpa_passphrase=RaspberryCivic
```
Edit /etc/udhcpd.conf
```
    start 192.168.7.2
    end 192.168.7.254
    interface wlan0
    remaining yes
    opt dns 8.8.8.8 4.2.2.2
    opt subnet 255.255.255.0
    opt router 192.168.7.1
    opt lease 864000
```
Add file /etc/systemd/network/wlan0.network
```
  [Match]
  Name=wlan0
  [Network]
  Address=192.168.7.1/24
```
Setup the systemd services
```
systemctl enable hostapd.service
systemctl start hostapd
systemctl enable systemd-networkd
systemctl start systemd-networkd
# replace udhcpd.service with this custom hacky one
systemctl enable udhcpd
systemctl start udhcpd
```

## Install Hard drive
```
lsblk -f #And look at the uuid of the drive being mounted
mkdir /media/usbdrive
```
Add to /etc/fstab:
```
   UUID=<uuid> /media/usbdrive ext4 defaults 0 0
```
Set the permissions for this directory
```
chmod 770 /media/usbdrive
chown mpd:audio -R /media/usbdrive
```

## ALSA
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

Now run python install.py