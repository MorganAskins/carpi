
import os, sys, shutil

def main():
    if os.geteuid() is not 0:
        sys.exit("You must run this as root\nExiting")
    files_here = os.listdir()
    services = ['carpi.service', 'carpi_alive.service', 'fade_volume.service']
    programs = ['carpi_alive', 'state-save', 'fade_volume']
    # Check if ALL services are available
    if not all(x in files_here for x in services):
        sys.exit("I can't seem to find the service files ...\nCya!")
    if not all(x in files_here for x in programs):
        if 'state-save' not in files_here:
            print('Make sure to go into mpdstate and make')
        sys.exit("I can't seem to find the programs ...\nCya!")

    # Move the programs to /usr/bin
    for fname in programs:
        newname = '/usr/bin/'+fname
        shutil.copyfile(fname, newname)
        # and set the permissions
        os.chmod(newname, 0o755)
    # Move service files to /usr/lib/systemd/system
    for fname in services:
        shutil.copyfile(fname, '/usr/lib/systemd/system/'+fname)
        os.system('systemctl enable '+fname)

    print(", ".join(services), 'will start on reboot')

if __name__ == '__main__':
    main()
