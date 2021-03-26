export ONLINE_MAIN=/usr/local
export OFFLINE_MAIN=/usr/local
export PATH=$ONLINE_MAIN/bin:$PATH
export LD_LIBRARY_PATH=$ONLINE_MAIN/lib
source $ONLINE_MAIN/bin/aliases.sh
export ROOT_INCLUDE_PATH=$ONLINE_MAIN/include:$ONLINE_MAIN/include/Event:$ONLINE_MAIN/include/pmonitor