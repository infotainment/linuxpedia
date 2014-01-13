# install.sh
#
# this script will install a specific module, and
# create a node by querying the /proc/devices file for
# the registered module name. You need to update this
# script for new modules you build in the future.
#
# remember, run this as a system admin (root user)


# adjust the value below for the name you register in your LKM
module="module4"

# adjust the value below for the name you wish to install
# in the file system
device="module4"

# adjust the value below for the minor number your LKM registered
minor="0"

# install the LKM and exit if insmod fails with an error
insmod ./$module.ko || exit 1

# query the /proc/devices file
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

# forcibly remove any existing file system devices (cleanup from
# previous attempts at running this script)
rm -f /dev/$device

# create the new file system node based on major/minor device numbers
mknod /dev/$device c $major $minor

# ensure device file is readable/writable by all
chmod 666 /dev/$device

