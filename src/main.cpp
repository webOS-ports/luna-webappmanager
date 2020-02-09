/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <QDebug>
#include <QStringList>
#include <QTime>
#include <QtGlobal>

#include <glib.h>
#include <systemd/sd-daemon.h>

#include <LocalePreferences.h>
#include <Settings.h>

#include "webappmanager.h"
#include "systemtime.h"

#define VERSION "0.1"
#define XDG_RUNTIME_DIR_DEFAULT "/tmp/luna-session"

static gboolean option_version = FALSE;
static gboolean option_verbose = FALSE;
static gboolean option_systemd = FALSE;
static gboolean option_allowfiles = TRUE;

static GOptionEntry options[] = {
    { "verbose", 0, 0, G_OPTION_ARG_NONE, &option_verbose, "Enable verbose logging" },
    { "version", 'v', 0, G_OPTION_ARG_NONE, &option_version,
        "Show version information and exit" },
    { "systemd", 0, 0, G_OPTION_ARG_NONE, &option_systemd, "Start with systemd support" },
    { "allow-file-access-from-files", 0, 0, G_OPTION_ARG_NONE, &option_allowfiles, "Allow file access from files" },
    { NULL },
};

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString timeStr = QTime::currentTime().toString("hh:mm:ss.zzz");

    switch(type)
    {
    case QtDebugMsg:
        if (option_verbose)
            fprintf(stderr, "DEBUG: %s: %s\n", timeStr.toUtf8().constData(), msg.toUtf8().constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "WARNING: %s: %s\n", timeStr.toUtf8().constData(), msg.toUtf8().constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "CRITICAL: %s: %s\n", timeStr.toUtf8().constData(), msg.toUtf8().constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "FATAL: %s: %s\n", timeStr.toUtf8().constData(), msg.toUtf8().constData());
        break;
    default:
        fprintf(stderr, "INFO: %s: %s\n", timeStr.toUtf8().constData(), msg.toUtf8().constData());
        break;
    }
}

int main(int argc, char **argv)
{
    GError *error = NULL;
    GOptionContext *context;

    qInstallMessageHandler(messageHandler);

    if (qgetenv("DISPLAY").isEmpty()) {
        setenv("EGL_PLATFORM", "wayland", 0);
        setenv("QT_QPA_PLATFORM", "wayland", 0);
        setenv("XDG_RUNTIME_DIR", XDG_RUNTIME_DIR_DEFAULT, 0);
        setenv("QT_IM_MODULE", "Maliit", 0);
        setenv("QT_WAYLAND_DISABLE_WINDOWDECORATION", "1", 0);
//        setenv("QT_SCALE_FACTOR", QString::number(Settings::LunaSettings()->layoutScale, 'f').toLatin1().constData(), 0);
    }

    QString storagePath = "/media/cryptofs/.sysmgr";
    QString storagePathEnv = qgetenv("PERSISTENT_STORAGE_PATH");
    if (!storagePathEnv.isEmpty())
        storagePath = storagePathEnv;

    if (qgetenv("XDG_DATA_HOME").isEmpty()) {
        QString dataDir = QString("%1/data").arg(storagePath);
        setenv("XDG_DATA_HOME", dataDir.toUtf8().constData(), 1);
    }

    if (qgetenv("XDG_CACHE_HOME").isEmpty()) {
        QString cacheDir = QString("%1/cache").arg(storagePath);
        setenv("XDG_CACHE_HOME", cacheDir.toUtf8().constData(), 1);
    }

    luna::WebAppManager webAppManager(argc, argv);

    context = g_option_context_new(NULL);
    g_option_context_add_main_entries(context, options, NULL);

    /*
     * the g_option_context_parse will modify argc and argv, but Qt keeps a pointer
     * on these references. So we only give copies of argc and argv to g_option_context_parse.
     * */
    int _argc = argc;
    char **_argv = new char*[argc+1];
    for(int _i=0; _i<argc; ++_i) {
        _argv[_i] = argv[_i];
    }
    _argv[argc]=NULL;

    if (!g_option_context_parse(context, &_argc, &_argv, &error)) {
        if (error) {
            g_printerr("%s\n", error->message);
            g_error_free(error);
        }
        else
            g_printerr("An unknown error occurred\n");
        exit(1);
    }

    g_option_context_free(context);
    delete[] _argv; _argv = NULL; _argc = 0;

    if (option_version) {
        g_message("LunaWebAppManager %s", VERSION);
        goto cleanup;
    }

    if (QFile::exists("/var/luna/dev-mode-enabled"))
        setenv("QTWEBENGINE_REMOTE_DEBUGGING", "1122", 0);

    LocalePreferences::instance();
    luna::SystemTime::instance();

    if (option_systemd)
        sd_notify(0, "READY=1");

    webAppManager.exec();

cleanup:

    return 0;
}
