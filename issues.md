# WebRPC issues

sample session. client: firefox

```md
SampleServer listening on 127.0.0.1 8080
 - http://127.0.0.1:8080/system.list_methods
GET /system.list_methods HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:67.0) Gecko/20100101 Firefox/67.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1

trg_str: system.list_methods
 HTTP/1.1 200 OK
Connection: close
Server: Beast
Content-Type: text/plain
Content-Length: 81

["DateTime","Echo","GetSetValue","Revert","Shuffle","Sum","system.list_methods"]

GET /system.list_methods HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:67.0) Gecko/20100101 Firefox/67.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1
**Pragma: no-cache**
**Cache-Control: no-cache**

trg_str: system.list_methods

Catchpoint 1 (exception thrown), 0x00007ffff78bfced in __cxa_throw () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
```