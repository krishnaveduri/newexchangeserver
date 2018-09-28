# Exchange Server

Exchange Server is a trading backend with high-speed performance, designed for cryptocurrency exchanges. It can support up to 10000 trades every second and real-time user/market data notification through websocket.

## Architecture

![Architecture](https://user-images.githubusercontent.com/1209350/32476113-5ffc622a-c3b0-11e7-9755-924f17bcc167.jpeg)

For this project, it is marked as Server in this picture.

## Code structure

**Required systems**

* MySQL: For saving operation log, user balance history, order history and trade history.

* Redis: A redis sentinel group is for saving market data.

* Kafka: A message system.

**Base library**

* network: An event base and high performance network programming library, easily supporting [1000K TCP connections](http://www.kegel.com/c10k.html). Include TCP/UDP/UNIX SOCKET server and client implementation, a simple timer, state machine, thread pool. 

* utils: Some basic library, including log, config parse, some data structure and http/websocket/rpc server implementation.

**Modules**

* matchengine: This is the most important part for it records user balance and executes user order. It is in memory database, saves operation log in MySQL and redoes the operation log when start. It also writes user history into MySQL, push balance, orders and deals message to kafka.

* marketprice: Reads message(s) from kafka, and generates k line data.

* readhistory: Reads history data from MySQL.

* accesshttp: Supports a simple HTTP interface and hides complexity for upper layer.

* accwssws: A websocket server that supports query and pushes for user and market data. By the way, you need nginx in front to support wss.

* alertcenter: A simple server that writes FATAL level log to redis list so we can send alert emails.

## Compile and Install

**Operating system**

Ubuntu 14.04 or Ubuntu 16.04. Not yet tested on other systems.

**Requirements**

See [requirements](https://github.com/viabtc/viabtc_exchange_server/wiki/requirements). Install the mentioned system or library.

You MUST use the depends/hiredis to install the hiredis library. Or it may not be compatible.

**Compilation**

Compile network and utils first. The rest all are independent.

**Deployment**

One single instance is given for matchengine, marketprice and alertcenter, while readhistory, accesshttp and accwssws can have multiple instances to work with loadbalancing.

Please do not install every instance on the same machine.

Every process runs in deamon and starts with a watchdog process. It will automatically restart within 1s when crashed.

The best practice of deploying the instance is in the following directory structure:

```
matchengine
|---bin
|   |---matchengine.exe
|---log
|   |---matchengine.log
|---conf
|   |---config.json
|---shell
|   |---restart.sh
|   |---check_alive.sh
```

## API

[HTTP Protocol](https://github.com/viabtc/viabtc_exchange_server/wiki/HTTP-Protocol) and [Websocket Protocol](https://github.com/viabtc/viabtc_exchange_server/wiki/WebSocket-Protocol) documents are available in Chinese. Should time permit, we will have it translated into English in the future.</br>
[Python3 API realisation](https://github.com/testnet-exchange/python-viabtc-api)


## Websocket authorization

The websocket protocol has an authorization method (`server.auth`) which is used to authorize the websocket connection to subscribe to user specific events (trade and balance events).

To accomodate this method your exchange frontend will need to supply an internal endpoint which takes an authorization token from the HTTP header named `Authorization` and validates that token and returns the user_id.

The internal authorization endpoint is defined by the `auth_url` setting in the config file (`accessws/config.json`).

Example response: `{"code": 0, "message": null, "data": {"user_id": 1}}`

## Exchange Server - Installation 

$ sudo apt install -y libev-dev libjansson-dev libmpdec-dev libmysqlclient-dev libcurl4-gnutls-dev libldap2-dev libgss-dev librtmp-dev libsasl2-dev 

//librdkafka: 0.11.3+; DO NOT INSTALL BY APT: version too old (0.8.x);

//if you do, remove them by: sudo apt remove librdkafka1 librdkafka-dev

$ wget https://github.com/edenhill/librdkafka/archive/v0.11.3.tar.gz -O librdkafka-0.11.3.tar.gz

$ tar zxf librdkafka-0.11.3.tar.gz

$ cd librdkafka-0.11.3

$ ./configure

$ make

$ sudo make install


$ git clone https://gopi_dodla@bitbucket.org/gopi_dodla/exchangeserver.git

$ cd exchangeserver

$ make -C depends/hiredis

$ make -C network

$ sudo apt install -y libssl-dev

$ vi utils/makefile #modify INCS

//INCS = -I ../network -I ../depends

$ make -C utils

$ vi accesshttp/makefile #modify INCS & LIBS

//INCS = -I ../network -I ../utils -I ../depends

//LIBS = -L ../utils -lutils -L ../network -lnetwork -L ../depends/hiredis -Wl,-Bstatic -lev -ljansson -lmpdec -lrdkafka -lz -lssl -lcrypto -lhiredis -lcurl -Wl,-Bdynamic -lm -lpthread -ldl -lssl -lldap -llber -lgss -lgnutls -lidn -lnettle -lrtmp -lsasl2 -lmysqlclient

$ make -C accesshttp

$ vi accessws/makefile

{modify INCS and LIBS like accesshttp/makefile}

$ make -C accessws

vi alertcenter/makefile

{modify INCS and LIBS like accesshttp/makefile}

$ make -C alertcenter

$ vi marketprice/makefile

{modify INCS and LIBS like accesshttp/makefile}

$ make -C marketprice

$ vi matchengine/makefile

{modify INCS and LIBS like accesshttp/makefile}

$ make -C matchengine

$ vi readhistory/makefile

{modify INCS and LIBS like accesshttp/makefile}

$ make -C readhistory


# Websocket configuration

Nginx server is required to access websocket server.

Place the accessws.conf file in the Nginx configuration directory /etc/nginx/conf.d/  to access websocket.

The websocket is configured to accessible on the port 8090.
