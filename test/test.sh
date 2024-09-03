# /bin/bash

printf "Compiling server...\n"
make re
make clean
valgrind --leak-check=full --show-leak-kinds=all --error-limit=no --log-file=valgrind.log ./webserv test/test.conf > webserv.log 2>&1 &
SERVER_PID=$!
printf "Server PID: $SERVER_PID\n"

printf "Initializing server...\n"
sleep 2
printf "\n"

python3 test/test_basic_requests.py

printf "\nRunning Siege on http://localhost:8080/siege.html\n"
siege -b -t1M http://localhost:8080/siege.html

printf "\n"
printf "Killing server process...\n"
kill -2 $SERVER_PID
printf "Waiting for server's terminating sequence...\n"
wait $SERVER_PID

printf "\nValgrind report:\n"
cat valgrind.log | grep "HEAP SUMMARY:" -A 4

printf "\nTo print the server log use the following command:\n"
printf "cat webserv.log\n"
