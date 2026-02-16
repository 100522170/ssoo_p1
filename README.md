Contributors: 
	- José Daniel García
	- Alberto Cascajo


# SSOO docker container

Docker container for the Operating Systems course.

## Using Docker

In order to complete this lab you should fulfill the following prerequisites:

* Install Docker Desktop in your host machine
* Install Visual Studio Code

The docker image is specified in the file `.devcontainer/Dockerfile`.

We will be able to use the image from the command line or from the VSCode development environment.

* In Visual Studio Code, install extension "Dev Container".
* In Visual Studio Code, open folder <the downloaded folder>
* Visual Studio will show a message to reopen the folder in a container (because it detects the ".devcontainer"). 
* Open the folder in a container 
    - Visual Studio will generate the docker image and connect the environment to the container.
    - This task may take a few minutes.

Now you can start working. 

Note: Once the image and container has been generated, you can open the folder directly in the container. 

### Using Docker from the command line

To build the image you can just do:

```bash
docker build -f .devcontainer/Dockerfile -t ssoo_docker_env .
```

If you want run an interactive comand line interpreter inside the container you can do:

```bash
docker run --rm -v $(pwd):/workspace -w /workspace -it ssoo_docker_env
```

You will be able to run commands inside the container. Try the following commands:

```bash
gcc --version
cmake --version
echo "Everything is ready to start working"
```



