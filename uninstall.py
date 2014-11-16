
import os, sys

def main():
    if os.geteuid() is not 0:
        sys.exit("You must run this as root\nExiting")
    
    print("uninstalling programs and services")
    services = ['carpi.service', 'carpi_alive.service', 'fade_volume.service']
    programs = ['carpi_alive', 'state-save', 'fade_volume']

    for fname in services:
        os.system('systemctl stop '+fname)
        os.system('systemctl disable '+fname)
        try:
            os.remove('/usr/lib/systemd/system/'+fname)
        except OSError:
            pass

    for fname in programs:
        try:
            os.remove('/usr/bin/'+fname)
        except OSError:
            pass

if __name__ == '__main__':
    main()
        
