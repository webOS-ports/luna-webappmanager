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
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>

#include <QtWebKit/private/qquickwebview_p.h>
#ifndef WITH_UNMODIFIED_QTWEBKIT
#include <QtWebKit/private/qwebnewpagerequest_p.h>
#endif
#include <QtWebKit/private/qwebloadrequest_p.h>

#include <applicationenvironment.h>

#include "webapplicationwindowbase.h"

class QQuickView;
class QQuickItem;

namespace luna
{

class BaseExtension;
class WebApplication;

enum TrustScope
{
    TrustScopeRemote = 0,
    TrustScopeSystem,
};

class WebApplicationWindow : public WebApplicationWindowBase,
                             public ApplicationEnvironment
{
    Q_OBJECT
    Q_PROPERTY(WebApplication *application READ application)
    Q_PROPERTY(QList<QUrl> userScripts READ userScripts)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QString trustScope READ trustScope CONSTANT)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(bool loadingAnimationDisabled READ loadingAnimationDisabled CONSTANT)
    Q_PROPERTY(QString windowType READ windowType CONSTANT)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
    Q_PROPERTY(bool focus READ hasFocus NOTIFY focusChanged)

public:
    explicit WebApplicationWindow(WebApplication *application, const QUrl& url, const QString& windowType,
                                  const QSize& size, bool headless = false, int parentWindowId = 0, QObject *parent = 0);
    ~WebApplicationWindow();

    WebApplication *application() const;

    void stagePreparing();
    void stageReady();

    void show();
    void hide();
    void focus();
    void unfocus();

    bool ready() const;
    bool headless() const;
    bool keepAlive() const;
    QQuickWebView *webView() const;
    QSize size() const;
    bool active() const;
    QString trustScope() const;
    QUrl url() const;
    int windowId() const;
    int parentWindowId() const;
    bool loadingAnimationDisabled() const;
    QString windowType() const;
    bool visible() const;
    bool hasFocus() const;

    QQmlEngine* qmlEngine() const;
    QQuickItem* rootItem() const;

    QList<QUrl> userScripts() const;

    void setKeepAlive(bool alive);

    void executeScript(const QString &script);
    void registerUserScript(const QUrl &path);

    QString getIdentifierForFrame(const QString& id, const QString& url);

    void clearMemoryCaches();

    void destroy();

    Q_INVOKABLE void configureWebView(QQuickItem *webViewItem);

Q_SIGNALS:
    void javaScriptExecNeeded(const QString &script);
    void extensionWantsToBeAdded(const QString &name, QObject *object);
    void closed();
    void readyChanged();
    void sizeChanged();
    void urlChanged();
    void visibleChanged();
    void focusChanged();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private Q_SLOTS:
#ifndef WITH_UNMODIFIED_QTWEBKIT
    void onCreateNewPage(QWebNewPageRequest *request);
    void onClosePage();
    void onSyncMessageReceived(const QVariantMap& message, QString& response);
#endif
    void onLoadingChanged(QWebLoadRequest *request);
    void onStageReadyTimeout();
    void onVisibleChanged(bool visible);
    void onWindowPropertyChanged(QPlatformWindow *window, const QString &name);

private:
    WebApplication *mApplication;
    QMap<QString, BaseExtension*> mExtensions;
    QQmlEngine *mEngine;
    QQuickItem *mRootItem;
    QQuickView *mWindow;
    bool mHeadless;
    QQuickWebView *mWebView;
    QUrl mUrl;
    QString mWindowType;
    bool mKeepAlive;
    bool mStagePreparing;
    bool mStageReady;
    QTimer mStageReadyTimer;
    QList<QUrl> mUserScripts;
    QSize mSize;
    TrustScope mTrustScope;
    int mWindowId;
    int mParentWindowId;
    bool mLoadingAnimationDisabled;
    bool mLaunchedHidden;

    void assignCorrectTrustScope();
    void createAndSetup();
    void configureQmlEngine();
    void loadAllExtensions();
    void addExtension(BaseExtension *extension);
    void createDefaultExtensions();
    void setWindowProperty(const QString &name, const QVariant &value);
    QVariant getWindowProperty(const QString &name);
    void updateWindowProperty(const QString &name);
    void setupPage();
    void notifyAppAboutFocusState(bool focus);
};

} // namespace luna

#endif // WEBAPPLICATIONWINDOW_H
