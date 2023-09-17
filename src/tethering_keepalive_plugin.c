/*
 *  Connection Manager plugin
 *
 *  Copyright (C) 2018 Jolla Ltd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <connman/notifier.h>
#include <connman/plugin.h>
#include <connman/log.h>

#include <fcntl.h>
#include <errno.h>

/** Path to kernel wakelock obtain sysfs file */
static const char sysfs_lock_path[] = "/sys/power/wake_lock";

/** Path to kernel wakelock release sysfs file */
static const char sysfs_unlock_path[] = "/sys/power/wake_unlock";

/** Name of the multiplexed "real" wakelock */
static const char lock_name[] = "connman_tethering";

/** Flag for: "real" wakelock is held */
static bool lock_locked = false;

static bool sysfs_write(const char *path, const char *data, int size);

/*
 * Here is how it's supposed to work:
 *
 * Tethering on
 * ============
 *
 * 1. Write "connman_tethering" to /sys/power/wake_lock
 *
 * Then we can proceed and let sailfish_wifi plugin to finish the job.
 *
 * Tethering off
 * =============
 *
 * 1. Write "connman_tethering" to /sys/power/wake_unlock
 *
 */

/** Helper for writing to sysfs files
 */
static bool
sysfs_write(const char *path, const char *data, int size)
{
    bool res = false;
    int  fd  = -1;

    if( !path || !data || size <= 0 )
        goto cleanup;

    if( (fd = open(path, O_WRONLY)) == -1 )
        goto cleanup;

    if( write(fd, data, size) == -1 )
        goto cleanup;

    res = true;

cleanup:
    if( fd != -1 ) close(fd);

    return res;
}

/** Async signal safe wakelock obtain
 */
static bool
lock_lock(void)
{
    return sysfs_write(sysfs_lock_path, lock_name,
                           sizeof lock_name - 1);
}

/** Async signal safe wakelock release
 */
static bool
lock_unlock(void)
{
    return sysfs_write(sysfs_unlock_path, lock_name,
                           sizeof lock_name - 1);
}

/** Set wakelock state
 *
 * @param lock  true to obtain real wakelock, false to release
 */
static void
tethering_lock_set(bool lock)
{
    if( lock_locked == lock )
        goto EXIT;

    errno = 0;

    if( (lock_locked = lock) ) {
        if( !lock_lock() )
            connman_info("failed to obtain wakelock");
    }
    else {
        if( !lock_unlock() )
            connman_info("failed to release wakelock");
    }

EXIT:
    return;
}

static
void
tethering_changed_notify(
    struct connman_technology* tech,
    bool on)
{
    if (on) {
        DBG("Tethering on");
        tethering_lock_set(true);

    } else {
        DBG("Tethering off");
        tethering_lock_set(false);
    }
}

struct connman_notifier tethering_plugin_notifier = {
    .name = "keepalive tethering notifier",
    .tethering_changed = tethering_changed_notify
};

static
int
tethering_plugin_init(
    void)
{
    connman_info("Initializing keepalive tethering plugin.");
    connman_notifier_register(&tethering_plugin_notifier);
    return 0;
}

static
void
tethering_plugin_exit(
    void)
{
    DBG("");
    connman_notifier_unregister(&tethering_plugin_notifier);
}

CONNMAN_PLUGIN_DEFINE(tethering_plugin, "keepalive tethering plugin",
    CONNMAN_VERSION, CONNMAN_PLUGIN_PRIORITY_DEFAULT,
    tethering_plugin_init, tethering_plugin_exit)

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
