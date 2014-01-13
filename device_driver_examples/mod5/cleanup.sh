# cleanup.sh
#
# this script will remove a specific module, and
# delete the node created earlier using the install.sh
#
# remember, run this as a system admin (root user)


# adjust the value below for the name you register in your LKM
module="module5"

# adjust the value below for the name you wish to install
# in the file system
device="module5"

# remove the LKM 
rmmod $module 

# forcibly remove any existing file system devices 
rm -f /dev/$device


