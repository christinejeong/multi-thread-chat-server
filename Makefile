.PHONY: all server client dashboard clean

all: server client

server:
	$(MAKE) -C server

client:
	$(MAKE) -C client

dashboard:
	cd dashboard && pip3 install -r requirements.txt

run-server: server
	./server/chat_server

run-dashboard: dashboard
	cd dashboard && python3 app.py

clean:
	$(MAKE) -C server clean
	$(MAKE) -C client clean