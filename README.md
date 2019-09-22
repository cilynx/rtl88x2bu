# Driver for rtl88x2bu wifi adaptors

Updated driver for rtl88x2bu wifi adaptors based on Realtek's source distributed with myriad adapters.

Realtek's 5.6.1 source was found bundled with the [Cudy WU1200 AC1200 High Gain USB Wi-Fi Adapter](https://amzn.to/351ADVq) and can be downloaded from [Cudy's website](http://www.cudytech.com/productinfo/517558.html).

Build confirmed on:

```bash
Nothing just yet
````

## DKMS installation

```bash
cd rtl88x2bu
VER=$(sed -n 's/\PACKAGE_VERSION="\(.*\)"/\1/p' dkms.conf)
sudo rsync -rvhP ./ /usr/src/rtl88x2bu-${VER}
sudo dkms add -m rtl88x2bu -v ${VER}
sudo dkms build -m rtl88x2bu -v ${VER}
sudo dkms install -m rtl88x2bu -v ${VER}
sudo modprobe 88x2bu
```

## Raspberry Pi Access Point

```bash
# Update all packages per normal
sudo apt update
sudo apt upgrade

# Install prereqs
sudo apt install git dnsmasq hostapd bc build-essential dkms raspberrypi-kernel-headers

# Reboot just in case there were any kernel updates
sudo reboot

# Pull down the driver source
git clone https://github.com/cilynx/rtl88x2bu
cd rtl88x2bu/

# Configure for RasPi
sed -i 's/I386_PC = y/I386_PC = n/' Makefile
sed -i 's/ARM_RPI = n/ARM_RPI = y/' Makefile

# DKMS as above
VER=$(sed -n 's/\PACKAGE_VERSION="\(.*\)"/\1/p' dkms.conf)
sudo rsync -rvhP ./ /usr/src/rtl88x2bu-${VER}
sudo dkms add -m rtl88x2bu -v ${VER}
sudo dkms build -m rtl88x2bu -v ${VER} # Takes ~3-minutes on a 3B+
sudo dkms install -m rtl88x2bu -v ${VER}

# Plug in your adapter then confirm your new interface name
ip addr

# Set a static IP for the new interface (adjust if you have a different interface name or preferred IP)
sudo tee -a /etc/dhcpcd.conf <<EOF
interface wlan1
    static ip_address=192.168.4.1/24
    nohook wpa_supplicant
EOF

# Clobber the default dnsmasq config
sudo tee /etc/dnsmasq.conf <<EOF
interface=wlan1
  dhcp-range=192.168.4.100,192.168.4.199,255.255.255.0,24h
EOF

# Configure hostapd
sudo tee /etc/hostapd/hostapd.conf <<EOF
interface=wlan1
driver=nl80211
ssid=pinet
hw_mode=g
channel=7
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=CorrectHorseBatteryStaple
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
EOF

sudo sed -i 's|#DAEMON_CONF=""|DAEMON_CONF="/etc/hostapd/hostapd.conf"|' /etc/default/hostapd

# Enable hostapd
sudo systemctl unmask hostapd
sudo systemctl enable hostapd

# Reboot to pick up the config changes
sudo reboot
```

If you want to setup masquerading or bridging, check out [the official Raspberry Pi docs](https://www.raspberrypi.org/documentation/configuration/wireless/access-point.md).
