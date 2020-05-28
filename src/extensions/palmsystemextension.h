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

#include <QString>
#include <QSharedPointer>

#include <baseextension.h>
#include <luna-service2++/handle.hpp>

namespace luna
{

class WebApplicationWindow;

class PalmSystemExtension : public BaseExtension
{
    Q_OBJECT

    Q_PROPERTY(QString launchParams READ launchParams NOTIFY launchParamsChanged)
    Q_PROPERTY(bool hasAlphaHole READ hasAlphaHole WRITE setHasAlphaHole NOTIFY hasAlphaHoleChanged)
    Q_PROPERTY(QString locale READ locale CONSTANT)
    Q_PROPERTY(QString localeRegion READ localeRegion CONSTANT)
    Q_PROPERTY(QString timeFormat READ timeFormat CONSTANT)
    Q_PROPERTY(QString timeZone READ timeZone CONSTANT)
    Q_PROPERTY(bool isMinimal READ isMinimal CONSTANT)
    Q_PROPERTY(QString identifier READ identifier CONSTANT)
    Q_PROPERTY(QString screenOrientation READ screenOrientation CONSTANT)
    Q_PROPERTY(QString windowOrientation READ windowOrientation WRITE setWindowOrientation NOTIFY windowOrientationChanged)
    Q_PROPERTY(QString specifiedWindowOrientation READ specifiedWindowOrientation CONSTANT)
    Q_PROPERTY(QString videoOrientation READ videoOrientation CONSTANT)
    Q_PROPERTY(QString deviceInfo READ deviceInfo CONSTANT)
    Q_PROPERTY(bool isActivated READ isActivated NOTIFY isActivatedChanged)
    Q_PROPERTY(int activityId READ activityId CONSTANT)
    Q_PROPERTY(QString phoneRegion READ phoneRegion CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)
public:
    explicit PalmSystemExtension(WebApplicationWindow *applicationWindow, QObject *parent = 0);

    Q_INVOKABLE QString getResource(const QString&resPath, const QString &);
    Q_INVOKABLE QString getIdentifierForFrame(const QString&id, const QString &url);
    Q_INVOKABLE QString addBannerMessage(const QString&msgTitle, const QString &launchParams,
                                         const QString&msgIconUrl, const QString &soundClass,
                                         const QString&soundFile, int duration,
                                         bool doNotSuppress);

    Q_INVOKABLE void LS2Call(int callId, int bridgeId, const QString &uri, const QString &payload);
    Q_INVOKABLE void LS2Cancel(int bridgeId);

public Q_SLOTS:

    void activate();
    void deactivate();
    void stagePreparing();
    void stageReady();
    void show();
    void hide();
    void setWindowProperties(const QString &properties);
    void enableFullScreenMode(bool enable);
    void removeBannerMessage(int id);
    void clearBannerMessages();
    void keepAlive(bool keep);
    void markFirstUseDone();

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
    void enableFullScreenMode(bool enable);
    void setAlertSound(const QString& soundClass, const QString& soundFile = "");
    void receivePageUpDownInLandscape(bool enable);
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

    QString launchParams();
    bool    hasAlphaHole();
    void    setHasAlphaHole(bool iVal);
    QString locale();
    QString localeRegion();
    QString timeFormat();
    QString timeZone();
    bool    isMinimal();
    QString identifier();
    QString screenOrientation();
    QString windowOrientation();
    void    setWindowOrientation(QString iVal);
    QString specifiedWindowOrientation();
    QString videoOrientation();
    QString deviceInfo();
    bool    isActivated();
    int     activityId();
    QString phoneRegion();
    QString version();

Q_SIGNALS:
    void hasAlphaHoleChanged();
    void windowOrientationChanged();
    void isActivatedChanged();
    void launchParamsChanged(bool needRelaunch);

    void palmBridgeServiceCall(QString body);
private:
    WebApplicationWindow *mApplicationWindow;
    LS::Handle mLunaAppHandle;

    LS::Handle &getLunaHandle();

    class PalmServiceBridgeObject {
    public:
        PalmServiceBridgeObject():
            bridgeId(0),
            callId(0),
            palmExt(nullptr),
            currentBridgeCall(nullptr) {}
        PalmServiceBridgeObject(const PalmServiceBridgeObject &other):
            bridgeId(other.bridgeId),
            callId(other.callId),
            palmExt(other.palmExt),
            currentBridgeCall(other.currentBridgeCall) {}
        PalmServiceBridgeObject &operator=(const PalmServiceBridgeObject &other) {
            bridgeId = other.bridgeId;
            callId = other.callId;
            palmExt = other.palmExt;
            currentBridgeCall = other.currentBridgeCall;
            return *this;
        }
        int bridgeId;
        int callId;
        PalmSystemExtension *palmExt;
        QSharedPointer<LS::Call> currentBridgeCall;

        bool handleReply(LSHandle *sh, LSMessage *reply);
    };
    QHash<int, PalmServiceBridgeObject> mListBridges;

    static bool replyCallback(LSHandle* sh, LSMessage* reply, void* context);
};

} // namespace luna

#endif // PALMSYSTEMPLUGIN_H
