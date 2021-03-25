rcdaq_client daq_set_maxvolume 1000
rcdaq_client daq_begin $1
while :
do
    rcdaq_client daq_status | grep 'Stopped' &> /dev/null
    if [ $? == 0 ]; then
	rcdaq_client daq_set_maxvolume 1000
	rcdaq_client daq_begin
    fi
    sleep 0.8
done