/*
 * This file was originally distributed as part of
 * shadow-utils-4.0.17-12.fc6.src.rpm and has been modified
 * in WRLinux for inclusion in rpm.
 */

extern struct group *__gr_dup (const struct group *);
extern void __gr_set_changed (void);
extern int gr_close (void);
extern const struct group *gr_locate (const char *);
extern int gr_lock (void);
extern int gr_name (const char *);
extern const struct group *gr_next (void);
extern int gr_open (int);
extern int gr_remove (const char *);
extern int gr_rewind (void);
extern int gr_unlock (void);
extern int gr_update (const struct group *);
extern int gr_sort (void);
