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

#ifndef PALMSYSTEMPLUGIN_H
#define PALMSYSTEMPLUGIN_H

#include "baseplugin.h"

namespace luna
{

class WebApplication;

class PalmSystemPlugin : public BasePlugin
{
    Q_OBJECT
public:
    explicit PalmSystemPlugin(WebApplicationWindow *applicationWindow, QObject *parent = 0);

    QString handleSynchronousCall(const QString& funcName, const QJsonArray& params);

public slots:

    void activate();
    void deactivate();
    void stagePreparing();
    void stageReady();
    void show();
    void hide();
    void setWindowProperties(const QString &properties);
    void enableFullScreenMode(bool enable);
    void addBannerMessage(int id, const QString &msg, const QString &params,
                          const QString &icon, const QString &soundClass, const QString &soundFile, int duration,
                          bool doNotSuppress);
    void removeBannerMessage(int id);
    void clearBannerMessages();
    void keepAlive(bool keep);

    /*
    void playSoundNotification(const QString& soundClass, const QString& soundFile = "",
                                        int duration = -1, bool wakeUpScreen = false);
    void simulateMouseClick(int pageX, int pageY, bool pressed);
    void paste();
    void copiedToClipboard();
    void pastedFromClipboard();
    void setWindowOrientation(const QString& orientation);
    QString encrypt(const QString& key, const QString& str);
    QString decrypt(const QString& key, const QString& str);
    void shutdown();
    void markFirstUseDone();
    void enableFullScreenMode(bool enable);
    void activate();
    void deactivate();
    void stagePreparing();
    void stageReady();
    void setAlertSound(const QString& soundClass, const QString& soundFile = "");
    void receivePageUpDownInLandscape(bool enable);
    void show();
    void hide();
    void enableDockMode(bool enable);
    QString getLocalizedString(const QString& str);
    QString addNewContentIndicator();
    void removeNewContentIndicator(const QString& requestId);
    void runAnimationLoop(const QVariantMap& domObj, const QString& onStep,
                                    const QString& onComplete, const QString& curve, qreal duration,
                                    qreal start, qreal end);
    void setActiveBannerWindowWidth();
    void cancelVibrations();
    void setWindowProperties(const QVariantMap& properties);
    bool addActiveCallBanner(const QString& icon, const QString& message, quint32 timeStart);
    void removeActiveCallBanner();
    bool updateActiveCallBanner(const QString& icon, const QString& message, quint32 timeStart);
    void applyLaunchFeedback(int offsetX, int offsetY);
    void launcherReady();
    QString getDeviceKeys(int key);
    void repaint();
    void hideSpellingWidget();
    void printFrame(const QString& frameName, int lpsJobId, int widthPx,
                                int heightPx, int printDpi, bool landscape, bool reverseOrder = false);
    void editorFocused(bool focused, int fieldType, int fieldActions);
    void allowResizeOnPositiveSpaceChange(bool allowResize);
    void useSimulatedMouseClicks(bool uses);
    void handleTapAndHoldEvent(int pageX, int pageY);
    void setManualKeyboardEnabled(bool enabled);
    void keyboardShow(int fieldType);
    void keyboardHide();
    QVariant getResource(QVariant a, QVariant b);
    */

    void registerPropertyChangeHandler(int successCallbackId, int errorCallbackId);
    void setProperty(const QString &name, const QVariant &value);
    void getProperty(int successCallbackId, int errorCallbackId, const QString &name);
    void initializeProperties(int successCallbackId, int errorCallbackId);

private:
    int mPropertyChangeHandlerCallbackId;

    QString getResource(const QJsonArray& params);
};

} // namespace luna

#endif // PALMSYSTEMPLUGIN_H
