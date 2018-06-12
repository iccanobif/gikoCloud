# gikoCloud

To build docker container:
```
docker build -t qt .
```
To run the docker container:
```
docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -ti \
           --mount type=bind,source=ROOT_DIRECTORY,target=/src -p 8080:8080 qt
```
Replacing "ROOT_DIRECTORY" with the absolute path of the directory containing the repo.

You can start the node.js server from inside the docker with 
```
./run
```
and then open the web page pointing a browser to `localhost:8080`
