# Hey! I'm Filing Here

In this lab, I successfully implemented the creation of a 1 MiB ext2 file system with the following structure:
- **2 directories**: The root directory (`/`) and `lost+found`.
- **1 regular file**: `hello-world`, which contains the string `"Hello world\n"` and is 12 bytes long.
- **1 symbolic link**: `hello`, which points to the `hello-world` file.

## Building

To build the project, navigate to the `lab4` directory and run the following command:

```bash
make
```

This will compile the code and generate the `ext2-create` executable.

## Running

To create the ext2 filesystem image:

Run the executable:

```bash
./ext2-create
```

This will generate a file named cs111-base.img in the current directory.

To verify the filesystem:

Run the fsck.ext2 command:
```bash
fsck.ext2 cs111-base.img
```
Ensure there are no errors reported after running this command.
Dump the filesystem information using:
```bash
dumpe2fs cs111-base.img
```
To mount the filesystem for inspection:

Create a directory to serve as the mount point:
```bash
mkdir mnt
```
Mount the filesystem image:
```bash
sudo mount -o loop cs111-base.img mnt
```
Navigate to the mounted directory and verify its structure using:
```bash
ls -ain mnt/
```

You should see the following output:

```yaml
total 7
2 drwxr-xr-x 3 0 0 1024 .
13 lrw-r--r-- 1 1000 1000 11 hello -> hello-world
12 -rw-r--r-- 1 1000 1000 12 hello-world
11 drwxr-xr-x 2 0 0 1024 lost+found
```

## Cleaning up

To clean up the build files:

Run the following commands in the lab4 directory:

To unmount the filesystem:

```bash
sudo umount mnt
```

The following will remove all compiled binaries and object files.
```bash
make clean
```

Optionally, delete the mount point directory:
```bash
rmdir mnt
```
