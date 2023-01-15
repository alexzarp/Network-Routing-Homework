# Network-Routing-Homework
## Compile
run the command bellow

```
gcc src/*.c -o router -pthread
``` 
## Run
Execute the binary n times to each router that you want
```
./router idrouter
```
## Commands
* Send message 
```
Message: RouterID Message
```
* List know router status
```
Message: -1 lstatus
```
* Shutdown router
```
Message: -1 q
```