/*
 * this is a copy of the sys.h of the linux kernel,
 * linux kernel version 0.0.1.
 * NOTES ADD BY Phoenixfirewingz
 *************************************
 * other people need notes nufflee 
 * no matter how readable the code is.
 *************************************
 * this ones with descriptions are the required 
 * system calls to make the echo lib work
 */

// ***
extern int sys_setup();
// exits a process
extern int sys_exit();
// creates a new process 
extern int sys_fork();
// reads from a file
extern int sys_read();
// writes to a file
extern int sys_write();
// opens a file for reading or writing 
extern int sys_open();
// closes a file descriptor
extern int sys_close();
// ***
extern int sys_waitpid();
// ***
extern int sys_creat();
// links a new file name to an existing file
extern int sys_link();
// removes directory entry
extern int sys_unlink();
// executes a file 
extern int sys_execve();
// changes the working directory
extern int sys_chdir();
// number of seconds since 1/1/1970 
extern int sys_time();
// makes a directory, special or ordinary file 
extern int sys_mknod();
// changes the mode of a file
extern int sys_chmod();
// changes the ownership of a file 
extern int sys_chown();
// ***
extern int sys_break();
// gets file status
extern int sys_stat();
//  moves read/write file pointer
extern int sys_lseek();
// gets group and process IDs 
extern int sys_getpid();
// mounts a filesystem
extern int sys_mount();
// unmounts a file system
extern int sys_umount();
// sets user and group IDs 
extern int sys_setuid();
// gets user and group IDs 
extern int sys_getuid();
// ***
extern int sys_stime();
// allows a process to trace the execution of another
extern int sys_ptrace();
// sets a process's alarm clock
extern int sys_alarm();
// ***
extern int sys_fstat();
// suspends a process until a signal occurs
extern int sys_pause();
// sets file access and modification times
extern int sys_utime();
// ***
extern int sys_stty();
// ***
extern int sys_gtty();
// checks if calling process has file access
extern int sys_access();
// ***
extern int sys_nice();
// ***
extern int sys_ftime();
// updates the super block
extern int sys_sync();
// sends a signal to one or more processes
extern int sys_kill();
// ***
extern int sys_rename();
// ***
extern int sys_mkdir();
// ***
extern int sys_rmdir();
// duplicates an open file descriptor
extern int sys_dup();
// creates an interprocess pipe
extern int sys_pipe();
// gets process and child process times
extern int sys_times();
// requests an execution profile
extern int sys_prof();
// ***
extern int sys_brk();
// sets user and group IDs 
extern int sys_setgid();
// gets user and group IDs 
extern int sys_getgid();
// control of signal processing
extern int sys_signal();
// gets user and group IDs
extern int sys_geteuid();
// gets user and group IDs
extern int sys_getegid();
// ***
extern int sys_acct();
// ***
extern int sys_phys();
// ***
extern int sys_lock();
// controls character devices
extern int sys_ioctl();
// controls open files
extern int sys_fcntl();
// ***
extern int sys_mpx();
// ***
extern int sys_setpgid();
// gets and sets user limit
extern int sys_ulimit();
// gets system information
extern int sys_uname();
// gets and sets file creation mask
extern int sys_umask();
// changes the root directory 
extern int sys_chroot();
// gets file system statistics
extern int sys_ustat();
// duplicates an open file descriptor
extern int sys_dup2();
// gets group and process IDs
extern int sys_getppid();
// gets group and process IDs
extern int sys_getpgrp();
// ***
extern int sys_setsid();

//this was edited to be a stant alone table

void* sys_call_table[] = { sys_setup, sys_exit, sys_fork, sys_read,
sys_write, sys_open, sys_close, sys_waitpid, sys_creat, sys_link,
sys_unlink, sys_execve, sys_chdir, sys_time, sys_mknod, sys_chmod,
sys_chown, sys_break, sys_stat, sys_lseek, sys_getpid, sys_mount,
sys_umount, sys_setuid, sys_getuid, sys_stime, sys_ptrace, sys_alarm,
sys_fstat, sys_pause, sys_utime, sys_stty, sys_gtty, sys_access,
sys_nice, sys_ftime, sys_sync, sys_kill, sys_rename, sys_mkdir,
sys_rmdir, sys_dup, sys_pipe, sys_times, sys_prof, sys_brk, sys_setgid,
sys_getgid, sys_signal, sys_geteuid, sys_getegid, sys_acct, sys_phys,
sys_lock, sys_ioctl, sys_fcntl, sys_mpx, sys_setpgid, sys_ulimit,
sys_uname, sys_umask, sys_chroot, sys_ustat, sys_dup2, sys_getppid,
sys_getpgrp,sys_setsid};