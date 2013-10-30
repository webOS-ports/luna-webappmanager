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

#ifndef WEBAPPLICATIONWINDOW_H
#define WEBAPPLICATIONWINDOW_H

#include <QObject>
#include <QQmlEngine>
#include <QQuickWindow>

#include <QtWebKit/private/qquickwebview_p.h>
#include <QtWebKit/private/qwebnewpagerequest_p.h>

namespace luna
{

class BasePlugin;
class WebApplication;

class WebApplicationWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WebApplication *application READ application)

public:
    explicit WebApplicationWindow(WebApplication *application, const QUrl& url, const QString& windowType,
                                  bool headless = false, QObject *parent = 0);
    ~WebApplicationWindow();

    WebApplication *application() const;

    void show();
    void hide();

    bool headless() const;
    bool keepAlive() const;
    QQuickWebView *webView() const;

    void setKeepAlive(bool alive);

public slots:
    void executeScript(const QString &script);

signals:
    void javaScriptExecNeeded(const QString &script);
    void pluginWantsToBeAdded(const QString &name, QObject *object);
    void closed();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void onCreateNewPage(QWebNewPageRequest *request);
    void onSyncMessageReceived(const QVariantMap& message, QString& response);
    void onClosed();

private:
    WebApplication *mApplication;
    QMap<QString, BasePlugin*> mPlugins;
    QQmlEngine mEngine;
    QObject *mRootItem;
    QQuickWindow *mWindow;
    bool mHeadless;
    QQuickWebView *mWebView;
    QUrl mUrl;
    QString mWindowType;
    bool mKeepAlive;

    void createAndSetup();
    void createPlugins();
    void createAndInitializePlugin(BasePlugin *plugin);
    void setWindowProperty(const QString &name, const QVariant &value);
};

} // namespace luna

#endif // WEBAPPLICATIONWINDOW_H
