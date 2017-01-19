read  -p "ip addr:" ip
for i in 1 2 3 4 5 6 7 8 9 10
do
	iperf -c ${ip} -t 10 >> iperf_data.txt
done
