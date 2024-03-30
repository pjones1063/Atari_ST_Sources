/*********************************************************************
*
* Mag!X 3.00
* ==========
*
* Strukturen f�r die Einbindung eines DFS.
* Die korrekten Prototypen f�r die Implementation eines DFS
* in 'C' folgen, so wie die 'C'- Schnittstelle fertig ist.
*
* Da z.T. mehrere Register f�r R�ckgabewerte verwendet werden und
* au�erdem Zeiger in Datenregistern �bergeben werden, ist eine
* Schnittstelle als _cdecl geplant, d.h. s�mtliche Parameter werden
* auf dem Stapel �bergeben, dies erm�glicht die Verwendung eines
* beliebigen Compilers.
*
* Version: 4.4.94
*
*********************************************************************/

typedef struct _mx_ddev {
     LONG cdecl     (*ddev_open)   (struct _mx_dosfd *f);
     LONG cdecl     (*ddev_close)();
     LONG cdecl     (*ddev_read)();
     LONG cdecl     (*ddev_write)();
     LONG cdecl     (*ddev_stat)();
     LONG cdecl     (*ddev_seek)();
     LONG cdecl     (*ddev_datime)();
     LONG cdecl     (*ddev_ioctl)();
     LONG cdecl     (*ddev_delete)();
     LONG cdecl     (*ddev_getc)();
     LONG cdecl     (*ddev_getline)();
     LONG cdecl     (*ddev_putc)();
} MX_DDEV;


typedef struct _mx_dosfd {
     MX_DMD    *fd_dmd;
     WORD      fd_refcnt;
     WORD      fd_mode;
     MX_DEV    *fd_dev;
     MX_DDEV   *fd_ddev;
     char      fd_name[11];
     char      fd_attr;
     PD        *fd_owner;
     struct _mx_dosfd  *fd_parent;
     struct _mx_dosfd  *fd_children;
     struct _mx_dosfd  *fd_next;
     struct _mx_dosfd  *fd_multi;
     struct _mx_dosfd  *fd_multi1;
     ULONG     fd_fpos;
     char      fd_dirch;
     char      fd_unused;
     WORD      fd_time;
     WORD      fd_date;
     WORD      fd_stcl;
     ULONG     fd_len;
     ULONG     fd_dirpos;
     ULONG     fd_user1;
     ULONG     fd_user2;
} MX_DOSFD;

typedef struct _mx_dosdta {
     char      dta_sname[12];
     ULONG     dta_usr1;
     ULONG     dta_usr2;
     char      dta_drive;
     char      dta_attr;
     WORD      dta_time;
     WORD      dta_date;
     ULONG     dta_len;
     char      dta_name[14];
} MX_DOSDTA;


typedef struct _mx_dosdmd {
     MX_XFS    *d_xfs;
     WORD      d_drive;
     MX_DOSFD  *d_root;
     WORD      biosdev;
     LONG      driver;
     LONG      devcode;
     struct _mx_dfs    *d_dfs;
     WORD		d_flags;
} MX_DOSDMD;


typedef struct _mx_dosdir {
     char      dir_name[11];
     char      dir_attr;
     WORD      dir_usr1;
     ULONG     dir_usr2;
     ULONG     dir_usr3;
     WORD      dir_time;
     WORD      dir_date;
     WORD      dir_stcl;
     ULONG     dir_flen;
} MX_DOSDIR;



typedef struct _mx_dfs {
     char      dfs_name[8];
     struct _mx_dfs   *dfs_next;
     long      (*dfs_init)();
     long      (*dfs_sync)();
     long      (*dfs_drv_open)();
     long      (*dfs_drv_close)();
     long      (*dfs_dfree)();
     long      (*dfs_sfirst)();
     long      (*dfs_snext)();
     long      (*dfs_ext_fd)();
     long      (*dfs_fcreate)();
     long      (*dfs_fxattr)();
     long      (*dfs_dir2index)();
     long      (*dfs_readlink)();
     long      (*dfs_dir2FD)();
     long      (*dfs_fdelete)();
     long      (*dfs_pathconf)();
} MX_DFS;

/* unterst�tzte Dcntl- Modi */
#define   DFS_GETINFO    0x1100
#define   DFS_INSTDFS    0x1200
#define   DEV_M_INSTALL  0xcd00
