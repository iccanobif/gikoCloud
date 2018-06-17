# gikoCloud

To build the docker container:
```
docker build -t gikocloud_dev .
```
To run the docker container (replace "ROOT_DIRECTORY" with the absolute path of the directory containing the repo):
```
docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -ti \
           --mount type=bind,source=ROOT_DIRECTORY,target=/src -p 8080:8080 gikocloud_dev
```
You can build everything and start the node.js server from inside the docker with 
```
cd src/web
npm install
cd ../..
make
./run
```
and then open the web page pointing a browser to `http://localhost:8080`.
